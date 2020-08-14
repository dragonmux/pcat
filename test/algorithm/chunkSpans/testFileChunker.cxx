#include <string_view>
#include <array>
#include <utility>
#include <stdexcept>
#include <substrate/utility>
#include <chunking.hxx>
#include "testFileChunker.hxx"

using namespace std::literals::string_view_literals;
constexpr static std::size_t operator ""_uz(const unsigned long long value) noexcept { return value; }

using substrate::fd_t;
using substrate::normalMode;
using pcat::transferBlockSize;
using pcat::inputFiles;

constexpr static auto chunkFiles{substrate::make_array<std::pair<std::string_view, std::size_t>>(
{
	{"chunk1.test"sv, 1024_uz},
	{"chunk2.test"sv, 2048_uz},
	{"chunk3.test"sv, 3072_uz},
	{"chunk4.test"sv, std::size_t(transferBlockSize) - 4096_uz},
	{"chunk5.test"sv, std::size_t(transferBlockSize)},
	{"chunk6.test"sv, std::size_t(transferBlockSize * 32U) - 2048_uz}
})};

#ifdef _WINDOWS
#define O_NOCTTY O_BINARY
#endif

class testFileChunker final : public testsuite
{
private:
	fd_t outputFile{"chunks.test", O_RDWR | O_CREAT | O_NOCTTY, normalMode};
	std::vector<fd_t> files{};

	void testDefaultConstruct()
	{
		inputFiles.clear();
		pcat::outputFile = outputFile.dup();
		fileChunker::testDefaultConstruct(*this);
	}

	void testFillAlignedChunk()
	{
		inputFiles.clear();
		inputFiles.emplace_back(files[4].dup());
		if (!outputFile.resize(transferBlockSize))
			fail("Failed to resize the output test file");
		pcat::outputFile = outputFile.dup();
		fileChunker::testFillAlignedChunk(*this);
	}

	void testFillUnalignedChunks()
	{
		inputFiles.clear();
		inputFiles.emplace_back(files[0].dup());
		inputFiles.emplace_back(files[2].dup());
		inputFiles.emplace_back(files[4].dup());
		// NOLINTNEXTLINE(readability-magic-numbers)
		if (!outputFile.resize(transferBlockSize + 4096))
			fail("Failed to resize the output test file");
		pcat::outputFile = outputFile.dup();
		fileChunker::testFillUnalignedChunks(*this);
	}

	void testFillLargeSpanChunk()
	{
		inputFiles.clear();
		inputFiles.emplace_back(files[0].dup());
		inputFiles.emplace_back(files[1].dup());
		inputFiles.emplace_back(files[2].dup());
		inputFiles.emplace_back(files[4].dup());
		inputFiles.emplace_back(files[3].dup());
		inputFiles.emplace_back(files[5].dup());
		// NOLINTNEXTLINE(readability-magic-numbers)
		if (!outputFile.resize(transferBlockSize * 34U))
			fail("Failed to resize the output test file");
		pcat::outputFile = outputFile.dup();
		fileChunker::testFillLargeSpanChunk(*this);
	}

	void makeFile(const std::string_view fileName, const std::size_t size)
	{
		const auto &file = files.emplace_back(fileName.data(), O_RDWR | O_CREAT | O_NOCTTY, normalMode);
		if (!file.valid() || !file.resize(size))
		{
			files.pop_back();
			throw std::logic_error{"Failed to create and resize an input test file"};
		}
	}

public:
	testFileChunker()
	{
		if (!outputFile.valid())
			throw std::logic_error{"Failed to create the output test file"};
		outputFile.resize(0);
		for (const auto &file : chunkFiles)
			makeFile(file.first, file.second);
	}

	testFileChunker(const testFileChunker &) = delete;
	testFileChunker(testFileChunker &&) = delete;
	testFileChunker &operator =(const testFileChunker &) = delete;
	testFileChunker &operator =(testFileChunker &&) = delete;

	~testFileChunker() final
	{
		pcat::inputFiles.clear();
		files.clear();
		for (const auto &file : chunkFiles)
			unlink(file.first.data());
		unlink("chunks.test");
	}

	void registerTests() final
	{
		CRUNCHpp_TEST(testDefaultConstruct)
		CRUNCHpp_TEST(testFillAlignedChunk)
		CRUNCHpp_TEST(testFillUnalignedChunks)
		CRUNCHpp_TEST(testFillLargeSpanChunk)
	}
};

CRUNCHpp_TESTS(testFileChunker)
