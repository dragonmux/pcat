#include <thread>
#include <chrono>
#include <string_view>
#include <substrate/console>
#include <substrate/fixed_vector>
#include <substrate/fd>
#include <substrate/pty>
#include <substrate/pipe>
#include <crunch++.h>
#include "testConsole.hxx"

#ifndef _WINDOWS
constexpr static const char *defaultTTY = "/dev/ptmx";
#else
constexpr static const char *defaultTTY = "CON";
#endif

using std::literals::string_view_literals::operator ""sv;
using std::literals::chrono_literals::operator ""us;
using substrate::console;
using substrate::consoleStream_t;
using substrate::asHex_t;
using substrate::operator ""_s;
using substrate::fd_t;
using substrate::pty_t;
using substrate::pipe_t;
using substrate::readPipe_t;
using substrate::fixedVector_t;

struct fileFree_t
{
	void operator()(FILE *const file) const noexcept { fclose(file); } // NOLINT(cppcoreguidelines-owning-memory)
};

constexpr auto testString{"test"sv};
constexpr auto colourDebugTest{" \033[1;34m[DBG]\033[0m test\r\n"sv};
constexpr auto colourInfoTest{" \033[36m[INF]\033[0m test\r\n"sv};
constexpr auto colourWarningTest{" \033[1;33m[WRN]\033[0m test\r\n"sv};
constexpr auto colourErrorTest{" \033[1;31m[ERR]\033[0m test\r\n"sv};

constexpr auto plainDebugTest{" [DBG] test\n"sv};
constexpr auto plainInfoTest{" [INF] test\n"sv};
constexpr auto plainWarningTest{" [WRN] test\n"sv};
constexpr auto plainErrorTest{" [ERR] test\n"sv};

constexpr auto plainTrue{" [INF] true\n"sv};
constexpr auto plainFalse{" [INF] false\n"sv};
constexpr auto plainRawString{" [INF] raw string\n"sv};
constexpr auto plainNullptr{" [INF] (null)\n"sv};
constexpr auto plainChar{" [INF] 1\n"sv};
constexpr auto plainIntegerSigned8Min{" [INF] -128\n"sv};
constexpr auto plainIntegerSigned8{" [INF] -127\n"sv};
constexpr auto plainIntegerSigned8Max{" [INF] 127\n"sv};
constexpr auto plainIntegerUnsigned8{" [INF] 255\n"sv};
constexpr auto plainIntegerUnsigned32{" [INF] 65536\n"sv};
constexpr auto plainIntegerHex{" [INF] F00DFACE\n"sv};
constexpr auto plainIntegerPaddedHex{" [INF] 01A5\n"sv};
constexpr auto plainIntegerZeroHex{" [INF] ___0\n"sv};
constexpr auto plainIntegerOverMaxPaddedHex{" [INF] __F0\n"sv};
constexpr auto plainCharArray{" [INF] testing\n"sv};
constexpr auto plainIntegerArray{" [INF] F00D, DEAD\n"sv};

constexpr std::array<char, 7> testCharArray{'t', 'e', 's', 't', 'i', 'n', 'g'};
constexpr std::array<uint16_t, 2> testIntegerArray{0xF00DU, 0xDEADU};

namespace consoleTests
{
	void testConstruct(testsuite &suite)
	{
		fd_t tty{defaultTTY, O_WRONLY | O_NOCTTY};
		suite.assertTrue(tty.valid());
		suite.assertTrue(isatty(tty));
		consoleStream_t stream{tty};
		suite.assertTrue(stream.valid());
		suite.assertTrue(stream.isTTY());
	}

#ifndef _WINDOWS
	void assertConsoleRead(testsuite &suite, const fd_t &fd, const std::string_view expected)
	{
		fixedVector_t<char> result{expected.length()};
		suite.assertTrue(result.valid());
		std::this_thread::sleep_for(10us);
		suite.assertTrue(fd.read(result.data(), result.size()));
		suite.assertEqual(result.data(), expected.data(), expected.length());
	}

