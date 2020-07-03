% PCAT(1) pcat 0.1.0 | Parallel Concatenate

# NAME

**pcat** - A fast, parallel, file concatenation utility

# SYNOPSIS

| **pcat** \[**-h**|**\--help**]
| **pcat** \[**-v**|**\--version**]
| **pcat** \[_options_] **-o**|**\--output** outputFile _FILES_

# DESCRIPTION

pcat is a utility to concatenate files together like `cat`, but in parallel with
CPU core pinning to allow for greatly reduced runtimes and higher throuput on a
large parallel filesystem such as one might find in a supercomputer.

# OPTIONS

## General

-v, \--version

:   Prints the version information for pcat

-h, \--help

:   Prints the utility help message

Any option that takes additional arguments such as **-o**/**\--output** can have those additional
arguments specified with either a space or equals sign seperating them. \
That is, the following are equivilent: **\--output file** vs **\--output=file**

# BUGS

Report bugs using [https://github.com/DX-MON/pcat/issues](https://github.com/DX-MON/pcat/issues)

# AUTHORS

Rachel Mant <dx-mon@users.sourceforge.net>
