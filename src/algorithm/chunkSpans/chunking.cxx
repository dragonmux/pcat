#include <cassert>
#include <string_view>
#include <substrate/console>
#include "chunking.hxx"
#include "mmap.hxx"
#include "threadPool.hxx"
#include "mappingOffset.hxx"

using namespace std::literals::string_view_literals;
using substrate::console;

namespace pcat::algorithm::chunkSpans
{
	struct chunkState_t final
	{
	private:
		inputFilesIterator_t file_{};
		off_t inputLength_{};
		mappingOffset_t inputOffset_{};
		mappingOffset_t outputOffset_{};

	public:
		chunkState_t() noexcept = default;
		constexpr chunkState_t(const inputFilesIterator_t &file, const off_t inputLength,
			const mappingOffset_t &inputOffset, const mappingOffset_t &outputOffset) noexcept :
			file_{file}, inputLength_{inputLength}, inputOffset_{inputOffset}, outputOffset_{outputOffset} { }
		constexpr chunkState_t(const chunkState_t &) noexcept = default;
		constexpr chunkState_t(chunkState_t &&) noexcept = default;
		chunkState_t &operator =(const chunkState_t &) noexcept = default;
		chunkState_t &operator =(chunkState_t &&) noexcept = default;
		~chunkState_t() noexcept = default;
		[[nodiscard]] constexpr const inputFilesIterator_t &file() const noexcept { return file_; }
		[[nodiscard]] constexpr off_t inputLength() const noexcept { return inputLength_; }
		[[nodiscard]] const fd_t &inputFile() const noexcept { return *file_; }
		[[nodiscard]] constexpr const mappingOffset_t &inputOffset() const noexcept { return inputOffset_; }
		[[nodiscard]] constexpr const mappingOffset_t &outputOffset() const noexcept { return outputOffset_; }

		[[nodiscard]] constexpr chunkState_t end() const noexcept
		{
			chunkState_t state{*this};
			/*while (!state.atEnd())
				++state;*/
			return state;
		}
	};

	struct chunking_t final
	{
	private:
		inputFilesIterator_t file{inputFiles.begin()};
		off_t inputLength{file == inputFiles.end() ? 0 : file->length()};
		mappingOffset_t inputOffset{0, blockLength(inputLength)};
		const off_t outputLength{outputFile.length()};
		mappingOffset_t outputOffset{};

		constexpr void nextInputBlock() noexcept
		{
			inputOffset += inputOffset.length();
			if (inputOffset.offset() == inputLength)
			{
				assert(file < inputFiles.end()); // NOLINT
				++file;
				inputLength = file == inputFiles.end() ? 0 : file->length();
				inputOffset = {};
			}
		};

	public:
		chunking_t() noexcept { outputOffset.length(blockLength(outputLength - outputOffset)); }
		chunking_t(const inputFilesIterator_t file_) noexcept : file{file_}, outputOffset{outputLength} { }
		[[nodiscard]] constexpr chunkState_t subchunkState() const noexcept
			{ return {file, inputLength, inputOffset, outputOffset}; }
		chunkState_t operator *() const noexcept { return subchunkState(); }

		constexpr void operator ++() noexcept
		{
			if (outputOffset == outputLength)
				return;
			else if (outputOffset.length() != inputOffset.length())
			{
				const auto state{subchunkState().end()};
				file = state.file();
				inputLength = state.inputLength();
				inputOffset = state.inputOffset();
			}
			nextInputBlock();
			inputOffset.length(blockLength(inputLength - inputOffset));
			outputOffset += outputOffset.length();
			outputOffset.length(blockLength(outputLength - outputOffset));
		}

		bool operator ==(const chunking_t &other) const noexcept
		{
			return file == other.file &&
				inputOffset == other.inputOffset &&
				outputOffset == other.outputOffset;
		}
		bool operator !=(const chunking_t &other) const noexcept { return !(*this == other); }
	};

	// begin() and end() are non-static to fufill the requirements of for-each looping
	struct fileChunker_t final
	{
		// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
		chunking_t begin() noexcept { return {}; }
		// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
		chunking_t end() noexcept { return {inputFiles.end()}; }
	};

	int32_t copyChunk(chunkState_t chunk)
	{
	}

	/*!
	 * The idea with this is that, we take the total length of the file,
	 * the number of threads and divide them together if there are more than
	 * (transferBlockSize * threads) bytes to move, otherwise splitting
	 * the blocks up into transferBlockSize chunks.
	 *
	 * The result should be up to threads chunks which are then scheduled
	 * off one onto each thread, and copied linearly within that thread.
	 * The goal is to create a mostly linear access order for sub-chunks
	 * of the input files to reduce IO Wait issues on Lustre + GPFS, and
	 * significantly boost the speed of copying in that way.
	 */

	int32_t chunkedCopy() noexcept try
	{
		//threadPool_t copyThreads{copyChunk};
		fileChunker_t chunker{};
		//assert(copyThreads.ready());

		for (const chunkState_t &chunk : chunker)
		{
			/*const int32_t result{copyThreads.queue(chunk)};
			if (result)
			{
				console.error("Copying failed: "sv, std::strerror(result));
				return result;
			}*/
			if (auto result{copyChunk(chunk)}; !result)
				return result;
		}
		//return copyThreads.finish();
		return 0;
	}
	catch (std::system_error &error)
	{
		console.error("Copying failed: "sv, error.what());
		return error.code().value();
	}
} // namespace pcat::algorithm::chunkSpans
