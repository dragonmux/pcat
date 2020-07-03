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

-o, \--output

:   Specifies the file to write the concatinated output to
    this file may NOT be stdout and must be on a mmap-able file system.

## Parallelism

-t, \--threads

:   If specified, this gives a thread count cap for the program to use
    so long as the number is less than the number of logical cores present.

-c, \--core-pins

:   Specifies which cores to pin the copy threads to.
    When specified, this option must have the same number of cores specified
    as threads given with -t/--threads. The same effect can be acomplished
    using numactl, but this is provided for convenience and flexibility.

\--async

:   Specifies to omit issuing msync() on each completed block, thereby
    putting the program into asynchronous operation.

    **WARNING** \
    This option comes with a terrible penalty in that: while
    runtimes will appear to improve due to the greatly shortened program
    runtime, IO will not have completed at program exit and you will be
    hit with a IO tail.
    This tail is caused by your machine's kernel as it does its best to process
    all the queued IO as fast as it can - freezing userspace in the process.
    For a 2.5GiB transfer on the author's machine on a 6-spindle RAID, this
    tail lasts for 30s vs a typical program runtime of around 20s without
    this option.

    **NOTE**\
    Additionally, use of this option prevents pcat
    properly reporting IO failures arising during this IO tail, and that
    as a result the program exit status cannot be used as an indication
    of a successfully completed concatenation.

\--no-sync

:   Synonym for \--async

Any option that takes additional arguments such as **-o**/**\--output** can have those additional
arguments specified with either a space or equals sign seperating them. \
That is, the following are equivilent: **\--output file** vs **\--output=file**

# BUGS

Report bugs using [https://github.com/DX-MON/pcat/issues](https://github.com/DX-MON/pcat/issues)

# AUTHORS

Rachel Mant <dx-mon@users.sourceforge.net>
