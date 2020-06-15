#include <substrate/utility>
#include "testChunkState.hxx"

std::vector<substrate::fd_t> pcat::inputFiles{};

using pcat::chunkState_t;
using pcat::mappingOffset_t;
using pcat::inputFiles;

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
		suite.assertTrue(state->atEnd());
		++*state;
		suite.assertTrue(state->atEnd());
	}

	void testNoFilesConstruct(testsuite &suite)
	{
		auto state{substrate::make_unique_nothrow<chunkState_t>(
			inputFiles.begin(), 0, mappingOffset_t{}, mappingOffset_t{}
		)};
		suite.assertNotNull(state);
		suite.assertEqual(state->inputLength(), 0);
		suite.assertTrue(state->inputOffset() == mappingOffset_t{});
		suite.assertTrue(state->outputOffset() == mappingOffset_t{});
	}
} // namespace chunkState
