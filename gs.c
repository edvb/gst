/* See LICENSE file for copyright and license details. */
#include <curl/curl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "extern/arg.h"
#include "extern/frozen.h"
#include "util.h"

/* defines */
#define LBUF_SIZE 1024   /* TODO remove BUF_SIZEs */
#define BUF_SIZE  100000
#define URL_SIZE  2048

/* typedefs */
typedef struct {
	char *ptr;
	size_t len;
} Str;

/* functions */
static size_t str_write(void *ptr, size_t size, size_t nmemb, Str *s);
static Str http_post(char *content);
static char *file_str(FILE *fp);
static char *files_js(char *files[], int filec);
static void gs_new(char *files[], int filec);

/* variables */
char *argv0;

#include "config.h"

/* used by cURL to write its response to a Str */
static size_t
str_write(void *ptr, size_t size, size_t nmemb, Str *s)
{
	size_t nlen = s->len + size*nmemb;
	s->ptr = erealloc(s->ptr, nlen+1);
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[nlen] = '\0';
	s->len = nlen;

	return size*nmemb;
}

/* HTTP POST request from content, returning response */
static Str
http_post(char *content)
{
	char *resmsg, url[URL_SIZE];
	long code;
	CURL *curl;
	CURLcode res;
	Str resstr;

	resstr.len = 0;
	resstr.ptr = emalloc(resstr.len+1);
	resstr.ptr[0] = '\0';

	/* init cURL */
	curl_global_init(CURL_GLOBAL_ALL);
	if (!(curl = curl_easy_init())) {
		curl_global_cleanup();
		die(1, "%s: cURL: could not init", argv0);
	}

	/* set cURL options */
	if (gist)
		snprintf(url, sizeof(url), "%s/%s", ghurl, gist);
	else
		strcpy(url, ghurl);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "gs/"VERSION);
	if (user) {
		if (strchr(user, ':')) {
			curl_easy_setopt(curl, CURLOPT_USERPWD, user);
		} else {
			curl_easy_setopt(curl, CURLOPT_USERNAME, user);
			curl_easy_setopt(curl, CURLOPT_PASSWORD, getpass("GitHub password: "));
		}
	}
	if (gist)
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, str_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resstr);

	/* run cURL */
	if ((res = curl_easy_perform(curl)) != CURLE_OK) {
		curl_global_cleanup();
		die(1, "%s: cURL: %s", argv0, curl_easy_strerror(res));
	}

	/* response checking and cleanup */
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	if (code != (gist ? 200 : 201)) { /* 200 returned when editing gist, 201 when creating */
		json_scanf(resstr.ptr, resstr.len, "{message: %Q}", &resmsg);
		die(-1, "%s: could not create Gist: %s", argv0, resmsg);
		free(resmsg);
		exit(1);
	}


	return resstr;
}

/* read file fp into returned string */
static char *
file_str(FILE *fp)
{
	char lbuf[LBUF_SIZE];                         /* buffer for each line */
	char *str = ecalloc(LBUF_SIZE, sizeof(char)); /* complete file */
	long flen = 1;                                /* file length */

	/* loop through each line in the file, append it to str */
	while (fgets(lbuf, LBUF_SIZE, fp)) {
		flen += strlen(lbuf);
		str = erealloc(str, flen);
		strcat(str, lbuf);
	}

	return str;
}

/* turn list of file into returned json string */
static char *
files_js(char *files[], int filec)
{
	char *fbuf, *js = emalloc(BUF_SIZE*sizeof(char)); /* fbuf: file contents, js: json string returned */
	int i;
	FILE *fp = stdin;
	struct json_out jout = JSON_OUT_BUF(js, BUF_SIZE);

	json_printf(&jout, "{ public: %B,", pub);
	if (!desc && !gist) /* when creating new Gist, if no description given, make it empty */
		desc = "";
	if (desc)           /* only add description as blank if gist is being created */
		json_printf(&jout, "description: %Q, ", desc);
	json_printf(&jout, "files: {");

	if (del) /* delete file if given by setting it to NULL */
		json_printf(&jout, "%Q: %Q", basename(del), NULL);

	/* add each file */
	for (i = 0; !i || i < filec; i++) {
		if (filec && !(fp = fopen(files[i], "r")))
			die(1, "%s: %s: could not load file", argv0, files[i]);
		if (filec)          /* set file name if given */
			fname = files[i];
		if (!fname && gist) /* don't need file if we are editing gist */
			break;
		if (!fname)         /* check for file name when using stdin */
			die(1, "%s: file name not given", argv0);
		if (i || del)       /* insert comma if this is another file */
			json_printf(&jout, ",");
		fbuf = file_str(fp);
		json_printf(&jout, "%Q: { content: %Q }", basename(fname), fbuf);
		efree(fbuf);
	}

	json_printf(&jout, "} }");

	return js;
}

static void
gs_new(char *files[], int filec)
{
	char *js, *url;
	Str resstr;

	js = files_js(files, filec);

	resstr = http_post(js);

	json_scanf(resstr.ptr, resstr.len, "{html_url: %Q}", &url);
	printf("%s\n", url);

	efree(js);
	efree(url);
	efree(resstr.ptr);

}

static void
usage(const int eval)
{
	die(eval, "usage: %s [-pPhv] [-e ID [-D FILE]] [-d DESCRIPTION] [-f FILENAME]\n"
	          "          [-g GITHUB_URL] [-u USER[:PASSWORD] | -U] FILES ...", argv0);
}

int
main(int argc, char *argv[])
{
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
	case 'u':
		user = EARGF(usage(1));
		break;
	case 'U':
		user = NULL;
		break;
	case 'h':
		usage(0);
	case 'v':
		printf("%s v%s (c) 2017 Ed van Bruggen\n", argv0, VERSION);
		return 0;
	default:
		usage(1);
	} ARGEND;

	if (gist && !user)
		die(1, "%s: cannot edit Gists without user", argv0);

	gs_new(argv, argc);

	return 0;
}
