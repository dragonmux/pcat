#include <cassert>
#include <string_view>
#include <array>
#include <random>
#include <utility>
#include <substrate/utility>
#include <crunch++.h>
#include <chunking.hxx>

using namespace std::literals::string_view_literals;
constexpr static std::size_t operator ""_uz(const unsigned long long value) noexcept { return value; }

std::vector<substrate::fd_t> pcat::inputFiles{};
substrate::fd_t pcat::outputFile{};

using random_t = typename std::random_device::result_type;
using substrate::fd_t;
using substrate::normalMode;
using pcat::transferBlockSize;
using pcat::inputFiles;
using pcat::outputFile;
using pcat::chunkedCopy;

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

class testChunking final : public testsuite
{
private:
	fd_t resultFile{"chunks.test", O_RDWR | O_CREAT | O_NOCTTY, normalMode};
	std::vector<fd_t> files{};

	void testCopyNone()
	{
		inputFiles.clear();
		outputFile = resultFile.dup();
		assertEqual(outputFile.length(), 0);
		assertTrue(inputFiles.begin() == inputFiles.end());
		assertEqual(chunkedCopy(), 0);
	}

	void testCopySingle()
	{
		inputFiles.clear();
		inputFiles.emplace_back(files[4].dup());
		if (!resultFile.resize(transferBlockSize))
			fail("Failed to resize the output test file");
		outputFile = resultFile.dup();
		assertEqual(outputFile.length(), transferBlockSize);
		assertFalse(inputFiles.begin() == inputFiles.end());
		assertEqual(inputFiles.size(), 1);
		assertEqual(inputFiles[0].length(), transferBlockSize);
		assertEqual(chunkedCopy(), 0);
	}

	void makeFile(const std::string_view fileName, const std::size_t size, const random_t seed) noexcept
	{
		const auto &file = files.emplace_back(fileName.data(), O_RDWR | O_CREAT | O_NOCTTY, normalMode);
		std::minstd_rand engine{seed};
		std::uniform_int_distribution<uint8_t> genRandom{};
		for (size_t i{}; i < size; ++i)
			file.write(genRandom(engine));
		assert(file.head()); // NOLINT
	}

public:
	testChunking()
	{
		std::random_device seed{};
		if (!resultFile.valid())
			throw std::logic_error{"Failed to create the output test file"};
		for (const auto &file : chunkFiles)
			makeFile(file.first, file.second, seed());
	}

	testChunking(const testChunking &) = delete;
	testChunking(testChunking &&) = delete;
	testChunking &operator =(const testChunking &) = delete;
	testChunking &operator =(testChunking &&) = delete;

	~testChunking() final
	{
		pcat::inputFiles.clear();
		files.clear();
		for (const auto &file : chunkFiles)
			unlink(file.first.data());
		unlink("chunks.test");
	}

	void registerTests() final
	{
		CRUNCHpp_TEST(testCopyNone)
		CRUNCHpp_TEST(testCopySingle)
	}
};

CRUNCHpp_TESTS(testChunking)
