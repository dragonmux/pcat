#include "testArgsParser.hxx"

class testParser final : public testsuite
{
private:
	void testEmpty() { parser::testEmpty(*this); }
	void testSimple() { parser::testSimple(*this); }
	void testAssigned() { parser::testAssigned(*this); }
	void testMultiple() { parser::testMultiple(*this); }
	void testUnknown() { parser::testUnknown(*this); }
	void testInvalid() { parser::testInvalid(*this); }
	void testShortOutputFile() { parser::testShortOutputFile(*this); }

public:
	testParser() = default;
	testParser(const testParser &) = delete;
	testParser(testParser &&) = delete;
	~testParser() final = default;
	testParser &operator =(const testParser &) = delete;
	testParser &operator =(testParser &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testEmpty)
		CRUNCHpp_TEST(testSimple)
		CRUNCHpp_TEST(testAssigned)
		CRUNCHpp_TEST(testMultiple)
		CRUNCHpp_TEST(testUnknown)
		CRUNCHpp_TEST(testInvalid)
		CRUNCHpp_TEST(testShortOutputFile)
	}
};

CRUNCHpp_TESTS(testParser)
