#include <substrate/utility>
#include "testFileChunker.hxx"

std::vector<substrate::fd_t> pcat::inputFiles{};
substrate::fd_t pcat::outputFile{};

using pcat::algorithm::chunkSpans::fileChunker_t;
using pcat::algorithm::chunkSpans::chunking_t;
using pcat::algorithm::chunkSpans::chunkState_t;
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
		chunking_t state{transferBlockSize};
		const auto beginChunk{state.subchunkState()};
		suite.assertEqual(beginChunk.inputLength(), 0);
		suite.assertTrue(beginChunk.inputOffset() == mappingOffset_t{});
		suite.assertTrue(beginChunk.outputOffset() == mappingOffset_t{});
		suite.assertTrue(beginChunk.end() == chunkState_t{});
		suite.assertTrue(beginChunk.atEnd());
		++state;
		const auto endChunk{state.subchunkState()};
		suite.assertTrue(beginChunk == endChunk);

		suite.assertTrue(fileChunker_t{transferBlockSize}.begin() == fileChunker_t{transferBlockSize}.end());
	}

	void testFillAlignedChunk(testsuite &suite)
	{
		suite.assertEqual(outputFile.length(), transferBlockSize);
		suite.assertFalse(inputFiles.begin() == inputFiles.end());
		suite.assertEqual(inputFiles.size(), 1);
		suite.assertEqual(inputFiles[0].length(), transferBlockSize);
		suite.assertFalse(fileChunker_t{transferBlockSize}.begin() == fileChunker_t{transferBlockSize}.end());
		suite.assertTrue(fileChunker_t{transferBlockSize}.begin() != fileChunker_t{transferBlockSize}.end());

		chunking_t beginState{transferBlockSize};
		const auto beginChunk{beginState.subchunkState()};
		suite.assertEqual(beginChunk.inputLength(), transferBlockSize);
		suite.assertTrue(beginChunk.inputOffset() == mappingOffset_t{0, transferBlockSize});
		suite.assertTrue(beginChunk.outputOffset() == mappingOffset_t{0, transferBlockSize});
		suite.assertTrue(beginChunk.end() == chunkState_t{inputFiles.end(), 0, {}, {transferBlockSize, 0}});
		suite.assertTrue(beginChunk == *beginState);

		const chunking_t endState{transferBlockSize, inputFiles.end()};
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

	void testFillUnalignedChunks(testsuite &suite)
	{
		suite.assertEqual(outputFile.length(), transferBlockSize + 4096);
		suite.assertFalse(inputFiles.begin() == inputFiles.end());
		suite.assertEqual(inputFiles.size(), 3);
		suite.assertEqual(inputFiles[0].length(), 1024);
		suite.assertEqual(inputFiles[1].length(), 3072);
		suite.assertEqual(inputFiles[2].length(), transferBlockSize);
		suite.assertFalse(fileChunker_t{transferBlockSize}.begin() == fileChunker_t{transferBlockSize}.end());
		suite.assertTrue(fileChunker_t{transferBlockSize}.begin() != fileChunker_t{transferBlockSize}.end());

		chunking_t beginState{transferBlockSize};
		const auto beginChunk{*beginState};
		suite.assertTrue(beginChunk.file() == inputFiles.begin());
		suite.assertEqual(beginChunk.inputLength(), 1024);
		suite.assertTrue(beginChunk.inputOffset() == mappingOffset_t{0, 1024});
		suite.assertTrue(beginChunk.outputOffset() == mappingOffset_t{0, transferBlockSize});
		suite.assertTrue(beginChunk.end() == chunkState_t{inputFiles.begin() + 2, transferBlockSize,
			{transferBlockSize - 4096, 4096}, {transferBlockSize, 0}});

		const chunking_t endState{transferBlockSize, inputFiles.end()};
		const auto endChunk{*endState};
		suite.assertTrue(endChunk.file() == inputFiles.end());
		suite.assertEqual(endChunk.inputLength(), 0);
		suite.assertTrue(endChunk.inputOffset() == mappingOffset_t{});
		suite.assertTrue(endChunk.outputOffset() == mappingOffset_t{transferBlockSize + 4096, 0});
		suite.assertTrue(endChunk.end() == chunkState_t{inputFiles.end(), 0, {}, {transferBlockSize + 4096, 0}});

		++beginState;
		const auto chunk{*beginState};
		suite.assertTrue(chunk.file() == inputFiles.begin() + 2);
		suite.assertEqual(chunk.inputLength(), transferBlockSize);
		suite.assertTrue(chunk.inputOffset() == mappingOffset_t{transferBlockSize - 4096, 4096});
		suite.assertTrue(chunk.outputOffset() == mappingOffset_t{transferBlockSize, 4096});
		suite.assertTrue(chunk.end() == chunkState_t{inputFiles.end(), 0, {}, {transferBlockSize + 4096, 0}});

		++beginState;
		suite.assertTrue(*beginState == endChunk);
		++beginState;
		suite.assertTrue(*beginState == endChunk);
	}

	void testFillLargeSpanChunk(testsuite &suite)
	{
		constexpr off_t spanSize{transferBlockSize * 8};
		constexpr auto totalHugeSize{transferBlockSize * 34};
		constexpr auto hugefileSize{(transferBlockSize * 32) - 2048};
		constexpr auto hugefileOffset{spanSize - (transferBlockSize * 2) - 2048};

		suite.assertEqual(outputFile.length(), transferBlockSize * 34U);
		suite.assertFalse(inputFiles.begin() == inputFiles.end());
		suite.assertEqual(inputFiles.size(), 6);
		suite.assertEqual(inputFiles[0].length(), 1024);
		suite.assertEqual(inputFiles[1].length(), 2048);
		suite.assertEqual(inputFiles[2].length(), 3072);
		suite.assertEqual(inputFiles[3].length(), transferBlockSize);
		suite.assertEqual(inputFiles[4].length(), transferBlockSize - 4096);
		suite.assertEqual(inputFiles[5].length(), hugefileSize);
		suite.assertFalse(fileChunker_t{spanSize}.begin() == fileChunker_t{spanSize}.end());
		suite.assertTrue(fileChunker_t{spanSize}.begin() != fileChunker_t{spanSize}.end());

		chunking_t beginState{spanSize};
		const auto beginChunk{*beginState};
		suite.assertTrue(beginChunk.file() == inputFiles.begin());
		suite.assertEqual(beginChunk.inputLength(), 1024);
		suite.assertTrue(beginChunk.inputOffset() == mappingOffset_t{0, 1024});
		suite.assertTrue(beginChunk.outputOffset() == mappingOffset_t{0, spanSize});
		suite.assertTrue(beginChunk.end() == chunkState_t{inputFiles.begin() + 5, hugefileSize,
			{hugefileOffset, hugefileSize - hugefileOffset}, {spanSize, 0}});

		const chunking_t endState{spanSize, inputFiles.end()};
		const auto endChunk{*endState};
		suite.assertTrue(endChunk.file() == inputFiles.end());
		suite.assertEqual(endChunk.inputLength(), 0);
		suite.assertTrue(endChunk.inputOffset() == mappingOffset_t{});
		suite.assertTrue(endChunk.outputOffset() == mappingOffset_t{totalHugeSize, 0});
		suite.assertTrue(endChunk.end() == chunkState_t{inputFiles.end(), 0, {}, {totalHugeSize, 0}});

	}
} // namespace fileChunker
