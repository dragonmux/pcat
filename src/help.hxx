#ifndef HELP__HXX
#define HELP__HXX

#include <string_view>

using std::literals::string_view_literals::operator ""sv;

namespace pcat
{
	constexpr static auto helpString{R"(Parallel `cat`

Usage:
	pcat [options] -o [output file] FILES

Options:
	--version     Print the version information for pcat
	-h, --help    Prints this help message

	-o, --output  Specifies the file to write the concatinated output to
	              this file may NOT be stdout and must be on a mmap-able file system.

This utility is licensed under the GPLv3+
Report bugs using https://github.com/DX-MON/pcat/issues)"sv
	};
}

#endif /*HELP__HXX*/
