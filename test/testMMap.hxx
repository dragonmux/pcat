#ifndef TEST_MMAP__HXX
#define TEST_MMAP__HXX

#include <random>
#include <string_view>
#include <substrate/fd>
#include <crunch++.h>
#include <chunking.hxx>

using substrate::fd_t;
using random_t = typename std::random_device::result_type;
using namespace std::literals::string_view_literals;

namespace memoryMap
{
	constexpr static auto testStringMmap{"mmap"sv};

	extern void testDefaultConstruct(testsuite &suite);
	extern void testMapEntireFile(testsuite &suite, const fd_t &fd, random_t random);
	extern void testMapPartials(testsuite &suite, const fd_t &fd, random_t random);
}

#endif /*TEST_MMAP__HXX*/
