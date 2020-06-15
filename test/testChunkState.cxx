#include <string_view>
#include <array>
#include <random>
#include <substrate/utility>
#include <chunking.hxx>
#include "testChunkState.hxx"

using namespace std::literals::string_view_literals;
constexpr static std::size_t operator ""_uz(const unsigned long long value) noexcept { return value; }

using random_t = typename std::random_device::result_type;
using substrate::fd_t;

constexpr static auto chunkFiles{substrate::make_array<std::pair<std::string_view, std::size_t>>(
{
	{"chunk1.test"sv, 1024_uz},
	{"chunk2.test"sv, 2048_uz},
	{"chunk3.test"sv, 3072_uz},
	{"chunk4.test"sv, std::size_t(pcat::transferBlockSize) - 4096_uz},
	{"chunk5.test"sv, std::size_t(pcat::transferBlockSize)}
})};

#ifdef _WINDOWS
#define O_NOCTTY O_BINARY
#endif

class testChunkState final : public testsuite
{
private:
	std::vector<fd_t> files{};

	void testDefaultConstruct() { chunkState::testDefaultConstruct(*this); }
	void testNoFilesConstruct() { chunkState::testNoFilesConstruct(*this); }

	void makeFile(const std::string_view fileName, const std::size_t size, const random_t seed) noexcept
	{
		fd_t file{fileName.data(), O_WRONLY | O_NOCTTY};
		std::minstd_rand engine{seed};
		std::uniform_int_distribution<uint32_t> genRandom{};
		for (size_t i{}; i < size; ++i)
			file.write(genRandom(engine));
		files.emplace_back(fileName.data(), O_RDONLY | O_NOCTTY);
	}

public:
	testChunkState()
	{
		std::random_device randDev{};
		for (const auto &file : chunkFiles)
			makeFile(file.first, file.second, randDev());
	}

	testChunkState(const testChunkState &) = delete;
	testChunkState(testChunkState &&) = delete;
	testChunkState &operator =(const testChunkState &) = delete;
	testChunkState &operator =(testChunkState &&) = delete;

	~testChunkState() final
	{
		pcat::inputFiles.clear();
		files.clear();
		for (const auto &file : chunkFiles)
			unlink(file.first.data());
	}

	void registerTests() final
	{
		CRUNCHpp_TEST(testDefaultConstruct)
		CRUNCHpp_TEST(testNoFilesConstruct)
	}
};

CRUNCHpp_TESTS(testChunkState)
