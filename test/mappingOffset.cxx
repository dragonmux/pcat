#include <substrate/utility>
#include <mappingOffset.hxx>
#include "testMappingOffset.hxx"

using pcat::mappingOffset_t;
using pcat::pageSize;

namespace mappingOffset
{
	using pcat::off_t;

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

	void testLengthConstruct(testsuite &suite)
	{
		const mappingOffset_t zeroOffset{0, 256};
		suite.assertEqual(zeroOffset.offset(), 0);
		suite.assertEqual(zeroOffset.adjustment(), 0);
		suite.assertEqual(zeroOffset.length(), 256);
		suite.assertEqual(zeroOffset.adjustedOffset(), 0);
		suite.assertEqual(zeroOffset.adjustedLength(), 256);
		suite.assertEqual(off_t{zeroOffset}, 0);

		const mappingOffset_t unalignedOffset{524, pageSize - 524};
		suite.assertEqual(unalignedOffset.offset(), 524);
		suite.assertEqual(unalignedOffset.adjustment(), 524);
		suite.assertEqual(unalignedOffset.length(), pageSize - 524);
		suite.assertEqual(unalignedOffset.adjustedOffset(), 0);
		suite.assertEqual(unalignedOffset.adjustedLength(), pageSize);
		suite.assertEqual(off_t{unalignedOffset}, 524);

		const mappingOffset_t alignedOffset{pageSize, 2408};
		suite.assertEqual(alignedOffset.offset(), pageSize);
		suite.assertEqual(alignedOffset.adjustment(), 0);
		suite.assertEqual(alignedOffset.length(), 2408);
		suite.assertEqual(alignedOffset.adjustedOffset(), pageSize);
		suite.assertEqual(alignedOffset.adjustedLength(), 2408);
		suite.assertEqual(off_t{alignedOffset}, pageSize);
	}

	void testOffsetRecalculation(testsuite &suite)
	{
		// Use of make_unique_nothrow forces construction into runtime.
		auto zeroOffset{substrate::make_unique_nothrow<mappingOffset_t>()};
		suite.assertNotNull(zeroOffset);
		suite.assertEqual(zeroOffset->offset(), 0);
		suite.assertEqual(zeroOffset->adjustment(), 0);
		suite.assertEqual(zeroOffset->length(), 0);
		suite.assertEqual(zeroOffset->adjustedOffset(), 0);
		suite.assertEqual(zeroOffset->adjustedLength(), 0);
		suite.assertEqual(off_t{*zeroOffset}, 0);
		zeroOffset->length(256);
		suite.assertEqual(zeroOffset->offset(), 0);
		suite.assertEqual(zeroOffset->adjustment(), 0);
		suite.assertEqual(zeroOffset->length(), 256);
		suite.assertEqual(zeroOffset->adjustedOffset(), 0);
		suite.assertEqual(zeroOffset->adjustedLength(), 256);
		suite.assertEqual(off_t{*zeroOffset}, 0);
		*zeroOffset += 256;
		suite.assertEqual(zeroOffset->offset(), 256);
		suite.assertEqual(zeroOffset->adjustment(), 256);
		suite.assertEqual(zeroOffset->length(), 256);
		suite.assertEqual(zeroOffset->adjustedOffset(), 0);
		suite.assertEqual(zeroOffset->adjustedLength(), 512);
		suite.assertEqual(off_t{*zeroOffset}, 256);

		auto unalignedOffset{substrate::make_unique_nothrow<mappingOffset_t>(524)};
		suite.assertNotNull(unalignedOffset);
		suite.assertEqual(unalignedOffset->offset(), 524);
		suite.assertEqual(unalignedOffset->adjustment(), 524);
		suite.assertEqual(unalignedOffset->length(), 0);
		suite.assertEqual(unalignedOffset->adjustedOffset(), 0);
		suite.assertEqual(unalignedOffset->adjustedLength(), 524);
		suite.assertEqual(off_t{*unalignedOffset}, 524);
		unalignedOffset->length((pageSize * 2) - 524);
		suite.assertEqual(unalignedOffset->offset(), 524);
		suite.assertEqual(unalignedOffset->adjustment(), 524);
		suite.assertEqual(unalignedOffset->length(), (pageSize * 2) - 524);
		suite.assertEqual(unalignedOffset->adjustedOffset(), 0);
		suite.assertEqual(unalignedOffset->adjustedLength(), pageSize * 2);
		suite.assertEqual(off_t{*unalignedOffset}, 524);
		*unalignedOffset += pageSize - 524;
		suite.assertEqual(unalignedOffset->offset(), pageSize);
		suite.assertEqual(unalignedOffset->adjustment(), 0);
		suite.assertEqual(unalignedOffset->length(), (pageSize * 2) - 524);
		suite.assertEqual(unalignedOffset->adjustedOffset(), pageSize);
		suite.assertEqual(unalignedOffset->adjustedLength(), (pageSize * 2) - 524);
		suite.assertEqual(off_t{*unalignedOffset}, pageSize);

		auto alignedOffset{substrate::make_unique_nothrow<mappingOffset_t>(pageSize, pageSize * 4)};
		suite.assertNotNull(alignedOffset);
		suite.assertEqual(alignedOffset->offset(), pageSize);
		suite.assertEqual(alignedOffset->adjustment(), 0);
		suite.assertEqual(alignedOffset->length(), pageSize * 4);
		suite.assertEqual(alignedOffset->adjustedOffset(), pageSize);
		suite.assertEqual(alignedOffset->adjustedLength(), pageSize * 4);
		suite.assertEqual(off_t{*alignedOffset}, pageSize);
		*alignedOffset += 1024;
		suite.assertEqual(alignedOffset->offset(), pageSize + 1024);
		suite.assertEqual(alignedOffset->adjustment(), 1024);
		suite.assertEqual(alignedOffset->length(), pageSize * 4);
		suite.assertEqual(alignedOffset->adjustedOffset(), pageSize);
		suite.assertEqual(alignedOffset->adjustedLength(), (pageSize * 4) + 1024);
		suite.assertEqual(off_t{*alignedOffset}, pageSize + 1024);
	}
} // namespace mappingOffset
