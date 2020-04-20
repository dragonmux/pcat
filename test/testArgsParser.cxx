#include <args.hxx>
#include <crunch++.h>
#include "testArgsParser.hxx"

class testParser : public testsuit
{
private:
	void testEmpty() { parser::testEmpty(*this); }
	void testSimple() { parser::testSimple(*this); }
	void testAssigned() { parser::testAssigned(*this); }
	void testMultiple() { parser::testMultiple(*this); }

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
	}
};

CRUNCH_API void registerCXXTests() noexcept;
void registerCXXTests() noexcept
{
	registerTestClasses<testParser>();
}
