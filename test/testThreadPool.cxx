#include "testThreadPool.hxx"

class testThreadPool final : public testsuite
{
private:
	void testUnused() { threadPool::testUnused(*this); }

public:
	testThreadPool() = default;
	testThreadPool(const testThreadPool &) = delete;
	testThreadPool(testThreadPool &&) = delete;
	~testThreadPool() final = default;
	testThreadPool &operator =(const testThreadPool &) = delete;
	testThreadPool &operator =(testThreadPool &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testUnused)
	}
};

CRUNCHpp_TESTS(testThreadPool)
