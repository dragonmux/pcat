#include <string_view>
#include <array>
#include <random>
#include <utility>
#include <substrate/utility>
#include <crunch++.h>
#include <chunking.hxx>

using namespace std::literals::string_view_literals;
constexpr static std::size_t operator ""_uz(const unsigned long long value) noexcept { return value; }

using random_t = typename std::random_device::result_type;
using substrate::fd_t;
using substrate::normalMode;
using pcat::transferBlockSize;
using pcat::inputFiles;
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
	fd_t outputFile{"chunks.test", O_RDWR | O_CREAT | O_NOCTTY, normalMode};
	std::vector<fd_t> files{};

	void testCopyNone()
	{
		inputFiles.clear();
		pcat::outputFile = outputFile.dup();
		assertEqual(chunkedCopy(), 0);
	}

	void testCopySingle()
	{
		inputFiles.clear();
		inputFiles.emplace_back(files[4].dup());
		if (!outputFile.resize(transferBlockSize))
			fail("Failed to resize the output test file");
		pcat::outputFile = outputFile.dup();
		assertEqual(chunkedCopy(), 0);
	}

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
	testChunking()
	{
		std::random_device seed{};
		if (!outputFile.valid())
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
