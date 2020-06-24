#include <substrate/utility>
#include <args.hxx>
#include "testThreadPool.hxx"

class testThreadPool final : public testsuite
{
private:
	void testUnused() { threadPool::testUnused(*this); }
	void testOnce() { threadPool::testOnce(*this); }
	void testQueueWait() { threadPool::testQueueWait(*this); }

public:
	testThreadPool() { args = substrate::make_unique<pcat::args::argsTree_t>(); }

	testThreadPool(const testThreadPool &) = delete;
	testThreadPool(testThreadPool &&) = delete;
	~testThreadPool() final = default;
	testThreadPool &operator =(const testThreadPool &) = delete;
	testThreadPool &operator =(testThreadPool &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testUnused)
		CRUNCHpp_TEST(testOnce)
		CRUNCHpp_TEST(testQueueWait)
	}
};

CRUNCHpp_TESTS(testThreadPool)
