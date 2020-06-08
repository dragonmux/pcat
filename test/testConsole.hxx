#ifndef TEST_CONSOLE__HXX
#define TEST_CONSOLE__HXX

#include <crunch++.h>

namespace consoleTests
{
	void testConstruct(testsuite &suite);
#ifndef _WINDOWS
	void testPTYWrite(testsuite &suite);
#endif
	void testPipeWrite(testsuite &suite);
	void testConversions(testsuite &suite);
}

#endif /*TEST_CONSOLE__HXX*/
