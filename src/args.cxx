#include "utils/span.hxx"
#include "args.hxx"
#include "args/tokenizer.hxx"

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

void indentBy(const size_t indent) noexcept
{
	for (size_t i{0}; i < indent; ++i)
		putc('\t', stdout);
}

void dumpAST(argsTree_t *node, const size_t indent) noexcept;
void dumpAST(argNode_t *node, const size_t indent) noexcept;

void dumpAST() noexcept
{
	if (!args)
		puts("Fatally failed to parse any arguments");
	else
		dumpAST(args.get(), 0);
}

void dumpUnrecogised(const argUnrecognised_t &node) noexcept
{
	std::string argument{node.argument()};
	std::string parameter{node.parameter()};
	if (parameter.empty())
		printf("Parsed unknown argument '%s'\n", argument.data());
	else
		printf("Parsed unknown argument '%s=%s'\n", argument.data(), parameter.data());
}

void dumpAST(argNode_t *node, const size_t indent) noexcept
{
	if (node->type() != argType_t::tree)
		indentBy(indent);
	switch (node->type())
	{
		case argType_t::tree:
			dumpAST(static_cast<argsTree_t *>(node), indent);
			break;
		case argType_t::unrecognised:
			dumpUnrecogised(*static_cast<argUnrecognised_t *>(node));
			break;
		case argType_t::help:
			puts("Parsed help");
			break;
		case argType_t::version:
			puts("Parsed version");
			break;
		default:
			puts("An internal error has occured");
	}
}

void dumpAST(argsTree_t *node, const size_t indent) noexcept
{
	indentBy(indent);
	printf("Parsed an argument tree containing %zu arguments\n", node->count());
	for (const auto &arg : *node)
		dumpAST(arg.get(), indent + 1);
}

bool parseArgument(tokenizer_t &lexer, const span_t<const option_t> &options, argsTree_t &ast);

std::unique_ptr<argNode_t> parseTree(tokenizer_t &lexer, const span_t<const option_t> &options)
{
	auto tree = std::make_unique<argsTree_t>();
	if (parseArgument(lexer, options, *tree))
		return tree;
	throw std::exception{};
}

std::unique_ptr<argNode_t> makeNode(tokenizer_t &lexer, const span_t<const option_t> &options, const option_t &option)
{
	lexer.next();
	switch (option.type())
	{
		case argType_t::tree:
			return parseTree(lexer, options);
		case argType_t::help:
			return std::make_unique<argHelp_t>();
		case argType_t::version:
			return std::make_unique<argVersion_t>();
		default:
			throw std::exception{};
	}
}

bool parseArgument(tokenizer_t &lexer, const span_t<const option_t> &options, argsTree_t &ast)
{
	const token_t &token = lexer.token();
	if (token.type() == tokenType_t::space)
		lexer.next();
	else if (token.type() != tokenType_t::arg)
		return false;
	const auto argument{token.value()};
	for (const auto &option : options)
	{
		if (argument == option.name())
		{
			ast.add(makeNode(lexer, options, option));
			return true;
		}
	}
	lexer.next();
	if (argument == "--dump-ast"sv)
		return needASTDump = true;
	else if (token.type() != tokenType_t::equals)
		ast.add(std::make_unique<argUnrecognised_t>(argument));
	else
	{
		lexer.next();
		if (token.type() == tokenType_t::space)
			ast.add(std::make_unique<argUnrecognised_t>(argument));
		else
			ast.add(std::make_unique<argUnrecognised_t>(argument, token.value()));
	}
	return true;
}

// Recursive descent parser that efficiently matches the current token from argv against
// the set of allowed arguments at the current parsing level, and returns their AST
// representation if matched, allowing the parser to build a neat tree of all
// the arguments for further use by the caller
bool parseArguments(const size_t argCount, const char *const *const argList,
	const option_t *const optionsBegin,
	const option_t *const optionsEnd)
{
	if (argCount < 2 || !argList)
		return false;
	// Skip the first argument (that's the name of the program) and start
	// tokenizing directly at the second.
	tokenizer_t lexer{argCount - 1, argList + 1};
	const token_t &token = lexer.token();
	const span_t options{optionsBegin, optionsEnd};
	args = std::make_unique<argsTree_t>();
	needASTDump = false;

	while (token.valid())
	{
		if (!parseArgument(lexer, options, *args))
		{
			std::string argument{token.value()};
			printf("Found invalid token '%s' (%s) in arguments\n", argument.data(), typeToName(token.type()));
			return false;
		}
	}
	puts("End of token stream");

	if (needASTDump)
		dumpAST();
	return true;
}
