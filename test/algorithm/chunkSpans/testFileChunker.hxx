#ifndef TEST_FILE_CHUNKER__HXX
#define TEST_FILE_CHUNKER__HXX

#include <crunch++.h>
#include <algorithm/chunkSpans/fileChunker.hxx>

namespace fileChunker
{
	extern void testDefaultConstruct(testsuite &suite);
	extern void testFillAlignedChunk(testsuite &suite);
	extern void testFillUnalignedChunks(testsuite &suite);
}

#endif /*TEST_FILE_CHUNKER__HXX*/
