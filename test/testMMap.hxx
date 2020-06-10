#ifndef TEST_MMAP__HXX
#define TEST_MMAP__HXX

#include <substrate/fd>
#include <crunch++.h>

using substrate::fd_t;

namespace memoryMap
{
	extern void testDefaultConstruct(testsuite &suite);
	extern void testMapEntireFile(testsuite &suite, const fd_t &fd);
}

#endif /*TEST_MMAP__HXX*/
