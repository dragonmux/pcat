#ifndef TEST_ARGS_PARSER__HXX
#define TEST_ARGS_PARSER__HXX

#include <crunch++.h>

namespace parser
{
	extern void testEmpty(testsuite &suite);
	extern void testSimple(testsuite &suite);
	extern void testAssigned(testsuite &suite);
	extern void testMultiple(testsuite &suite);
	extern void testUnknown(testsuite &suite);
	extern void testInvalid(testsuite &suite);
}

#endif /*TEST_ARGS_PARSER__HXX*/
