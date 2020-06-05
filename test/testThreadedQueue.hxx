#ifndef TEST_THREADED_QUEUE__HXX
#define TEST_THREADED_QUEUE__HXX

#include <crunch++.h>

namespace threadedQueue
{
	extern void testEmpty(testsuite &suite);
	extern void testEmplace(testsuite &suite);
	extern void testPush(testsuite &suite);
	extern void testPop(testsuite &suite);
}

#endif /*TEST_THREADED_QUEUE__HXX*/
