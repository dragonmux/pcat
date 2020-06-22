#ifndef THREAD_POOL__HXX
#define THREAD_POOL__HXX

#include <cstdint>
#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <tuple>
#include <utility>
#include <chrono>
#include "affinity.hxx"
#include "threadedQueue.hxx"

namespace pcat
{
	using namespace std::literals::chrono_literals;

	template<typename workFunc_t> struct threadPool_t;

	template<typename result_t, typename... args_t> struct threadPool_t<result_t(args_t...)> final
	{
	private:
		using workFunc_t = result_t (*)(args_t...);
		std::atomic<std::size_t> waitingThreads{};
		std::mutex workMutex{};
		std::condition_variable haveWork{};
		threadedQueue_t<std::tuple<args_t...>> work{};
		std::atomic<bool> finished{false};
		threadedQueue_t<result_t> results{};
		std::vector<std::thread> threads{};
		affinity_t affinity{};
		workFunc_t workerFunction;

		std::pair<bool, std::tuple<args_t...>> waitWork() noexcept
		{
			std::unique_lock<std::mutex> lock{workMutex};
			++waitingThreads;
			// wait, but protect ourselves from accidental wake-ups..
			haveWork.wait(lock, [this]() noexcept -> bool { return finished || !work.empty(); });
			--waitingThreads;
			if (!work.empty())
				return {true, work.pop()};
			return {false, {}};
		}

		template<std::size_t... indicies> auto invoke(std::tuple<args_t...> &&args,
			std::index_sequence<indicies...>) { return workerFunction(std::get<indicies>(std::move(args))...); }

		void workerThread(const int32_t processor)
		{
			affinity.pinThreadTo(processor);
			while (!(finished && work.empty()))
			{
				auto [valid, args] = waitWork();
				// This checks for both if we don't have something to do and if we're supposed to be finishing up
				if (finished && !valid)
					break;
				auto result = invoke(std::move(args), std::make_index_sequence<sizeof...(args_t)>());
				results.push(std::move(result));
			}
		}

		auto clearResultQueue()
		{
			result_t result{};
			while (!results.empty())
			{
				const auto thisResult = results.pop();
				if (!result)
					result = thisResult;
			}
			return result;
		}

	public:
		threadPool_t(const workFunc_t function) : workerFunction{function}
		{
			for (const uint32_t processor : affinity)
				threads.emplace_back(std::thread{[this](const int32_t processor) -> void
					{ workerThread(processor); }, processor});
			while (!ready())
				std::this_thread::sleep_for(1us);
		}
		threadPool_t(const threadPool_t &) = delete;
		threadPool_t(threadPool_t &&) = delete;
		~threadPool_t() noexcept { [[maybe_unused]] const auto result = finish(); }
		threadPool_t &operator =(const threadPool_t &) = delete;
		threadPool_t &operator =(threadPool_t &&) = delete;

		[[nodiscard]] auto numProcessors() const noexcept { return affinity.numProcessors(); }
		[[nodiscard]] auto valid() const noexcept { return !threads.empty(); }
		[[nodiscard]] auto ready() const noexcept { return waitingThreads == affinity.numProcessors(); }

		[[nodiscard]] auto queue(args_t ...args)
		{
			work.emplace(std::forward<args_t>(args)...);
			haveWork.notify_one();
			return clearResultQueue();
		}

		[[nodiscard]] result_t finish()
		{
			if (threads.empty())
				return {};
			finished = true;
			haveWork.notify_all();
			for (auto &thread : threads)
				thread.join();
			threads.clear();
			return clearResultQueue();
		}
	};

	template<typename result_t, typename... args_t>
		threadPool_t(result_t (*)(args_t...)) -> threadPool_t<result_t(args_t...)>;
} // namespace pcat

#endif /*THREAD_POOL__HXX*/
