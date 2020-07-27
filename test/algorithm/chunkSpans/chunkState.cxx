#include <substrate/utility>
#include "testChunkState.hxx"

std::vector<substrate::fd_t> pcat::inputFiles{};

using pcat::algorithm::chunkSpans::chunkState_t;
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
		suite.assertTrue(beginState.inputFile().valid());
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

	void testFillFirstUnalignedChunk(testsuite &suite)
	{
		auto beginState{substrate::make_unique_nothrow<chunkState_t>
		(
			inputFiles.begin(), 1024, mappingOffset_t{0, 1024}, mappingOffset_t{0, transferBlockSize}
		)};
		const chunkState_t midState1
		{
			inputFiles.begin() + 1, 3072, mappingOffset_t{0, 3072}, mappingOffset_t{1024, transferBlockSize - 1024}
		};
		const chunkState_t midState2
		{
			inputFiles.begin() + 2, transferBlockSize, mappingOffset_t{0, transferBlockSize - 4096},
			mappingOffset_t{4096, transferBlockSize - 4096}
		};
		const chunkState_t endState
		{
			inputFiles.begin() + 2, transferBlockSize, mappingOffset_t{transferBlockSize - 4096},
			mappingOffset_t{transferBlockSize, 0}
		};
		suite.assertEqual(beginState->inputLength(), 1024);
		suite.assertTrue(beginState->file() == inputFiles.begin());
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertFalse(beginState->atEnd());
		suite.assertTrue(beginState->end() == endState);
		++*beginState;
		suite.assertFalse(beginState->atEnd());
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertTrue(*beginState == midState1);
		++*beginState;
		suite.assertFalse(beginState->atEnd());
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertTrue(*beginState == midState2);
		++*beginState;
		suite.assertTrue(beginState->atEnd());
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertTrue(*beginState == endState);
		suite.assertTrue(beginState->inputLength() == endState.inputLength());
		suite.assertTrue(beginState->inputOffset() == endState.inputOffset());
		suite.assertTrue(beginState->outputOffset() == endState.outputOffset());
		suite.assertTrue(beginState->file() == endState.file());
	}

	void testFillSecondUnalignedChunk(testsuite &suite)
	{
		auto beginState{substrate::make_unique_nothrow<chunkState_t>
		(
			inputFiles.begin() + 2, transferBlockSize, mappingOffset_t{transferBlockSize - 4096, 4096},
			mappingOffset_t{0, 4096}
		)};
		const chunkState_t endState
		{
			inputFiles.end(), 0, mappingOffset_t{}, mappingOffset_t{4096, 0}
		};
		suite.assertEqual(beginState->inputLength(), transferBlockSize);
		suite.assertTrue(beginState->file() == inputFiles.begin() + 2);
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertFalse(beginState->atEnd());
		suite.assertTrue(beginState->end() == endState);
		++*beginState;
		suite.assertTrue(beginState->atEnd());
		suite.assertTrue(*beginState == endState);
		suite.assertTrue(beginState->inputLength() == endState.inputLength());
		suite.assertTrue(beginState->inputOffset() == endState.inputOffset());
		suite.assertTrue(beginState->outputOffset() == endState.outputOffset());
		suite.assertTrue(beginState->file() == inputFiles.end());
	}

	void testFillUnalignedChunks(testsuite &suite)
	{
		suite.assertFalse(inputFiles.begin() == inputFiles.end());
		suite.assertEqual(inputFiles.size(), 3);
		suite.assertEqual(inputFiles[0].length(), 1024);
		suite.assertEqual(inputFiles[1].length(), 3072);
		suite.assertEqual(inputFiles[2].length(), transferBlockSize);
		testFillFirstUnalignedChunk(suite);
		testFillSecondUnalignedChunk(suite);
	}

	void testFillLargeSpanChunk(testsuite &suite)
	{
		constexpr off_t spanSize{transferBlockSize * 8};
		constexpr auto hugefileSize{(transferBlockSize * 32) - 2048};
		constexpr auto hugefileOffset{spanSize - (transferBlockSize * 2) - 2048};
		suite.assertFalse(inputFiles.begin() == inputFiles.end());
		suite.assertEqual(inputFiles.size(), 6);
		suite.assertEqual(inputFiles[0].length(), 1024);
		suite.assertEqual(inputFiles[1].length(), 2048);
		suite.assertEqual(inputFiles[2].length(), 3072);
		suite.assertEqual(inputFiles[3].length(), transferBlockSize);
		suite.assertEqual(inputFiles[4].length(), transferBlockSize - 4096);
		suite.assertEqual(inputFiles[5].length(), hugefileSize);

		auto beginState{substrate::make_unique_nothrow<chunkState_t>
		(
			inputFiles.begin(), 1024, mappingOffset_t{0, 1024}, mappingOffset_t{0, spanSize}
		)};
		const chunkState_t midState1
		{
			inputFiles.begin() + 1, 2048, mappingOffset_t{0, 2048}, mappingOffset_t{1024, spanSize - 1024}
		};
		const chunkState_t midState2
		{
			inputFiles.begin() + 2, 3072, mappingOffset_t{0, 3072}, mappingOffset_t{3072, spanSize - 3072}
		};
		const chunkState_t midState3
		{
			inputFiles.begin() + 3, transferBlockSize - 4096, mappingOffset_t{0, transferBlockSize - 4096},
			mappingOffset_t{6144, spanSize - 6144}
		};
		const chunkState_t midState4
		{
			inputFiles.begin() + 4, transferBlockSize, mappingOffset_t{0, transferBlockSize},
			mappingOffset_t{transferBlockSize + 2048, spanSize - transferBlockSize - 2048}
		};
		const chunkState_t midState5
		{
			inputFiles.begin() + 5, hugefileSize, mappingOffset_t{0, hugefileSize},
			mappingOffset_t{(transferBlockSize * 2) + 2048, hugefileOffset}
		};
		const chunkState_t endState
		{
			inputFiles.begin() + 5, hugefileSize, mappingOffset_t{hugefileOffset, hugefileSize - hugefileOffset},
			mappingOffset_t{spanSize, 0}
		};
		suite.assertEqual(beginState->inputLength(), 1024);
		suite.assertTrue(beginState->file() == inputFiles.begin());
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertFalse(beginState->atEnd());
		suite.assertTrue(beginState->end() == endState);
		++*beginState;
		suite.assertFalse(beginState->atEnd());
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertTrue(*beginState == midState1);
		++*beginState;
		suite.assertFalse(beginState->atEnd());
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertTrue(*beginState == midState2);
		++*beginState;
		suite.assertFalse(beginState->atEnd());
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertTrue(*beginState == midState3);
		++*beginState;
		suite.assertFalse(beginState->atEnd());
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertTrue(*beginState == midState4);
		++*beginState;
		suite.assertFalse(beginState->atEnd());
		suite.assertTrue(beginState->inputFile().valid());
		suite.assertTrue(*beginState == midState5);
		++*beginState;
		suite.assertTrue(beginState->atEnd());
		suite.assertTrue(*beginState == endState);
		suite.assertEqual(beginState->inputLength(), endState.inputLength());
		suite.assertTrue(beginState->inputOffset() == endState.inputOffset());
		suite.assertTrue(beginState->outputOffset() == endState.outputOffset());
		suite.assertTrue(beginState->file() == inputFiles.end());
	}
} // namespace chunkState
