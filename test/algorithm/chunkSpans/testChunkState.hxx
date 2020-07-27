#ifndef TEST_CHUNK_STATE__HXX
#define TEST_CHUNK_STATE__HXX

#include <crunch++.h>
#include <algorithm/chunkSpans/chunkState.hxx>

namespace chunkState
{
	extern void testDefaultConstruct(testsuite &suite);
	extern void testNoFilesConstruct(testsuite &suite);
	extern void testFillAlignedChunk(testsuite &suite);
	extern void testFillUnalignedChunks(testsuite &suite);
	extern void testFillLargeSpanChunk(testsuite &suite);
}

#endif /*TEST_CHUNK_STATE__HXX*/
