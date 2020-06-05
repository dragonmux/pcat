#include "testAffinity.hxx"

class testAffinity final : public testsuite
{
private:
	void testConstruct() { affinity::testConstruct(*this); }
	void testProcessorCount() { affinity::testProcessorCount(*this); }
	void testIteration() { affinity::testIteration(*this); }
	void testPinning() { affinity::testPinning(*this); }

public:
	testAffinity() = default;
	testAffinity(const testAffinity &) = delete;
	testAffinity(testAffinity &&) = delete;
	~testAffinity() final = default;
	testAffinity &operator =(const testAffinity &) = delete;
	testAffinity &operator =(testAffinity &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testConstruct)
		CRUNCHpp_TEST(testProcessorCount)
		CRUNCHpp_TEST(testIteration)
		CRUNCHpp_TEST(testPinning)
	}
};

CRUNCHpp_TESTS(testAffinity)
