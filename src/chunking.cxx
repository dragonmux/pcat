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
		off_t offset_{0};
		off_t adjust_{0};
		off_t length_{0};

		constexpr void recalcAdjustment()
		{
			adjust_ = offset_ % pageSize;
			offset_ -= adjust_;
		}

	public:
		constexpr mappingOffset_t() noexcept = default;
		constexpr mappingOffset_t(const off_t offset) noexcept : offset_{offset} { recalcAdjustment(); }
		constexpr mappingOffset_t(const off_t offset, const off_t length) noexcept :
			offset_{offset}, length_{length} { recalcAdjustment(); }
		[[nodiscard]] constexpr off_t offset() const noexcept { return offset_ + adjust_; }
		[[nodiscard]] constexpr off_t adjustment() const noexcept { return adjust_; }
		[[nodiscard]] constexpr off_t length() const noexcept { return length_; }
		[[nodiscard]] constexpr off_t adjustedOffset() const noexcept { return offset_; }
		[[nodiscard]] constexpr off_t adjustedLength() const noexcept { return length_ + adjust_; }
		constexpr void length(const off_t length) noexcept { length_ = length; }
		constexpr operator off_t() const noexcept { return offset(); }

		constexpr mappingOffset_t &operator +=(const off_t amount) noexcept
		{
			offset_ += amount + adjust_;
			recalcAdjustment();
			return *this;
		}
	};

	constexpr off_t blockLength(const off_t length)
		{ return std::min(transferBlockSize, length); }

	void calculateInputChunking() noexcept
	{
		auto file = inputFiles.begin();
		off_t inputLength = file->length();
		mappingOffset_t inputOffset{0, blockLength(inputLength)};

		const off_t outputLength = outputFile.length();
		for (mappingOffset_t outputOffset{}; outputOffset < outputLength; outputOffset += transferBlockSize)
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
					assert(file <= inputFiles.end()); // NOLINT
					inputLength = file == inputFiles.end() ? 0 : file->length();
					inputOffset = {};
				}
			};

			if (outputOffset.length() != inputOffset.length())
			{
				const off_t originalOutputOffset = outputOffset.offset();
				while (outputOffset.length() - inputOffset.length())
				{
					const off_t remainder = outputOffset.length() - inputOffset.length();
					console.info("Transfer caused a remainder of ", remainder, " bytes to go for output block");
					outputOffset += inputOffset.length();
					outputOffset.length(remainder);
					nextInputBlock();
					inputOffset.length(std::min(remainder, inputLength));

					console.info("Copying ", inputOffset.length(), " bytes at ", inputOffset.offset(),
						" to ", outputOffset.length(), " byte region at ", outputOffset.offset());
				}
				outputOffset = {originalOutputOffset};
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
