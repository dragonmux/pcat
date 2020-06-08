#include <substrate/utility>
#include <mmap.hxx>
#include "testMMap.hxx"

using pcat::mmap_t;

namespace memoryMap
{
	void testDefaultConstruct(testsuite &suite)
	{
		auto map{substrate::make_unique_nothrow<mmap_t>()};
		suite.assertNotNull(map);
		suite.assertFalse(map->valid());
	}
} // namespace memoryMap
