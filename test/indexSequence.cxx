#include <substrate/utility>
#include <indexSequence.hxx>
#include "testIndexSequence.hxx"

constexpr static std::size_t operator ""_uz(const unsigned long long value) noexcept { return value; }

using pcat::indexIterator_t;
using pcat::indexSequence_t;

namespace indexSequence
{
	void testIterator(testsuite &suite)
	{
		const auto iteratorPtr{substrate::make_unique_nothrow<indexIterator_t>(0_uz)};
		suite.assertNotNull(iteratorPtr);

		const auto &iterator{*iteratorPtr};
		suite.assertEqual(*iterator, 0_uz);
		suite.assertTrue(iterator == indexIterator_t{0_uz});
		suite.assertFalse(iterator != indexIterator_t{0_uz});
		suite.assertFalse(iterator == indexIterator_t{1_uz});
		suite.assertTrue(iterator != indexIterator_t{1_uz});

		suite.assertTrue(++*iteratorPtr == iterator);
		suite.assertFalse(iterator == indexIterator_t{0_uz});
		suite.assertTrue(iterator != indexIterator_t{0_uz});
		suite.assertTrue(iterator == indexIterator_t{1_uz});
		suite.assertFalse(iterator != indexIterator_t{1_uz});
	}

	void testIteration(testsuite &suite)
	{
		const auto emptySequence{substrate::make_unique_nothrow<indexSequence_t>(0_uz, 0_uz)};
		suite.assertNotNull(emptySequence);
		suite.assertTrue(emptySequence->begin() == emptySequence->end());
	}
}
