#include <crunch++.h>
#include "testConsole.hxx"

class testConsole final : public testsuit
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
		CXX_TEST(testConstruct)
#ifndef _WINDOWS
		CXX_TEST(testPTYWrite)
#endif
		CXX_TEST(testPipeWrite)
		CXX_TEST(testConversions)
	}
};

CRUNCH_API void registerCXXTests() noexcept;
void registerCXXTests() noexcept
{
	registerTestClasses<testConsole>();
}
