#include <cstring>

#include "tokenizer.hxx"

namespace pcat::args::tokenizer
{
	void tokenizer_t::nextArg() noexcept
	{
		if (currentArg == count)
		{
			arg = nullptr;
			offset = nullptr;
			length = 0;
		}
		else
		{
			arg = args[currentArg++];
			offset = arg;
			length = strlen(arg);
		}
	}

	[[nodiscard]] bool isEquals(const char c) noexcept { return c == '='; }

	void tokenizer_t::readToken() noexcept
	{
		if (!arg)
			_token = {};
		else if (offset == arg + length)
		{
			nextArg();
			if (!arg)
				_token = {};
			else
				_token = {tokenType_t::space};
		}
		else if (isEquals(*offset))
		{
			++offset;
			_token = {tokenType_t::equals, {"=", 1}};
		}
		else
		{
			const auto begin = offset;
			while (offset < arg + length)
			{
				if (isEquals(*offset))
					break;
				++offset;
			}
			_token = {tokenType_t::arg, {begin, size_t(offset - begin)}};
		}
	}
}
