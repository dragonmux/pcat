#ifndef ARGS__HXX
#define ARGS__HXX

#include <vector>
#include <array>
#include <memory>
#include <string_view>

namespace pcat::args
{
	enum class argType_t : uint8_t
	{
		tree,
		unrecognised,
		help,
		version,
		outputFile
	};

	struct argNode_t
	{
	private:
		argType_t type_;

	public:
		constexpr argNode_t(argType_t type) noexcept : type_{type} { }
		argNode_t(const argNode_t &) = delete;
		argNode_t(argNode_t &&) = delete;
		virtual ~argNode_t() noexcept = default;
		argNode_t &operator =(const argNode_t &) = delete;
		argNode_t &operator =(argNode_t &&) = delete;
		[[nodiscard]] auto type() const noexcept { return type_; }
	};

	struct argsTree_t final : argNode_t
	{
	private:
		std::vector<std::unique_ptr<argNode_t>> children_;

	public:
		argsTree_t() noexcept : argNode_t{argType_t::tree}, children_{} { }
		[[nodiscard]] argNode_t *find(argType_t type, bool recursive = false) const noexcept;
		[[nodiscard]] bool add(std::unique_ptr<argNode_t> &&node) noexcept;

		[[nodiscard]] auto count() const noexcept { return children_.size(); }
		[[nodiscard]] auto size() const noexcept { return children_.size(); }

		[[nodiscard]] auto begin() const noexcept { return children_.begin(); }
		[[nodiscard]] auto end() const noexcept { return children_.end(); }
	};

	struct argUnrecognised_t final : argNode_t
	{
	private:
		std::string_view argument_;
		std::string_view parameter_;

	public:
		argUnrecognised_t() = delete;
		constexpr argUnrecognised_t(const std::string_view argument) : argNode_t{argType_t::unrecognised},
			argument_{argument}, parameter_{} { }
		constexpr argUnrecognised_t(const std::string_view argument, const std::string_view parameter) :
			argNode_t{argType_t::unrecognised}, argument_{argument}, parameter_{parameter} { }
		[[nodiscard]] auto argument() const noexcept { return argument_; }
		[[nodiscard]] auto parameter() const noexcept { return parameter_; }
	};

	struct argOutputFile_t final : argNode_t
	{
	private:
		std::string_view fileName_;

	public:
		argOutputFile_t() = delete;
		constexpr argOutputFile_t(const std::string_view fileName) :
			argNode_t{argType_t::outputFile}, fileName_{fileName} { }
		[[nodiscard]] auto fileName() const noexcept { return fileName_; }
	};

	template<argType_t argType> struct argOfType_t final : argNode_t
	{
	public:
		constexpr argOfType_t() noexcept : argNode_t{argType} { }
	};

	using argHelp_t = argOfType_t<argType_t::help>;
	using argVersion_t = argOfType_t<argType_t::version>;

	struct option_t final
	{
	private:
		std::string_view option_;
		argType_t type_;

	public:
		constexpr option_t(const std::string_view option, const argType_t type) noexcept :
			option_{option}, type_{type} { }

		[[nodiscard]] constexpr auto &name() const noexcept { return option_; }
		[[nodiscard]] constexpr auto &option() const noexcept { return option_; }
		[[nodiscard]] constexpr auto type() const noexcept { return type_; }
	};
} // namespace pcat::args

using pcat::args::argType_t;

// parseArguments() as a free function that takes argc, argv and
// a descriptor structure containing information on the supported arguments as a
// parsing tree, and returns a new argsTree_t std::unique_ptr<> or nullptr

// The goal of this subsystem is to make further argument handling cheap by not having
// to continuously re-run strcmp(), having done that the minimum number of times possible
// in the args parser and encoded as much of the results as simple integers as reasonable

extern std::unique_ptr<pcat::args::argsTree_t> args;
using pcat::args::argType_t;

extern bool parseArguments(size_t argCount, const char *const *argList,
	const pcat::args::option_t *optionsBegin,
	const pcat::args::option_t *optionsEnd);

template<size_t optionCount> bool parseArguments(const size_t argCount, const char *const *const argList,
	const std::array<pcat::args::option_t, optionCount> &options)
{
	return parseArguments(argCount, argList, options.begin(), options.end());
}

extern void dumpAST() noexcept;

#endif /*ARGS__HXX*/
