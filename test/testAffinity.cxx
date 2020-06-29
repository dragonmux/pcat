#include <substrate/utility>
#include <args.hxx>
#include "testAffinity.hxx"

class testAffinity final : public testsuite
{
private:
	void testConstruct() { affinity::testConstruct(*this); }
	void testProcessorCount() { affinity::testProcessorCount(*this); }
	void testIteration() { affinity::testIteration(*this); }
	void testPinning() { affinity::testPinning(*this); }
	void testThreadCap() { affinity::testThreadCap(*this); }
	void testUserPinning() { affinity::testUserPinning(*this); }
	void testPinSecondCore() { affinity::testPinSecondCore(*this); }

public:
	testAffinity() noexcept { args = substrate::make_unique_nothrow<pcat::args::argsTree_t>(); }
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
		CRUNCHpp_TEST(testThreadCap)
		CRUNCHpp_TEST(testUserPinning)
		CRUNCHpp_TEST(testPinSecondCore)
	}
};

CRUNCHpp_TESTS(testAffinity)
