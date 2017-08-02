# gs \- painless gist creator

## SYNOPSIS

**gs** [**-pPhv**] [**-e** *ID* [**-D** *FILE*]] [**-d** *DESCRIPTION*] [**-f** *FILENAME*] [**-g** *GITHUB\_URL*] [**-u** *USER*[: *PASSWORD*] | **-U**] [*FILES* ...]

## DESCRIPTION

Easy way to create GitHub Gists through the command line.

## OPTIONS

#### -e ID

Edit previously created Gist specified with *ID*

#### -d DESCRIPTION

Set Gist description

#### -D FILE

Delete *FILE* from Gist being edited

#### -f FILENAME

Set file name when reading from `stdin`

#### -g GITHUB_URL

Change the GitHub API URL

#### -p

Make Gist private

#### -P

Make Gist public (default)

#### -u USER[:PASSWORD]

Change the GitHub account the Gist will be posted under. A password can
given as well with a separating colon, a prompt is provided if not.

#### -U

Post Gist anonymously (default)

#### -h

Print help and exit

#### -v

Print version info and exit

## USAGE

Create a new Gist of file `file.txt`:

```
$ gs file.txt
https://gist.github.com/<new-id>
```

There is also support for multiple files in a single Gist:

```
$ gs README.md Makefile prog.c
https://gist.github.com/<new-id>
```

Specify the Gist's description:

```
$ gs -d 'a cool shell script' script.sh
https://gist.github.com/<new-id>
```

The Gist's URL can be piped to other programs, for example to your clipboard to
be pasted else where:

```
$ gs file.txt | xsel -bi
```

You can also create a new Gist from `stdin`. The file name needs to be supplied
however:

```
$ cmd-which-errors | gs -f log.txt
https://gist.github.com/<new-id>
```

New Gists can be private as well:

```
$ gs -p personal.info
https://gist.github.com/<new-id>
```

The new Gist can be posted under a GitHub user, a prompt will ask for your
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

## AUTHOR

Ed van Bruggen <edvb54@gmail.com>

## SEE ALSO

See project page at <http://edryd.org/projects/gs.html>

View source code at <https://github.com/edvb/gs>

## LICENSE

zlib License
