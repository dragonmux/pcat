#include <substrate/utility>
#include "testChunkState.hxx"

using pcat::chunkState_t;
using pcat::mappingOffset_t;

std::vector<substrate::fd_t> pcat::inputFiles;

namespace chunkState
{
	void testDefaultConstruct(testsuite &suite)
	{
		auto state{substrate::make_unique_nothrow<chunkState_t>()};
		suite.assertNotNull(state);
		suite.assertEqual(state->inputLength(), 0);
		suite.assertTrue(state->inputOffset() == mappingOffset_t{});
		suite.assertTrue(state->outputOffset() == mappingOffset_t{});
		suite.assertTrue(state->end() == chunkState_t{});
	}
} // namespace chunkState
