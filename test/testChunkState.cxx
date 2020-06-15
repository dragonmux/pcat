#include <string_view>
#include <array>
#include <utility>
#include <stdexcept>
#include <substrate/utility>
#include <chunking.hxx>
#include "testChunkState.hxx"

using namespace std::literals::string_view_literals;
constexpr static std::size_t operator ""_uz(const unsigned long long value) noexcept { return value; }

using substrate::fd_t;
using pcat::transferBlockSize;
using pcat::inputFiles;

constexpr static auto chunkFiles{substrate::make_array<std::pair<std::string_view, std::size_t>>(
{
	{"chunk1.test"sv, 1024_uz},
	{"chunk2.test"sv, 2048_uz},
	{"chunk3.test"sv, 3072_uz},
	{"chunk4.test"sv, std::size_t(transferBlockSize) - 4096_uz},
	{"chunk5.test"sv, std::size_t(transferBlockSize)}
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

	void testFillAlignedChunk()
	{
		inputFiles.clear();
		inputFiles.emplace_back(files[4].dup());
		chunkState::testFillAlignedChunk(*this);
	}

	void makeFile(const std::string_view fileName, const std::size_t size)
	{
		const auto &file = files.emplace_back(fileName.data(), O_RDWR | O_CREAT | O_NOCTTY);
		if (!file.valid() || !file.resize(size))
		{
			files.pop_back();
			throw std::logic_error{"Failed to create and resize an input test file"};
		}
	}

public:
	testChunkState()
	{
		for (const auto &file : chunkFiles)
			makeFile(file.first, file.second);
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
