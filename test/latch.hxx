#ifndef LATCH__HXX
#define LATCH__HXX

#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace pcat
{
	struct latch_t final
	{
	private:
		mutable std::mutex latchMutex{};
		mutable std::condition_variable waitZero{};
		std::atomic<std::size_t> count{};

	public:
		explicit latch_t(std::size_t expected) : count{expected} { }
		~latch_t() = default;

		void countDown() noexcept { if (count) --count; }

		void wait() const noexcept
		{
			if (!count)
				waitZero.notify_all();
			else
			{
				std::unique_lock<std::mutex> lock{latchMutex};
				waitZero.wait(lock, [this]() noexcept -> bool { return !count; });
			}
		}

		void arriveAndWait() noexcept
		{
			countDown();
			wait();
		}

		latch_t(const latch_t &) = delete;
		latch_t(latch_t &&) = delete;
		latch_t &operator =(const latch_t &) = delete;
		latch_t &operator =(latch_t &&) = delete;
	};
} // namespace pcat

#endif /*LATCH__HXX*/
