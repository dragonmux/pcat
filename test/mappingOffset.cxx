#include <mappingOffset.hxx>
#include "testMappingOffset.hxx"

using pcat::mappingOffset_t;
using pcat::pageSize;

namespace mappingOffset
{
	void testDefaultConstruct(testsuite &suite)
	{
		const mappingOffset_t offset{};
		suite.assertEqual(offset.offset(), 0);
		suite.assertEqual(offset.adjustment(), 0);
		suite.assertEqual(offset.length(), 0);
		suite.assertEqual(offset.adjustedOffset(), 0);
		suite.assertEqual(offset.adjustedLength(), 0);
		suite.assertEqual(off_t{offset}, 0);
	}

	void testOffsetConstruct(testsuite &suite)
	{
		const mappingOffset_t zeroOffset{0};
		suite.assertEqual(zeroOffset.offset(), 0);
		suite.assertEqual(zeroOffset.adjustment(), 0);
		suite.assertEqual(zeroOffset.length(), 0);
		suite.assertEqual(zeroOffset.adjustedOffset(), 0);
		suite.assertEqual(zeroOffset.adjustedLength(), 0);
		suite.assertEqual(off_t{zeroOffset}, 0);

		const mappingOffset_t unalignedOffset{524};
		suite.assertEqual(unalignedOffset.offset(), 524);
		suite.assertEqual(unalignedOffset.adjustment(), 524);
		suite.assertEqual(unalignedOffset.length(), 0);
		suite.assertEqual(unalignedOffset.adjustedOffset(), 0);
		suite.assertEqual(unalignedOffset.adjustedLength(), 524);
		suite.assertEqual(off_t{unalignedOffset}, 524);

		const mappingOffset_t alignedOffset{pageSize};
		suite.assertEqual(alignedOffset.offset(), pageSize);
		suite.assertEqual(alignedOffset.adjustment(), 0);
		suite.assertEqual(alignedOffset.length(), 0);
		suite.assertEqual(alignedOffset.adjustedOffset(), pageSize);
		suite.assertEqual(alignedOffset.adjustedLength(), 0);
		suite.assertEqual(off_t{alignedOffset}, pageSize);
	}
} // namespace mappingOffset
