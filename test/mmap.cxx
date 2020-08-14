#include <substrate/utility>
#include <mmap.hxx>
#include "testMMap.hxx"

using pcat::mmap_t;
#ifdef _WINDOWS
using pcat::PROT_READ;
using pcat::PROT_WRITE;
using pcat::MAP_PRIVATE;

using pcat::MADV_SEQUENTIAL;
const auto MADV_RANDOM{0};
using pcat::MADV_WILLNEED;
const auto MADV_DONTNEED{0};
using pcat::MADV_DONTDUMP;
const auto MADV_REMOVE{0};
#endif

constexpr static auto testString{u8"おはようございます"sv};

namespace memoryMap
{
	using pcat::off_t;

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
		suite.assertTrue(*map == mmap_t{});
	}

	void testMapEntireFile(testsuite &suite, const fd_t &fd, const random_t random)
	{
		suite.assertGreaterThan(fd.length(), 0);
		mmap_t map{fd, fd.length(), PROT_READ};
		suite.assertTrue(map.valid());
		suite.assertEqual(map.length(), fd.length());
		suite.assertNotNull(map.address(0));
		suite.assertTrue(map.advise(MADV_DONTNEED));
		suite.assertTrue(map.advise<MADV_SEQUENTIAL>());
		suite.assertTrue(map.advise<MADV_RANDOM, MADV_WILLNEED>());
		suite.assertFalse(map.advise<MADV_REMOVE>());
		suite.assertTrue(map.sync());

		random_t result{};
		map.copyFrom(4, result);
		suite.assertEqual(result, random);

		std::array<char, 4> magicString{};
		map.copyFrom(0, magicString.data(), magicString.size());
		suite.assertEqual(std::string_view{magicString.data(), magicString.size()}, testStringMmap);

		auto defaultMap{substrate::make_unique_nothrow<mmap_t>()};
		suite.assertNotNull(defaultMap);
		suite.assertTrue(map != *defaultMap);
	}

	void testMapUpper(testsuite &suite, const fd_t &fd, const random_t random)
	{
		mmap_t map{fd, pcat::pageSize, off_t(sizeof(random_t)), PROT_READ};
		suite.assertTrue(map.valid());
		suite.assertEqual(map.length(), sizeof(random_t));

		random_t result{};
		map.copyFrom(0, result);
		suite.assertEqual(result, random);
	}

	void testMapWriteLower(testsuite &suite, const fd_t &fd, const random_t random)
	{
		mmap_t map{fd, pcat::pageSize, PROT_WRITE};
		suite.assertTrue(map.valid());
		suite.assertEqual(map.length(), pcat::pageSize);
		map.copyTo(4, random);
		map.copyTo(1024, testString.data(), testString.size());
	}

	void testMapReadLower(testsuite &suite, const fd_t &fd, const random_t random)
	{
		const mmap_t map{fd, off_t{}, pcat::pageSize, PROT_READ, MAP_PRIVATE};
		suite.assertTrue(map.valid());
		suite.assertEqual(map.length(), pcat::pageSize);
		suite.assertNotNull(map.address(0));

		random_t result{};
		map.copyFrom(4, result);
		suite.assertEqual(result, random);

		std::array<char, testString.size()> testData{};
		map.copyFrom(1024, testData.data(), testData.size());
		static_assert(testData.size() == testString.size());
		suite.assertEqual(testData.data(), testString.data(), testString.size());
	}

	void testMapPartials(testsuite &suite, const fd_t &fd, const random_t random)
	{
		suite.assertEqual(fd.length(), pcat::pageSize + sizeof(random_t));
		testMapUpper(suite, fd, random);
		testMapWriteLower(suite, fd, random);
		testMapReadLower(suite, fd, random);
	}
} // namespace memoryMap
