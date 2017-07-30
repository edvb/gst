char *desc  = "";                             /* Gist description */
char *fname = NULL;                           /* Gist filename when reading from stdin */
char *user  = NULL;                           /* GitHub user, password can be given after colon */
char *ghurl = "https://api.github.com/gists"; /* default GitHub URL to post gists to */
int pub = 1;                                  /* 0: private need link to view, 1: public anyone can find */
