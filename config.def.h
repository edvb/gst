char *gist  = NULL;                           /* ID of existing gist to edit */
char *desc  = NULL;                           /* gist description */
char *del   = NULL;                           /* file name to delete from gist */
char *fname = NULL;                           /* gist filename when reading from stdin */
char *token = NULL;                           /* GitHub authentication token */
char *tfile = NULL;                           /* path to file containing token */
char *ghurl = "https://api.github.com/gists"; /* GitHub URL to post gists to */
int pub = 1;                                  /* 0: private need link to view, 1: public anyone can find */
