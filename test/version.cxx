#include "testPcat.hxx"
#include <version.hxx>

namespace pcat
{
	using namespace pcat::versionInfo;

	void testVersion(testsuite &suite)
	{
		suite.assertEqual(printVersion(), 0);
		suite.assertGreaterThan(version.size(), 0);
		suite.assertGreaterThan(compiler.size(), 0);
		suite.assertGreaterThan(compilerVersion.size(), 0);
		suite.assertGreaterThan(system.size(), 0);
		suite.assertGreaterThan(arch.size(), 0);
	}
}
