#include <substrate/utility>
#include <threadPool.hxx>
#include "testAffinity.hxx"

using pcat::threadPool_t;

namespace threadPool
{
	bool dummyWork() { return true; }

	void testUnused(testsuite &suite)
	{
		auto pool = substrate::make_unique_nothrow<threadPool_t<decltype(dummyWork)>>(dummyWork);
		suite.assertNotNull(pool);
		suite.assertTrue(pool->valid());
		suite.assertFalse(pool->finish());
		suite.assertFalse(pool->valid());
		suite.assertFalse(pool->finish());
	}
}
