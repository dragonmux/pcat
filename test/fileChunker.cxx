#include <substrate/utility>
#include "testFileChunker.hxx"

std::vector<substrate::fd_t> pcat::inputFiles{};
substrate::fd_t pcat::outputFile{};

using pcat::fileChunker_t;
using pcat::chunking_t;
using pcat::chunkState_t;
using pcat::mappingOffset_t;
using pcat::transferBlockSize;
using pcat::inputFiles;
using pcat::outputFile;

namespace fileChunker
{
	void testDefaultConstruct(testsuite &suite)
	{
		suite.assertEqual(outputFile.length(), 0);
		suite.assertTrue(inputFiles.begin() == inputFiles.end());
		chunking_t state{};
		const auto beginChunk{state.subchunkState()};
		suite.assertEqual(beginChunk.inputLength(), 0);
		suite.assertTrue(beginChunk.inputOffset() == mappingOffset_t{});
		suite.assertTrue(beginChunk.outputOffset() == mappingOffset_t{});
		suite.assertTrue(beginChunk.end() == chunkState_t{});
		suite.assertTrue(beginChunk.atEnd());
		++state;
		const auto endChunk{state.subchunkState()};
		suite.assertTrue(beginChunk == endChunk);

		suite.assertTrue(fileChunker_t{}.begin() == fileChunker_t{}.end());
	}

	void testFillAlignedChunk(testsuite &suite)
	{
		suite.assertEqual(outputFile.length(), transferBlockSize);
		suite.assertFalse(inputFiles.begin() == inputFiles.end());
		suite.assertEqual(inputFiles.size(), 1);
		suite.assertEqual(inputFiles[0].length(), transferBlockSize);
		suite.assertFalse(fileChunker_t{}.begin() == fileChunker_t{}.end());
		suite.assertTrue(fileChunker_t{}.begin() != fileChunker_t{}.end());

		chunking_t beginState{};
		const auto beginChunk{beginState.subchunkState()};
		suite.assertEqual(beginChunk.inputLength(), transferBlockSize);
		suite.assertTrue(beginChunk.inputOffset() == mappingOffset_t{0, transferBlockSize});
		suite.assertTrue(beginChunk.outputOffset() == mappingOffset_t{0, transferBlockSize});
		suite.assertTrue(beginChunk.end() == chunkState_t{inputFiles.end(), 0, {}, {transferBlockSize, 0}});
		suite.assertTrue(beginChunk == *beginState);

		const chunking_t endState{inputFiles.end()};
		const auto endChunk{endState.subchunkState()};
		suite.assertEqual(endChunk.inputLength(), 0);
		suite.assertTrue(endChunk.inputOffset() == mappingOffset_t{});
		suite.assertTrue(endChunk.outputOffset() == mappingOffset_t{transferBlockSize, 0});
		suite.assertTrue(endChunk.end() == chunkState_t{inputFiles.end(), 0, {}, {transferBlockSize, 0}});
		suite.assertTrue(endChunk == *endState);

		++beginState;
		suite.assertTrue(*beginState == endChunk);
		++beginState;
		suite.assertTrue(*beginState == endChunk);
	}

	/*void testFillFirstUnalignedChunk(testsuite &suite)
	{
		auto beginState{substrate::make_unique_nothrow<fileChunker_t>
		(
			inputFiles.begin(), 1024, mappingOffset_t{0, 1024}, mappingOffset_t{0, transferBlockSize}
		)};
		const fileChunker_t midState1
		{
			inputFiles.begin() + 1, 3072, mappingOffset_t{0, 3072}, mappingOffset_t{1024, transferBlockSize - 1024}
		};
		const fileChunker_t midState2
		{
			inputFiles.begin() + 2, transferBlockSize, mappingOffset_t{0, transferBlockSize - 4096},
			mappingOffset_t{4096, transferBlockSize - 4096}
		};
		const fileChunker_t endState
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
		auto beginState{substrate::make_unique_nothrow<fileChunker_t>
		(
			inputFiles.begin() + 2, transferBlockSize, mappingOffset_t{transferBlockSize - 4096, 4096},
			mappingOffset_t{0, 4096}
		)};
		const fileChunker_t endState
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
	}*/

	void testFillUnalignedChunks(testsuite &suite)
	{
		suite.assertEqual(outputFile.length(), transferBlockSize + 4096);
		suite.assertFalse(inputFiles.begin() == inputFiles.end());
		suite.assertEqual(inputFiles.size(), 3);
		suite.assertEqual(inputFiles[0].length(), 1024);
		suite.assertEqual(inputFiles[1].length(), 3072);
		suite.assertEqual(inputFiles[2].length(), transferBlockSize);
		//testFillFirstUnalignedChunk(suite);
		//testFillSecondUnalignedChunk(suite);
	}
} // namespace fileChunker
