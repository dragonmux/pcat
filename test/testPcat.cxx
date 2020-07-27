#include "testPcat.hxx"

class testPcat final : public testsuite
{
private:
	void testVersion() { pcat::testVersion(*this); }

public:
	testPcat() = default;
	testPcat(const testPcat &) = delete;
	testPcat(testPcat &&) = delete;
	testPcat &operator =(const testPcat &) = delete;
	testPcat &operator =(testPcat &&) = delete;
	~testPcat() final = default;

	void registerTests() final
	{
		CRUNCHpp_TEST(testVersion)
	}
};

CRUNCHpp_TESTS(testPcat)
