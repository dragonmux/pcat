#include <cassert>
#include <cerrno>
#include <substrate/console>
#include "chunking.hxx"
#include "mmap.hxx"

using substrate::console;

namespace pcat
{
	struct mappingOffset_t final
	{
	private:
		size_t offset_;
		size_t adjust_;
		size_t length_;

		constexpr void recalcAdjustment()
		{
			adjust_ = offset_ % pageSize;
			offset_ -= adjust_;
		}

	public:
		constexpr mappingOffset_t(const size_t offset, const size_t length) : offset_{offset},
			adjust_{0}, length_{length} { recalcAdjustment(); }
		[[nodiscard]] constexpr size_t offset() const noexcept { return offset_ + adjust_; }
		[[nodiscard]] constexpr size_t adjustment() const noexcept { return adjust_; }
		[[nodiscard]] constexpr size_t length() const noexcept { return length_; }
		[[nodiscard]] constexpr size_t adjustedOffset() const noexcept { return offset_; }
		[[nodiscard]] constexpr size_t adjustedLength() const noexcept { return length_ + adjust_; }
		constexpr void length(const size_t length) noexcept { length_ = length; }
		constexpr operator size_t() const noexcept { return offset(); }

		constexpr mappingOffset_t &operator +=(const size_t amount) noexcept
		{
			offset_ += amount + adjust_;
			recalcAdjustment();
			return *this;
		}
	};

	constexpr size_t blockLength(const size_t length)
		{ return std::min(transferBlockSize, length); }

	void calculateInputChunking() noexcept
	{
		auto file = inputFiles.begin();
		size_t inputLength = file->length();
		mappingOffset_t inputOffset{0, blockLength(inputLength)};

		const size_t outputLength = outputFile.length();
		for (mappingOffset_t outputOffset{0, 0}; outputOffset < outputLength;
			outputOffset += transferBlockSize)
		{
			outputOffset.length(blockLength(outputLength - outputOffset));
			console.info("Copying ", inputOffset.length(), " bytes at ", inputOffset.offset(),
				" to ", outputOffset.length(), " byte region at ", outputOffset.offset());

			const auto nextInputBlock = [&]() -> void
			{
				inputOffset += inputOffset.length();
				if (inputOffset.offset() == inputLength)
				{
					++file;
					inputLength = file->length();
					inputOffset = {0, 0};
				}
			};

			if (outputOffset.length() != inputOffset.length())
			{
				const size_t originalOutputOffset = outputOffset.offset();
				while (outputOffset.length() - inputOffset.length())
				{
					const size_t remainder = outputOffset.length() - inputOffset.length();
					console.info("Transfer caused a remainder of ", remainder, " bytes to go for output block");
					outputOffset += inputOffset.length();
					outputOffset.length(remainder);
					nextInputBlock();
					inputOffset.length(std::min(remainder, inputLength));

					console.info("Copying ", inputOffset.length(), " bytes at ", inputOffset.offset(),
						" to ", outputOffset.length(), " byte region at ", outputOffset.offset());
				}
				outputOffset = {originalOutputOffset, 0};
			}
			nextInputBlock();
			inputOffset.length(blockLength(inputLength - inputOffset));
		}

		assert(file == inputFiles.end()); // NOLINT
	}

	int32_t chunkedCopy() noexcept
	{
		calculateInputChunking();
		return 0;
	}
} // namespace pcat
