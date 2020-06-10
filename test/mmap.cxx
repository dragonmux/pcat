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
		suite.assertEqual(map->length(), 0);
		try
		{
			[[maybe_unused]] auto addr = map->address(0);
			suite.fail("mmap_t failed to throw when asking for the address of the first item on a bad map");
		}
		catch (const std::out_of_range &) { }
	}

	void testMapEntireFile(testsuite &suite, const fd_t &fd)
	{
		suite.assertGreaterThan(fd.length(), 0);
		mmap_t map{fd, fd.length(), PROT_READ};
		suite.assertTrue(map.valid());
		suite.assertEqual(map.length(), fd.length());
		suite.assertNotNull(map.address(0));
		suite.assertTrue(map.advise(MADV_WILLNEED));
		suite.assertTrue(map.advise<MADV_SEQUENTIAL>());
		suite.assertTrue(map.advise<MADV_RANDOM, MADV_DONTNEED>());
		suite.assertTrue(map.sync());
	}
} // namespace memoryMap
