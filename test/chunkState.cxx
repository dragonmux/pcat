#include <substrate/utility>
#include "testChunkState.hxx"

std::vector<substrate::fd_t> pcat::inputFiles{};

using pcat::chunkState_t;
using pcat::mappingOffset_t;
using pcat::transferBlockSize;
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
		suite.assertTrue(inputFiles.begin() == inputFiles.end());
		auto state{substrate::make_unique_nothrow<chunkState_t>
		(
			inputFiles.begin(), 0, mappingOffset_t{}, mappingOffset_t{}
		)};
		suite.assertNotNull(state);
		suite.assertEqual(state->inputLength(), 0);
		suite.assertTrue(state->inputOffset() == mappingOffset_t{});
		suite.assertTrue(state->outputOffset() == mappingOffset_t{});
		suite.assertTrue(state->atEnd());
		suite.assertTrue(state->end() == *state);
		suite.assertTrue(state->file() == inputFiles.begin());
	}

	void testFillAlignedChunk(testsuite &suite)
	{
		suite.assertFalse(inputFiles.begin() == inputFiles.end());
		suite.assertEqual(inputFiles.size(), 1);
		suite.assertEqual(inputFiles[0].length(), transferBlockSize);
		chunkState_t beginState
		{
			inputFiles.begin(), transferBlockSize, mappingOffset_t{0, transferBlockSize},
			mappingOffset_t{0, transferBlockSize}
		};
		const chunkState_t endState
		{
			inputFiles.end(), 0, mappingOffset_t{}, mappingOffset_t{transferBlockSize, 0}
		};
		suite.assertEqual(beginState.inputLength(), transferBlockSize);
		suite.assertTrue(beginState.file() == inputFiles.begin());
		suite.assertFalse(beginState.atEnd());
		suite.assertTrue(beginState.end() == endState);
		++beginState;
		suite.assertTrue(beginState.atEnd());
		suite.assertTrue(beginState == endState);
		suite.assertEqual(beginState.inputLength(), endState.inputLength());
		suite.assertTrue(beginState.inputOffset() == endState.inputOffset());
		suite.assertTrue(beginState.outputOffset() == endState.outputOffset());
		suite.assertTrue(beginState.file() == inputFiles.end());
	}
} // namespace chunkState
