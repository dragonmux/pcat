#include <substrate/console>
#include <substrate/utility>
#include "utils/span.hxx"
#include "args.hxx"
#include "args/tokenizer.hxx"

using substrate::console;
using pcat::utils::span_t;
using namespace pcat::args;
using namespace pcat::args::tokenizer;
using std::literals::string_view_literals::operator ""sv;

std::unique_ptr<argsTree_t> args{};
bool needASTDump = false;

const char *typeToName(const tokenType_t type)
{
	switch (type)
	{
	case tokenType_t::unknown:
		return "unknown";
	case tokenType_t::arg:
		return "argument";
	case tokenType_t::space:
		return "implicit space";
	case tokenType_t::equals:
		return "equals sign";
	}
	return nullptr;
}

struct indentBy_t : substrate::printable_t
{
private:
	size_t indentCount;

public:
	constexpr indentBy_t(const size_t indent) noexcept : indentCount{indent} { }

	void operator ()(const substrate::consoleStream_t &stream) const noexcept final
	{
		if (indentCount)
			stream.write(' ');
		for (size_t i{0}; i < indentCount; ++i)
			stream.write('\t');
	}
};

void dumpAST(argsTree_t *node, size_t indent) noexcept;
void dumpAST(argNode_t *node, size_t indent) noexcept;

void dumpAST() noexcept
{
	if (!args)
		console.error("Fatally failed to parse any arguments"sv); // NOLINT(readability-magic-numbers)
	else
		dumpAST(args.get(), 0);
}

void dumpUnrecogised(const argUnrecognised_t &node, const size_t indent) noexcept
{
	const auto argument{node.argument()};
	const auto parameter{node.parameter()};
	if (parameter.empty())
		console.warn(indentBy_t{indent}, "Parsed unknown argument '"sv, argument, "'"sv); // NOLINT(readability-magic-numbers)
	else
		console.warn(indentBy_t{indent}, "Parsed unknown argument '"sv, argument, '=', parameter, "'"sv); // NOLINT(readability-magic-numbers)
}

void dumpOutputFile(const argOutputFile_t &node, const size_t indent) noexcept
	{ console.info(indentBy_t{indent}, "Parsed output file: "sv, node.fileName()); }

void dumpAST(argNode_t *node, const size_t indent) noexcept
{
	switch (node->type())
	{
		case argType_t::tree:
			dumpAST(static_cast<argsTree_t *>(node), indent);
			break;
		case argType_t::unrecognised:
			dumpUnrecogised(*static_cast<argUnrecognised_t *>(node), indent);
			break;
		case argType_t::help:
			console.info(indentBy_t{indent}, "Parsed help"sv); // NOLINT(readability-magic-numbers)
			break;
		case argType_t::version:
			console.info(indentBy_t{indent}, "Parsed version"sv); // NOLINT(readability-magic-numbers)
			break;
		case argType_t::outputFile:
			dumpOutputFile(*static_cast<argOutputFile_t *>(node), indent);
			break;
		default:
			console.error("An internal error has occured"sv); // NOLINT(readability-magic-numbers)
	}
}

void dumpAST(argsTree_t *node, const size_t indent) noexcept
{
	console.info(indentBy_t{indent}, "Parsed an argument tree containing "sv, node->count(), " arguments"sv); // NOLINT(readability-magic-numbers)
	for (const auto &arg : *node)
		dumpAST(arg.get(), indent + 1);
}

bool parseArgument(tokenizer_t &lexer, const span_t<const option_t> &options, argsTree_t &ast);

auto parseTree(tokenizer_t &lexer, const span_t<const option_t> &options)
{
	auto tree{substrate::make_unique<argsTree_t>()};
	if (parseArgument(lexer, options, *tree))
		return tree;
	throw std::exception{};
}

auto parseOutputFile(tokenizer_t &lexer)
{
	const auto &token{lexer.token()};
	if (token.type() == tokenType_t::unknown)
	{
		console.error("Output file option given but failed to specify output file name"sv); // NOLINT(readability-magic-numbers)
		throw std::exception{};
	}
	lexer.next();
	const auto fileName{token.value()};
	lexer.next();
	return substrate::make_unique<argOutputFile_t>(fileName);
}

std::unique_ptr<argNode_t> makeNode(tokenizer_t &lexer, const span_t<const option_t> &options, const option_t &option)
{
	lexer.next();
	switch (option.type())
	{
		case argType_t::tree:
			return parseTree(lexer, options);
		case argType_t::help:
			return substrate::make_unique<argHelp_t>();
		case argType_t::version:
			return substrate::make_unique<argVersion_t>();
		case argType_t::outputFile:
			return parseOutputFile(lexer);
		default:
			throw std::exception{};
	}
}

bool parseArgument(tokenizer_t &lexer, const span_t<const option_t> &options, argsTree_t &ast)
{
	const auto &token{lexer.token()};
	if (token.type() == tokenType_t::space)
		lexer.next();
	else if (token.type() != tokenType_t::arg)
		return false;
	const auto argument{token.value()};
	for (const auto &option : options)
	{
		if (argument == option.name())
			return ast.add(makeNode(lexer, options, option));
	}
	lexer.next();
	if (argument == "--dump-ast"sv) // NOLINT(readability-magic-numbers)
		return needASTDump = true;
	else if (token.type() != tokenType_t::equals)
	{
		if (!ast.add(substrate::make_unique<argUnrecognised_t>(argument)))
			return false;
	}
	else
	{
		lexer.next();
		if (token.type() == tokenType_t::space)
		{
			if (!ast.add(substrate::make_unique<argUnrecognised_t>(argument)))
				return false;
		}
		else if (!ast.add(substrate::make_unique<argUnrecognised_t>(argument, token.value())))
			return false;
	}
	return true;
}

// Recursive descent parser that efficiently matches the current token from argv against
// the set of allowed arguments at the current parsing level, and returns their AST
// representation if matched, allowing the parser to build a neat tree of all
// the arguments for further use by the caller
bool parseArguments(const size_t argCount, const char *const *const argList,
	const option_t *const optionsBegin,
	const option_t *const optionsEnd) try
{
	if (argCount < 2 || !argList)
		return false;
	// Skip the first argument (that's the name of the program) and start
	// tokenizing directly at the second.
	tokenizer_t lexer{argCount - 1, argList + 1};
	const auto &token{lexer.token()};
	const span_t options{optionsBegin, optionsEnd};
	args = substrate::make_unique<argsTree_t>();
	needASTDump = false;

	while (token.valid())
	{
		if (!parseArgument(lexer, options, *args))
		{
			std::string argument{token.value()};
			console.warn("Found invalid token '"sv, argument, "' ("sv, // NOLINT(readability-magic-numbers)
				typeToName(token.type()), ") in arguments"sv); // NOLINT(readability-magic-numbers)
			return false;
		}
	}

	if (needASTDump)
		dumpAST();
	return true;
}
catch (std::exception &)
	{ return false; }
