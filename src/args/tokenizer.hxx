#ifndef ARGS_TOKENIZER__HXX
#define ARGS_TOKENIZER__HXX

#include <cstddef>
#include <string_view>

namespace pcat::args::tokenizer
{
	enum class tokenType_t
	{
		unknown,
		arg,
		space,
		equals
	};

	struct token_t final
	{
	private:
		tokenType_t _type;
		std::string_view _value;

	public:
		constexpr token_t() noexcept : _type{tokenType_t::unknown}, _value{} { }
		constexpr token_t(const tokenType_t type) noexcept : _type{type}, _value{} { }
		token_t(const tokenType_t type, std::string_view &&value) noexcept :
			_type{type}, _value{value} { }

		bool valid() const noexcept { return _type != tokenType_t::unknown; }
		tokenType_t type() const noexcept { return _type; }
		std::string_view value() const noexcept { return _value; }
	};

	struct tokenizer_t final
	{
	private:
		size_t currentArg;
		const char *arg;
		const char *offset;
		size_t length;
		token_t _token;
		size_t count;
		const char *const *args;

		void nextArg() noexcept;
		void readToken() noexcept;

	public:
		tokenizer_t(const size_t argsCount, const char *const *const argsList) noexcept :
			currentArg{}, arg{}, offset{}, length{}, _token{}, count{argsCount}, args{argsList}
			{ nextArg(); next(); }

		const token_t &token() const noexcept { return _token; }
		token_t &token() noexcept { return _token; }

		token_t &next() noexcept
		{
			readToken();
			return _token;
		}
	};
}

#endif /*ARGS_TOKENIZER__HXX*/
