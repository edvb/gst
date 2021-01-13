/* See LICENSE file for copyright and license details. */
#include <curl/curl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "extern/arg.h"
#include "extern/frozen.h"

/* defines */
#define LBUF_SIZE 1024
#define BUF_SIZE  100000
#define URL_SIZE  2048

/* typedefs */
typedef struct {
	char *ptr;
	size_t len;
} Str;

/* variables */
char *argv0;

static char *file_str(FILE *fp);

#include "config.h"

void
die(int eval, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}

	if (eval > -1)
		exit(eval);
}
/* used by cURL to write its response to a Str */
static size_t
str_write(void *ptr, size_t size, size_t nmemb, Str *s)
{
	size_t nlen = s->len + size*nmemb;
	if (!(s->ptr = realloc(s->ptr, nlen+1)))
		die(1, "realloc:");
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[nlen] = '\0';
	s->len = nlen;

	return size*nmemb;
}

/* HTTP POST request with content, returning response */
static Str
http_post(char *content)
{
	char *resmsg, url[URL_SIZE], *tokenstr;
	long code;
	CURL *curl;
	CURLcode res;
	Str resstr;
	struct curl_slist *chunk = NULL;

	resstr.len = 0;
	if (!(resstr.ptr = malloc(resstr.len+1)))
		die(1, "malloc:");
	resstr.ptr[0] = '\0';

	/* init cURL */
	curl_global_init(CURL_GLOBAL_ALL);
	if (!(curl = curl_easy_init())) {
		curl_global_cleanup();
		die(1, "%s: cURL: could not init", argv0);
	}

	/* set cURL options */
	if (gist)
		/* TODO if given URL split to get ID */
		snprintf(url, sizeof(url), "%s/%s", ghurl, gist);
	else
		strcpy(url, ghurl);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "gst/"VERSION);
	if (gist)
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, str_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resstr);
	if (tfile) {
		FILE *fp = fopen(tfile, "r");
		if (!fp) die(1, "%s: %s: could not load file", argv0, tfile);
		token = file_str(fp);
		fclose(fp);
	}
	if (token) {
		if (!(tokenstr = malloc((23+strlen(token))*sizeof(char))))
			die(1, "malloc:");
		strcpy(tokenstr, "Authorization: token ");
		strcat(tokenstr, token);
		chunk = curl_slist_append(chunk, tokenstr);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		free(tokenstr);
		if (tfile) free(token);
	}

	/* run cURL */
	if ((res = curl_easy_perform(curl)) != CURLE_OK) {
		curl_global_cleanup();
		die(1, "%s: cURL: %s", argv0, curl_easy_strerror(res));
	}

	/* response checking and cleanup */
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	/* 200 returned when editing gist, 201 when creating */
	if (code != (gist ? 200 : 201)) {
		json_scanf(resstr.ptr, resstr.len, "{message: %Q}", &resmsg);
		die(-1, "%s: [%d] could not create gist: %s",
		         argv0, code, resmsg);
		free(resmsg);
		exit(1);
	}

	return resstr;
}

/* read file fp into returned string */
static char *
file_str(FILE *fp)
{
	char buf[LBUF_SIZE]; /* buffer for each chuck */
	char *str = calloc(LBUF_SIZE, sizeof(char)); /* complete file */
	size_t flen = 1; /* file length, start at 1 for null terminator */

	if (!str) die(1, "calloc:");
	/* loop through each LBUF_SIZE chunk in file, append it to str */
	while (fgets(buf, LBUF_SIZE, fp)) {
		flen += strlen(buf);
		str = realloc(str, flen);
		if (!str) die(1, "realloc:");
		strncat(str, buf, LBUF_SIZE);
	}

	return str;
}

/* turn list of file names into returned json string */
static char *
files_js(char *files[], int filec)
{
	char *fbuf; /* file contents */
	char *js = malloc(BUF_SIZE*sizeof(char)); /* json string returned */
	FILE *fp = stdin; /* read from stdin by default if no file is given */
	struct json_out jout = JSON_OUT_BUF(js, BUF_SIZE);

	if (!js) die(1, "malloc:");
	json_printf(&jout, "{ public: %B,", pub);
	if (!desc && !gist) /* when creating new gist if no description given make it empty */
		desc = "";
	if (desc) /* only add description as blank if gist is being created */
		json_printf(&jout, "description: %Q, ", desc);
	json_printf(&jout, "files: {");

	if (del) /* delete file if given by setting it to NULL */
		json_printf(&jout, "%Q: %Q", basename(del), NULL);

	/* add each file */
	for (int i = 0; !i || i < filec; i++) {
		if (filec && !(fp = fopen(files[i], "r")))
			die(1, "%s: %s: could not load file", argv0, files[i]);
		if (filec) /* set file name if given */
			fname = files[i];
		if (!fname && gist) /* don't need file if we are editing gist */
			break;
		if (!fname) /* check for file name when using stdin */
			die(1, "%s: file not given", argv0);
		if (i || del) /* insert comma if this is another file */
			json_printf(&jout, ",");
		fbuf = file_str(fp);
		json_printf(&jout, "%Q: { content: %Q }", basename(fname), fbuf);
		fclose(fp);
		free(fbuf);
	}

	json_printf(&jout, "} }");

	return js;
}

static void
usage(const int eval)
{
	die(eval, "usage: %s [-pPhv] [-e ID [-D FILE]] [-d DESCRIPTION] [-f FILENAME]\n"
	          "           [-g URL] [-U | -u TOKENFILE | -T TOKEN] FILES ...", argv0);
}

int
main(int argc, char *argv[])
{
	char *js, *url;
	Str resstr;

	ARGBEGIN {
	case 'e':
		gist = EARGF(usage(1));
		break;
	case 'd':
		desc = EARGF(usage(1));
		break;
	case 'D':
		del = EARGF(usage(1));
		break;
	case 'f':
		fname = EARGF(usage(1));
		break;
	case 'g':
		ghurl = EARGF(usage(1));
		break;
	case 'p':
		pub = 0;
		break;
	case 'P':
		pub = 1;
		break;
	case 'T':
		token = EARGF(usage(1));
		break;
	case 'u':
		tfile = EARGF(usage(1));
		break;
	case 'U':
		tfile = NULL;
		token = NULL;
		break;
	case 'h':
		usage(0);
	case 'v':
		printf("%s v%s (c) 2017-2020 Ed van Bruggen\n", argv0, VERSION);
		return 0;
	default:
		usage(1);
	} ARGEND;

	if (gist && !token && !tfile)
		die(1, "%s: cannot edit gists without token", argv0);

	js = files_js(argv, argc);
	resstr = http_post(js);

	json_scanf(resstr.ptr, resstr.len, "{html_url: %Q}", &url);
	puts(url);

	free(js);
	free(url);
	free(resstr.ptr);

	return 0;
}
