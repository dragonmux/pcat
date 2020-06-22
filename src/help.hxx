#ifndef HELP__HXX
#define HELP__HXX

#include <string_view>

using namespace std::literals::string_view_literals;

namespace pcat
{
	constexpr static auto helpString{R"(Parallel `cat`

Usage:
	pcat [options] -o [output file] FILES

Options:
	--version       Print the version information for pcat
	-h, --help      Prints this help message

	-o, --output    Specifies the file to write the concatinated output to
	                this file may NOT be stdout and must be on a mmap-able file system.

	-t, --threads   If specified, this gives a thread count cap for the program to use
	                so long as the number is less than the number of logical cores present.
	-c, --core-pins Specifies which cores to pin the copy threads to.
	                When specified, this option must have the same number of cores specified
	                as threads given with -t/--threads. The same effect can be acomplished
	                using numactl, but this is provided for convenience and flexibility.

	--async         Specifies to omit issuing msync() on each completed block, thereby
	                putting the program into asynchronous operation.

	                WARNING: This option comes with a terrible penalty in that while
	                runtimes will appear to improve due to the greatly shortened program
	                runtime, IO will not have completed at program exit and you will be
	                hit with a IO tail while your machine's kernel does its best to process
	                all the queued IO as fast as it can, freezing userspace in the process.

	                For a 2.5GiB transfer on the author's machine on a 6-spindle RAID, this
	                tail lasts for 30s vs a typical program runtime of around 20s without
	                this option.

	                Additionally, it must be noted that use of this option prevents pcat
	                properly reporting IO failures arising during this IO tail, and that
	                as a result the program exit status cannot be used as an indication
	                of a successfully completed concatenation.
	--no-sync       Synonym for --async

This utility is licensed under the GPLv3+
Report bugs using https://github.com/DX-MON/pcat/issues)"sv
	};
}

#endif /*HELP__HXX*/
