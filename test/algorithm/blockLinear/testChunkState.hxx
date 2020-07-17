#ifndef TEST_CHUNK_STATE__HXX
#define TEST_CHUNK_STATE__HXX

#include <crunch++.h>
#include <algorithm/blockLinear/chunkState.hxx>

namespace chunkState
{
	extern void testDefaultConstruct(testsuite &suite);
	extern void testNoFilesConstruct(testsuite &suite);
	extern void testFillAlignedChunk(testsuite &suite);
	extern void testFillUnalignedChunks(testsuite &suite);
}

#endif /*TEST_CHUNK_STATE__HXX*/
