#include <substrate/utility>
#include <indexSequence.hxx>
#include "testIndexSequence.hxx"

constexpr static std::size_t operator ""_uz(const unsigned long long value) noexcept { return value; }

using pcat::indexSequence_t;
using pcat::indexIterator_t;

namespace indexSequence
{
	void testIteration(testsuite &suite)
	{
		const auto emptySequence = substrate::make_unique_nothrow<indexSequence_t>(0_uz, 0_uz);
		suite.assertNotNull(emptySequence);
		suite.assertTrue(emptySequence->begin() == emptySequence->end());
	}

	void testIterator(testsuite &suite)
	{
		//
	}
}
