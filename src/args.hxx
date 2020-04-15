#ifndef ARGS__HXX
#define ARGS__HXX

#include <vector>
#include <array>
#include <memory>
#include <string_view>

namespace pcat::args
{
	enum class argType_t
	{
		tree,
		unrecognised,
		help,
		version
	};

	struct argNode_t
	{
	private:
		argType_t _type;

	public:
		constexpr argNode_t(argType_t type) noexcept : _type{type} { }
		virtual ~argNode_t() noexcept = default;
		argType_t type() const noexcept { return _type; }
	};

	struct argsTree_t final : argNode_t
	{
	private:
		std::vector<std::unique_ptr<argNode_t>> _children;

	public:
		argsTree_t() noexcept : argNode_t{argType_t::tree}, _children{} { }
		[[nodiscard]] argNode_t *find(argType_t type, bool recursive = false) const noexcept;
		[[nodiscard]] bool add(std::unique_ptr<argNode_t> &&node) noexcept;

		[[nodiscard]] size_t count() const noexcept { return _children.size(); }
		[[nodiscard]] size_t size() const noexcept { return _children.size(); }

		[[nodiscard]] auto begin() const noexcept { return _children.begin(); }
		[[nodiscard]] auto end() const noexcept { return _children.end(); }
	};

	struct argUnrecognised_t final : argNode_t
	{
	private:
		std::string_view _argument;
		std::string_view _parameter;

	public:
		argUnrecognised_t(std::string_view argument) : argNode_t{argType_t::unrecognised},
			_argument{argument}, _parameter{} { }
		argUnrecognised_t(std::string_view argument, std::string_view &&parameter) :
			argNode_t{argType_t::unrecognised}, _argument{argument}, _parameter{parameter} { }
		[[nodiscard]] std::string_view argument() const noexcept { return _argument; }
		[[nodiscard]] std::string_view parameter() const noexcept { return _parameter; }
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
		std::string _option;
		argType_t _type;

	public:
		option_t(std::string &&option, const argType_t type) : _option{std::move(option)},
			_type{type} { }

		[[nodiscard]] const std::string &name() const noexcept { return _option; }
		[[nodiscard]] const std::string &option() const noexcept { return _option; }
		[[nodiscard]] argType_t type() const noexcept { return _type; }
	};
}

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

#endif /*ARGS__HXX*/
