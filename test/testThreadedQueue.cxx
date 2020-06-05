#include "testThreadedQueue.hxx"

class testThreadedQueue final : public testsuite
{
private:

public:
	testThreadedQueue() = default;
	testThreadedQueue(const testThreadedQueue &) = delete;
	testThreadedQueue(testThreadedQueue &&) = delete;
	~testThreadedQueue() final = default;
	testThreadedQueue &operator =(const testThreadedQueue &) = delete;
	testThreadedQueue &operator =(testThreadedQueue &&) = delete;

	void registerTests() final
	{
	}
};

CRUNCHpp_TESTS(testThreadedQueue)
