#include <mappingOffset.hxx>
#include "testMappingOffset.hxx"

using pcat::mappingOffset_t;

namespace mappingOffset
{
	void testDefaultConstruct(testsuite &suite)
	{
		mappingOffset_t offset{};
		suite.assertEqual(offset.offset(), 0);
		suite.assertEqual(offset.adjustment(), 0);
		suite.assertEqual(offset.length(), 0);
		suite.assertEqual(offset.adjustedOffset(), 0);
		suite.assertEqual(offset.adjustedLength(), 0);
		suite.assertEqual(off_t{offset}, 0);
	}
} // namespace mappingOffset
