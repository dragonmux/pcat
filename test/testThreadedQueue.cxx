#include "testThreadedQueue.hxx"

class testThreadedQueue final : public testsuite
{
private:
	void testEmpty() { threadedQueue::testEmpty(*this); }
	void testEmplace() { threadedQueue::testEmplace(*this); }
	void testPush() { threadedQueue::testPush(*this); }
	void testPop() { threadedQueue::testPop(*this); }
	void testAsync() { threadedQueue::testAsync(*this); }

public:
	testThreadedQueue() = default;
	testThreadedQueue(const testThreadedQueue &) = delete;
	testThreadedQueue(testThreadedQueue &&) = delete;
	~testThreadedQueue() final = default;
	testThreadedQueue &operator =(const testThreadedQueue &) = delete;
	testThreadedQueue &operator =(testThreadedQueue &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testEmpty)
		CRUNCHpp_TEST(testEmplace)
		CRUNCHpp_TEST(testPush)
		CRUNCHpp_TEST(testPop)
		CRUNCHpp_TEST(testAsync)
	}
};

CRUNCHpp_TESTS(testThreadedQueue)