	void testPTYWrite(testsuite &suite)
	{
		pty_t pty{};
		suite.assertTrue(pty.valid());
		// Initialise console_t with a fresh outputStream + errorStream
		// set to our new PTY's "slave" side
		console = {pty.pts(), pty.pts()};
		suite.assertTrue(console.valid());

		console.debug(testString);
		assertConsoleRead(suite, pty.ptmx(), colourDebugTest);
		console.info(testString);
		assertConsoleRead(suite, pty.ptmx(), colourInfoTest);
		console.warning(testString);
		assertConsoleRead(suite, pty.ptmx(), colourWarningTest);
		console.warn(testString);
		assertConsoleRead(suite, pty.ptmx(), colourWarningTest);
		console.error(testString);
		assertConsoleRead(suite, pty.ptmx(), colourErrorTest);

		console = {};
		suite.assertFalse(console.valid());
	}
#endif

	void assertPipeRead(testsuite &suite, const readPipe_t &fd, const std::string_view expected)
	{
		fixedVector_t<char> result{expected.length()};
		suite.assertTrue(result.valid());
		suite.assertTrue(fd.read(result.data(), result.size()));
		suite.assertEqual(result.data(), expected.data(), expected.length());
	}

	void testPipeWrite(testsuite &suite)
	{
		pipe_t pipe{};
		suite.assertTrue(pipe.valid());
		// Initialise console_t with a fresh outputStream + errorStream
		// set to our pipe's write side
		console = {pipe.writeFD(), pipe.writeFD()};
		suite.assertTrue(console.valid());

		console.debug(testString);
		assertPipeRead(suite, pipe, plainDebugTest);
		console.info(testString);
		assertPipeRead(suite, pipe, plainInfoTest);
		console.warning(testString);
		assertPipeRead(suite, pipe, plainWarningTest);
		console.warn(testString);
		assertPipeRead(suite, pipe, plainWarningTest);
		console.error(testString);
		assertPipeRead(suite, pipe, plainErrorTest);

		console = {};
		suite.assertFalse(console.valid());
	}

	void testConversions(testsuite &suite)
	{
		pipe_t pipe{};
		suite.assertTrue(pipe.valid());
		std::unique_ptr<FILE, fileFree_t> file{fdopen(dup(pipe.writeFD()), "wb")};
		suite.assertNotNull(file.get());
		// Initialise console_t with a fresh outputStream + errorStream
		// set to our pipe's write side
		console = {file.get(), file.get()};
		suite.assertTrue(console.valid());

		std::unique_ptr<char> testCharPtr{};
		suite.assertNull(testCharPtr);
		std::unique_ptr<char []> testArrayPtr{};
		suite.assertNull(testArrayPtr);

		console.info(true);
		assertPipeRead(suite, pipe, plainTrue);
		console.info(false);
		assertPipeRead(suite, pipe, plainFalse);
		console.info("raw string");
		assertPipeRead(suite, pipe, plainRawString);
		console.info(testCharPtr);
		assertPipeRead(suite, pipe, plainNullptr);
		console.info(testArrayPtr);
		assertPipeRead(suite, pipe, plainNullptr);
		console.info('1');
		assertPipeRead(suite, pipe, plainChar);
		console.info(int8_t{-127});
		assertPipeRead(suite, pipe, plainIntegerSigned8);
		console.info(int8_t{-128});
		assertPipeRead(suite, pipe, plainIntegerSigned8Min);
		console.info(int8_t{127});
		assertPipeRead(suite, pipe, plainIntegerSigned8Max);
		console.info(uint8_t{255});
		assertPipeRead(suite, pipe, plainIntegerUnsigned8);
		console.info(uint32_t{65536});
		assertPipeRead(suite, pipe, plainIntegerUnsigned32);
		console.info(asHex_t<>{0xF00DFACEU});
		assertPipeRead(suite, pipe, plainIntegerHex);
		console.info(asHex_t<4, '0'>{0x1A5U});
		assertPipeRead(suite, pipe, plainIntegerPaddedHex);
		console.info(asHex_t<4, '_'>{0U});
		assertPipeRead(suite, pipe, plainIntegerZeroHex);
		console.info(asHex_t<4, '_'>{uint8_t{0xF0U}});
		assertPipeRead(suite, pipe, plainIntegerOverMaxPaddedHex);
		console.info(testCharArray);
		assertPipeRead(suite, pipe, plainCharArray);
		console.info(testIntegerArray);
		assertPipeRead(suite, pipe, plainIntegerArray);

		console = {};
		suite.assertFalse(console.valid());
	}
} // namespace consoleTests
