# gs - gist creator

## SYNOPSIS

`gs` [**-pPhv**] [**-d** *DESCRIPTION*] [**-f** *FILENAME*] [**-u** *USER*[:*PASSWORD*] | **-U**] [*FILES* ...]

## DESCRIPTION

Easy way to create Github gists with the command line.

## OPTIONS

**-d** *DESCRIPTION*
	Set gist description

**-f** *FILENAME*
	Set file name when reading from `stdin`

**-p**
	Make gist private

**-P**
	Make gist public (default)

**-u** *USER*[:*PASSWORD*]
	Change the Github account the gist will be posted under. A password can
	given as well with a separating colon, a prompt is provided if not.

**-U**
	Post gist anonymously (default)

**-h**
	Print help and exit

**-v**
	Print version info and exit

## USAGE

Create a new gist of file `file.txt`:

	$ gs file.txt
	https://gist.github.com/<new-id>

There is also support for multiple files in a single gist:

	$ gs README.md Makefile prog.c
	https://gist.github.com/<new-id>

Specify the gist's description:

	$ gs -d 'a cool shell script' script.sh
	https://gist.github.com/<new-id>

The gist's URL can be piped to other programs, for example to your clipboard to
be pasted else where:

	$ gs file.txt | xsel -bi

You can also create a new gist from `stdin`. The file name needs to be supplied
however:

	$ cmd-which-errors | gs -f log.txt
	https://gist.github.com/<new-id>

New gists can be private as well:

	$ gs -p personal.info
	https://gist.github.com/<new-id>

The new gist can be posted under a Github user, a prompt will ask for your
password:

	$ gs -u your-name good-proj.rs
	Github password:
	https://gist.github.com/<new-id>

To skip the prompt your password can be supplied after a colon with the
username:

	$ gs -u your-name:password plugin/func.vim doc/func.txt
	https://gist.github.com/<new-id>

## AUTHOR

Ed van Bruggen <edvb54@gmail.com>

## SEE ALSO

See project page at: <http://edryd.org/projects/gs.html>

View source code at: <https://github.com/edvb/gs>

## LICENSE

zlib License
