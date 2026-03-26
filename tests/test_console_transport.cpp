#include <gtest/gtest.h>
#include <polycpp/winston/console_transport.hpp>
#include <polycpp/winston/level_config.hpp>
#include <polycpp/os.hpp>
#include <any>
#include <sstream>
#include <string>
#include <vector>

using namespace polycpp::winston;

// ---------------------------------------------------------------------------
// Default behavior (all to stdout)
// ---------------------------------------------------------------------------

TEST(ConsoleTransportTest, DefaultWritesToStdout) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    ConsoleTransport transport(opts, out, err);

    LogInfo info("info", "hello world");
    transport.write(info);

    EXPECT_EQ(out.str(), "hello world" + polycpp::os::EOL);
    EXPECT_TRUE(err.str().empty());
}

TEST(ConsoleTransportTest, AllLevelsToStdoutByDefault) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    ConsoleTransport transport(opts, out, err);

    transport.write(LogInfo("error", "e"));
    transport.write(LogInfo("warn", "w"));
    transport.write(LogInfo("info", "i"));
    transport.write(LogInfo("debug", "d"));

    std::string expected =
        "e" + polycpp::os::EOL +
        "w" + polycpp::os::EOL +
        "i" + polycpp::os::EOL +
        "d" + polycpp::os::EOL;
    EXPECT_EQ(out.str(), expected);
    EXPECT_TRUE(err.str().empty());
}

// ---------------------------------------------------------------------------
// stderr routing via stderrLevels
// ---------------------------------------------------------------------------

TEST(ConsoleTransportTest, StderrLevelsRoutesToStderr) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    opts.stderrLevels = {"error"};
    ConsoleTransport transport(opts, out, err);

    transport.write(LogInfo("error", "bad"));
    transport.write(LogInfo("info", "ok"));

    EXPECT_EQ(err.str(), "bad" + polycpp::os::EOL);
    EXPECT_EQ(out.str(), "ok" + polycpp::os::EOL);
}

TEST(ConsoleTransportTest, MultipleStderrLevels) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    opts.stderrLevels = {"error", "warn"};
    ConsoleTransport transport(opts, out, err);

    transport.write(LogInfo("error", "e"));
    transport.write(LogInfo("warn", "w"));
    transport.write(LogInfo("info", "i"));

    std::string expectedErr =
        "e" + polycpp::os::EOL +
        "w" + polycpp::os::EOL;
    EXPECT_EQ(err.str(), expectedErr);
    EXPECT_EQ(out.str(), "i" + polycpp::os::EOL);
}

// ---------------------------------------------------------------------------
// stderr routing via consoleWarnLevels
// ---------------------------------------------------------------------------

TEST(ConsoleTransportTest, ConsoleWarnLevelsRoutesToStderr) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    opts.consoleWarnLevels = {"warn"};
    ConsoleTransport transport(opts, out, err);

    transport.write(LogInfo("warn", "warning"));
    transport.write(LogInfo("info", "normal"));

    EXPECT_EQ(err.str(), "warning" + polycpp::os::EOL);
    EXPECT_EQ(out.str(), "normal" + polycpp::os::EOL);
}

// ---------------------------------------------------------------------------
// EOL handling
// ---------------------------------------------------------------------------

TEST(ConsoleTransportTest, DefaultEol) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    ConsoleTransport transport(opts, out, err);

    transport.write(LogInfo("info", "msg"));

    // Output should end with the platform EOL
    std::string result = out.str();
    ASSERT_FALSE(result.empty());
    EXPECT_EQ(result, "msg" + polycpp::os::EOL);
}

TEST(ConsoleTransportTest, CustomEol) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    opts.eol = "\r\n";
    ConsoleTransport transport(opts, out, err);

    transport.write(LogInfo("info", "msg"));

    EXPECT_EQ(out.str(), "msg\r\n");
}

TEST(ConsoleTransportTest, ExplicitEolUsed) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    opts.eol = "---";
    ConsoleTransport transport(opts, out, err);

    transport.write(LogInfo("info", "test"));

    EXPECT_EQ(out.str(), "test---");
}

// ---------------------------------------------------------------------------
// formattedMessage vs message
// ---------------------------------------------------------------------------

TEST(ConsoleTransportTest, UsesFormattedMessage) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    ConsoleTransport transport(opts, out, err);

    LogInfo info("info", "raw message");
    info.formattedMessage = "formatted output";
    transport.write(info);

    EXPECT_EQ(out.str(), "formatted output" + polycpp::os::EOL);
}

TEST(ConsoleTransportTest, FallbackToMessage) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    ConsoleTransport transport(opts, out, err);

    LogInfo info("info", "raw message");
    // formattedMessage is empty by default
    transport.write(info);

    EXPECT_EQ(out.str(), "raw message" + polycpp::os::EOL);
}

// ---------------------------------------------------------------------------
// Transport-level filtering
// ---------------------------------------------------------------------------

TEST(ConsoleTransportTest, TransportLevelFiltering) {
    auto config = LevelConfig::npm();

    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    opts.level = "warn";
    ConsoleTransport transport(opts, out, err);
    transport.setLevels(&config);

    // info (priority 2) is below warn (priority 1) -> filtered
    transport.write(LogInfo("info", "should not appear"));
    EXPECT_TRUE(out.str().empty());

    // error (priority 0) is above warn -> passes
    transport.write(LogInfo("error", "should appear"));
    EXPECT_EQ(out.str(), "should appear" + polycpp::os::EOL);
}

// ---------------------------------------------------------------------------
// Silent mode
// ---------------------------------------------------------------------------

TEST(ConsoleTransportTest, SilentSuppressesOutput) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    opts.silent = true;
    ConsoleTransport transport(opts, out, err);

    transport.write(LogInfo("info", "hidden"));

    EXPECT_TRUE(out.str().empty());
    EXPECT_TRUE(err.str().empty());
}

// ---------------------------------------------------------------------------
// Event emission
// ---------------------------------------------------------------------------

TEST(ConsoleTransportTest, EmitsLoggedEvent) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    ConsoleTransport transport(opts, out, err);

    int loggedCount = 0;
    transport.on("logged", [&loggedCount](const std::vector<std::any>&) {
        loggedCount++;
    });

    transport.write(LogInfo("info", "first"));
    EXPECT_EQ(loggedCount, 1);

    transport.write(LogInfo("info", "second"));
    EXPECT_EQ(loggedCount, 2);
}

// ---------------------------------------------------------------------------
// Multiple messages
// ---------------------------------------------------------------------------

TEST(ConsoleTransportTest, MultipleMessages) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    ConsoleTransport transport(opts, out, err);

    transport.write(LogInfo("info", "one"));
    transport.write(LogInfo("info", "two"));
    transport.write(LogInfo("info", "three"));

    std::string eol = polycpp::os::EOL;
    std::string expected = "one" + eol + "two" + eol + "three" + eol;
    EXPECT_EQ(out.str(), expected);
}

// ---------------------------------------------------------------------------
// originalLevel used for routing (not the possibly-colorized level)
// ---------------------------------------------------------------------------

TEST(ConsoleTransportTest, RoutingUsesOriginalLevel) {
    std::ostringstream out, err;
    ConsoleTransportOptions opts;
    opts.stderrLevels = {"error"};
    ConsoleTransport transport(opts, out, err);

    LogInfo info("error", "boom");
    // Simulate colorization mutating the level field
    info.level = "\033[31merror\033[39m";
    // originalLevel remains "error"
    transport.write(info);

    EXPECT_EQ(err.str(), "boom" + polycpp::os::EOL);
    EXPECT_TRUE(out.str().empty());
}
