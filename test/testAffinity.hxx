#ifndef TEST_AFFINITY__HXX
#define TEST_AFFINITY__HXX

#include <crunch++.h>

namespace affinity
{
	extern void testConstruct(testsuite &suite);
	extern void testProcessorCount(testsuite &suite);
	extern void testIteration(testsuite &suite);
	extern void testPinning(testsuite &suite);
	extern void testThreadCap(testsuite &suite);
	extern void testUserPinning(testsuite &suite);
}

#endif /*TEST_AFFINITY__HXX*/
