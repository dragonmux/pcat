#ifndef TEST_CONSOLE__HXX
#define TEST_CONSOLE__HXX

namespace consoleTests
{
	void testConstruct(testsuit &suite);
#ifndef _WINDOWS
	void testPTYWrite(testsuit &suite);
#endif
	void testPipeWrite(testsuit &suite);
	void testConversions(testsuit &suite);
}

#endif /*TEST_CONSOLE__HXX*/
