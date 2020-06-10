#include <system_error>
#include "testMMap.hxx"

class testMMap final : public testsuite
{
private:
	fd_t testData{prepare()};
	random_t firstRandom{};
	random_t secondRandom{};

	fd_t prepare()
	{
		fd_t file{"mmap.test", O_CREAT | O_WRONLY, substrate::normalMode};
		if (!file.valid())
			return {};
		std::random_device dev{};
		firstRandom = dev();
		secondRandom = dev();
		if (!file.write("mmap"sv) ||
			// This uses the machine native order so we can more easily test the value later
			!file.write(firstRandom) ||
			file.seek(pcat::pageSize, SEEK_SET) != pcat::pageSize ||
			!file.write(secondRandom))
			throw std::system_error{std::error_code{errno, std::system_category()}};
		file = {};
		return {"mmap.test", O_RDWR};
	}

	void testDefaultConstruct() { memoryMap::testDefaultConstruct(*this); }
	void testMapEntireFile() { memoryMap::testMapEntireFile(*this, testData, firstRandom); }

public:
	testMMap() = default;
	testMMap(const testMMap &) = delete;
	testMMap(testMMap &&) = delete;
	testMMap &operator =(const testMMap &) = delete;
	testMMap &operator =(testMMap &&) = delete;

	~testMMap() final
	{
		if (testData.valid())
		{
			testData = {};
			unlink("mmap.test");
		}
	}

	void registerTests() final
	{
		if (!testData.valid())
			skip("Could not create test data for mmap_t tests");
		CRUNCHpp_TEST(testDefaultConstruct)
		CRUNCHpp_TEST(testMapEntireFile)
	}
};

CRUNCHpp_TESTS(testMMap)
