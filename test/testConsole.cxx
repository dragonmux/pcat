#include "testConsole.hxx"

class testConsole final : public testsuite
{
private:
	void testConstruct() { consoleTests::testConstruct(*this); }
#ifndef _WINDOWS
	void testPTYWrite() { consoleTests::testPTYWrite(*this); }
#endif
	void testPipeWrite() { consoleTests::testPipeWrite(*this); }
	void testConversions() { consoleTests::testConversions(*this); }

public:
	testConsole() = default;
	testConsole(const testConsole &) = delete;
	testConsole(testConsole &&) = delete;
	~testConsole() final = default;
	testConsole &operator =(const testConsole &) = delete;
	testConsole &operator =(testConsole &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testConstruct)
#ifndef _WINDOWS
		CRUNCHpp_TEST(testPTYWrite)
#endif
		CRUNCHpp_TEST(testPipeWrite)
		CRUNCHpp_TEST(testConversions)
	}
};

CRUNCHpp_TESTS(testConsole)
