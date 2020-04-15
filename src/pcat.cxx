#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <array>
#include <vector>
#include <numeric>
#include <substrate/fd>
#include <substrate/mmap>
#include <substrate/utility>
#include <substrate/units>
#include <substrate/console>
#include <sys/file.h>
#include <version.hxx>
#include "args.hxx"
#include "help.hxx"

using std::literals::string_view_literals::operator ""sv;
using substrate::console;

namespace pcat
{
	using substrate::fd_t;
	using substrate::mmap_t;
	using substrate::operator ""_KiB;

	constexpr static auto options{substrate::make_array<args::option_t>(
	{
		{"--version"sv, argType_t::version},
		{"--help"sv, argType_t::help},
		{"-h"sv, argType_t::help},
		{"--output"sv, argType_t::outputFile},
		{"-o"sv, argType_t::outputFile}
	})};

	constexpr static size_t pageSize = 4_KiB;
	std::vector<fd_t> inputFiles{};

	inline int32_t printHelp() noexcept
	{
		console.info(helpString);
		return 0;
	}

	bool lockFile(const fd_t &file) noexcept
	{
		struct flock lock{};
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		// This combo means "whole file, for eternity, even if it grows".
		lock.l_start = 0;
		lock.l_len = 0;
		return fcntl(file, F_SETLK, &lock) == 0 && // NOLINT(cppcoreguidelines-pro-type-vararg)
			flock(file, LOCK_NB | LOCK_MAND | LOCK_READ) == 0;
	}

	bool unlockFile(const fd_t &file) noexcept
	{
		struct flock lock{};
		lock.l_type = F_UNLCK;
		lock.l_whence = SEEK_SET;
		// This combo means "whole file, for eternity, even if it grows".
		lock.l_start = 0;
		lock.l_len = 0;
		return fcntl(file, F_SETLK, &lock) == 0 && // NOLINT(cppcoreguidelines-pro-type-vararg)
			flock(file, LOCK_UN) == 0;
	}

	bool checkFile(const std::string_view fileName) noexcept
	{
		fd_t file{fileName.data(), O_RDWR | O_NOCTTY};
		// If the file wasn't able to be opened, or is not stat()-able, discard it.
		if (!file.valid() || file.length() <= 0)
			return false;
		// Change it from just open, to exclusive access so it can't be removed or changed from under us.
		if (!lockFile(file))
			return false;
		inputFiles.emplace_back(std::move(file));
		return true;
	}

	bool gatherFiles() noexcept
	{
		for (const auto &arg : *::args)
		{
			if (arg->type() != argType_t::unrecognised)
				continue;
			const auto &file{dynamic_cast<args::argUnrecognised_t &>(*arg)};
			if (!checkFile(file.argument()))
				return false;
		}
		return true;
	}

	std::size_t totalSize() noexcept
	{
		return std::accumulate(inputFiles.begin(), inputFiles.end(), 0,
			[](const std::size_t count, const fd_t &file) noexcept -> std::size_t
				{ return count + file.length(); }
		);
	}

	void closeFiles() noexcept
	{
		for (const auto &file : inputFiles)
			unlockFile(file);
		inputFiles.clear();
	}
} // namespace pcat

int main(int argCount, char **argList)
{
	console = {stdout, stderr};
	if (!parseArguments(argCount, argList, pcat::options))
	{
		console.error("Failed to parse arguments"sv); // NOLINT(readability-magic-numbers)
		return 1;
	}
	if (args->find(argType_t::version) && args->find(argType_t::help))
	{
		console.error("Can only specify one of --help and --version, not both."sv); // NOLINT(readability-magic-numbers)
		return 1;
	}
	if (args->find(argType_t::version))
		return pcat::versionInfo::printVersion();
	else if (args->find(argType_t::help))
		return pcat::printHelp();
	dumpAST();
	if (!pcat::gatherFiles())
	{
		console.error("One or more files specified to concatentate are invalid, exiting."sv); // NOLINT(readability-magic-numbers)
		return 1;
	}
	console.info("Length of the resulting concatenated file will be "sv, pcat::totalSize(), " bytes"sv);

	pcat::closeFiles();
	return 0;
}
