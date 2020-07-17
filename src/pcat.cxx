#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <string_view>
#include <array>
#include <vector>
#include <numeric>
#include <substrate/fd>
#include <substrate/utility>
#include <substrate/console>
#include <sys/file.h>
#include <version.hxx>
#include "args.hxx"
#include "help.hxx"
#include "algorithm/blockLinear/chunking.hxx"

using namespace std::literals::string_view_literals;
using substrate::console;

namespace pcat
{
	using substrate::fd_t;

	constexpr static auto options{substrate::make_array<args::option_t>(
	{
		{"--version"sv, argType_t::version},
		{"--help"sv, argType_t::help},
		{"-h"sv, argType_t::help},
		{"--output"sv, argType_t::outputFile},
		{"-o"sv, argType_t::outputFile},
		{"--no-sync"sv, argType_t::async},
		{"--async"sv, argType_t::async},
		{"--threads"sv, argType_t::threads},
		{"-t"sv, argType_t::threads},
		{"--core-pins"sv, argType_t::pinning},
		{"-c"sv, argType_t::pinning},
		{"--algorithm"sv, argType_t::algorithm}
	})};

	std::vector<fd_t> inputFiles{};
	fd_t outputFile{};

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
		constexpr std::size_t zero{};
		return std::accumulate(inputFiles.begin(), inputFiles.end(), zero,
			[](const std::size_t count, const fd_t &file) noexcept -> std::size_t
				{ return count + static_cast<std::size_t>(file.length()); }
		);
	}

	bool openOutputFile(int32_t &error)
	{
		const auto fileName = dynamic_cast<args::argOutputFile_t *>(::args->find(argType_t::outputFile))->fileName();
		errno = 0;
		fd_t file{fileName.data(), O_CREAT | O_RDWR | O_NOCTTY, substrate::normalMode};
		error = errno;
		if (!file.valid())
			return false;
		if (!file.resize(totalSize()))
		{
			error = errno;
			return false;
		}
		outputFile = std::move(file);
		return true;
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
		// NOLINTNEXTLINE(readability-magic-numbers)
		console.error("Can only specify one of --help and --version, not both."sv);
		return 1;
	}
	if (args->find(argType_t::version))
		return pcat::versionInfo::printVersion();
	else if (args->find(argType_t::help))
		return pcat::printHelp();
	else if (!args->find(argType_t::outputFile))
	{
		// NOLINTNEXTLINE(readability-magic-numbers)
		console.error("Output file must be specified on the command line, exiting."sv);
		return 1;
	}
	else if (!pcat::gatherFiles())
	{
		// NOLINTNEXTLINE(readability-magic-numbers)
		console.error("One or more files specified to concatentate are invalid, exiting."sv);
		return 1;
	}
	else if (std::int32_t error{}; !pcat::openOutputFile(error))
	{
		// NOLINTNEXTLINE(readability-magic-numbers)
		console.error("Could not open or resize the output file requested, exiting. Reason: "sv,
			std::strerror(error));
		pcat::closeFiles();
		return 1;
	}
	else if (std::int32_t error{pcat::algorithm::blockLinear::chunkedCopy()}; error)
	{
		// NOLINTNEXTLINE(readability-magic-numbers)
		console.error("Copying data to output file failed, exiting. Reason: "sv,
			std::strerror(error));
		pcat::closeFiles();
		return 1;
	}
	pcat::closeFiles();
	return 0;
}
