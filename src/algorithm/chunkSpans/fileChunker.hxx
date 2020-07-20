#ifndef ALGORITHM_CHUNK_SPANS_FILE_CHUNKER__HXX
#define ALGORITHM_CHUNK_SPANS_FILE_CHUNKER__HXX

#include "chunkState.hxx"

namespace pcat::algorithm::chunkSpans
{
	struct chunking_t final
	{
	private:
		std::size_t spanLength;
		inputFilesIterator_t file{inputFiles.begin()};
		off_t inputLength{file == inputFiles.end() ? 0 : file->length()};
		mappingOffset_t inputOffset{0, blockLength(inputLength)};
		const off_t outputLength{outputFile.length()};
		mappingOffset_t outputOffset{};

		constexpr off_t spanOf(const off_t length)
			{ return std::min(off_t(spanLength), length); }

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
			inputOffset.length(blockLength(inputLength - inputOffset));
		};

	public:
		chunking_t(const std::size_t spanLength_) noexcept : spanLength{spanLength_}
			{ outputOffset.length(spanOf(outputLength - outputOffset)); }
		chunking_t(const std::size_t spanLength_, const inputFilesIterator_t file_) noexcept :
			spanLength{spanLength_}, file{file_}, outputOffset{outputLength} { }
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
			if (file != inputFiles.end())
				nextInputBlock();
			outputOffset += outputOffset.length();
			if (outputOffset == outputLength)
				outputOffset.length(0);
			else if (outputLength - outputOffset - spanLength < spanLength)
				outputOffset.length(outputLength - outputOffset);
			else
				outputOffset.length(spanLength);
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
	private:
		std::size_t spanLength_;

	public:
		constexpr fileChunker_t(const std::size_t spanLength) : spanLength_{spanLength} { }

		// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
		[[nodiscard]] chunking_t begin() const noexcept { return {spanLength_}; }
		// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
		[[nodiscard]] chunking_t end() const noexcept { return {spanLength_, inputFiles.end()}; }
	};
} // namespace pcat::algorithm::chunkSpans

#endif /*ALGORITHM_CHUNK_SPANS_FILE_CHUNKER__HXX*/
