# gs \- painless gist creator

Easy way to create GitHub Gists through the command line. Simply give it the
files to upload and gs will return the URL to the new gist.

## Options

#### -e *ID*

Edit previously created gist specified with *ID*

#### -d *DESCRIPTION*

Set gist description

#### -D *FILE*

Delete *FILE* from gist being edited

#### -f *FILENAME*

Set file name when reading from `stdin`

#### -g *URL*

Change the GitHub API URL (default: https://api.github.com/gists)

#### -p

Make gist private

#### -P

Make gist public (default)

#### -u *USER[:PASSWORD]*

Change the GitHub account the gist will be posted under. A password can
given as well with a separating colon, a prompt is provided if not.

#### -U

Post gist anonymously (default)

#### -h

Print help and exit

#### -v

Print version info and exit

## Usage

Create a new gist of `file.txt`:

```
$ gs file.txt
https://gist.github.com/<new-id>
```

There is also support for multiple files in a single gist:

```
$ gs README.md Makefile prog.c
https://gist.github.com/<new-id>
```

Specify the gist's description:

```
$ gs -d 'a cool shell script' script.sh
https://gist.github.com/<new-id>
```

The gist's URL can be piped to other programs, for example to your clipboard to
be pasted elsewhere:

```
$ gs file.txt | xsel -bi
```

You can also create a new gist from `stdin`. The file name needs to be supplied
however:

```
$ cmd-which-errors | gs -f log.txt
https://gist.github.com/<new-id>
```

The new gist can be posted under a GitHub user, a prompt will ask for your
password:

```
$ gs -u your-name good-proj.rs
GitHub password:
https://gist.github.com/<new-id>
```

To skip the prompt your password can be supplied after a colon with the
username:

```
$ gs -u your-name:password plugin/func.vim doc/func.txt
https://gist.github.com/<new-id>
```

New gists can be private when created under a user:

```
$ gs -p -u name personal.info
https://gist.github.com/<new-id>
```

You can also edit a previously created gist under your account, such as adding
a new file or updating the description:

```
$ gs -e <id> -u name -d "new description" additional-file.txt
https://gist.github.com/<id>
```

It is even possible to delete files, although it should be noted the gist still
exists even if you delete all the files within it due to the limitations of the
API:

```
$ gs -e <id> -u name -D old.py
https://gist.github.com/<id>
```

## Installation

The only dependency needed to build gs is cURL. gs be compiled with a simple
`make` and `sudo make install` will install the executable and man page.

## Customization

gs can be customized by editing the custom `config.h` and (re)compiling the
source code. This keeps it fast, secure and simple. All customization can be
done through the command line interface however, so basic aliases could also be
utilized to change default options.

## Author

Ed van Bruggen <ed@edryd.org>

## See Also

See project page at <https://edryd.org/projects/gs.html>

View source code at <https://git.edryd.org/gs>

## License

zlib License
