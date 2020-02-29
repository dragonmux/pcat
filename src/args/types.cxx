#include "../args.hxx"

using namespace pcat::args;

argNode_t *argsTree_t::find(const argType_t nodeType, const bool recursive) const noexcept
{
	for (const auto &node : _children)
	{
		if (node->type() == nodeType)
			return node.get();
	}

	if (recursive)
	{
		for (const auto &node : _children)
		{
			if (node->type() == argType_t::tree)
			{
				auto tree = static_cast<const argsTree_t *>(node.get());
				auto result = tree->find(nodeType, true);
				if (result)
					return result;
			}
		}
	}
	return nullptr;
}

bool argsTree_t::add(std::unique_ptr<argNode_t> &&node) noexcept try
{
	_children.emplace_back(std::move(node));
	return true;
}
catch (std::bad_alloc &)
	{ return false; }
