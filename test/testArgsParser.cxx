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
	void testBadOutputFile() { parser::testBadOutputFile(*this); }
	void testBadThreads() { parser::testBadThreads(*this); }
	void testBadPinning() { parser::testBadPinning(*this); }
	void testBadAlgorithm() { parser::testBadAlgorithm(*this); }

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
		CRUNCHpp_TEST(testBadOutputFile)
		CRUNCHpp_TEST(testBadThreads)
		CRUNCHpp_TEST(testBadPinning)
		CRUNCHpp_TEST(testBadAlgorithm)
	}
};

CRUNCHpp_TESTS(testParser)
