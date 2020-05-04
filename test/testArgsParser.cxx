#include <args.hxx>
#include <crunch++.h>
#include "testArgsParser.hxx"

class testParser : public testsuite
{
private:
	void testEmpty() { parser::testEmpty(*this); }
	void testSimple() { parser::testSimple(*this); }
	void testAssigned() { parser::testAssigned(*this); }
	void testMultiple() { parser::testMultiple(*this); }
	void testUnknown() { parser::testUnknown(*this); }
	void testInvalid() { parser::testInvalid(*this); }

public:
	testParser() = default;
	testParser(const testParser &) = delete;
	testParser(testParser &&) = delete;
	~testParser() final = default;
	testParser &operator =(const testParser &) = delete;
	testParser &operator =(testParser &&) = delete;

	void registerTests() final
	{
		CXX_TEST(testEmpty)
		CXX_TEST(testSimple)
		CXX_TEST(testAssigned)
		CXX_TEST(testMultiple)
		CXX_TEST(testUnknown)
		CXX_TEST(testInvalid)
	}
};

CRUNCH_API void registerCXXTests() noexcept;
void registerCXXTests() noexcept
{
	registerTestClasses<testParser>();
}
