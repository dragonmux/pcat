#include "testChunkState.hxx"

class testChunkState final : public testsuite
{
private:
	void testDefaultConstruct() { chunkState::testDefaultConstruct(*this); }

public:
	testChunkState() = default;
	testChunkState(const testChunkState &) = delete;
	testChunkState(testChunkState &&) = delete;
	testChunkState &operator =(const testChunkState &) = delete;
	testChunkState &operator =(testChunkState &&) = delete;
	~testChunkState() final = default;

	void registerTests() final
	{
		CRUNCHpp_TEST(testDefaultConstruct)
	}
};

CRUNCHpp_TESTS(testChunkState)
