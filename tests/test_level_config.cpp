#include <gtest/gtest.h>
#include <polycpp/winston/level_config.hpp>

using namespace polycpp::winston;

// --- npm preset tests ---

TEST(LevelConfigTest, NpmLevelsCorrectPriorities) {
    auto config = LevelConfig::npm();
    EXPECT_EQ(config.levels["error"], 0);
    EXPECT_EQ(config.levels["warn"], 1);
    EXPECT_EQ(config.levels["info"], 2);
    EXPECT_EQ(config.levels["http"], 3);
    EXPECT_EQ(config.levels["verbose"], 4);
    EXPECT_EQ(config.levels["debug"], 5);
    EXPECT_EQ(config.levels["silly"], 6);
    EXPECT_EQ(config.levels.size(), 7u);
}

TEST(LevelConfigTest, NpmLevelsHaveColors) {
    auto config = LevelConfig::npm();
    EXPECT_EQ(config.colors["error"], "red");
    EXPECT_EQ(config.colors["warn"], "yellow");
    EXPECT_EQ(config.colors["info"], "green");
    EXPECT_EQ(config.colors["http"], "green");
    EXPECT_EQ(config.colors["verbose"], "cyan");
    EXPECT_EQ(config.colors["debug"], "blue");
    EXPECT_EQ(config.colors["silly"], "magenta");
    EXPECT_EQ(config.colors.size(), 7u);
}

// --- syslog preset tests ---

TEST(LevelConfigTest, SyslogLevelsCorrectPriorities) {
    auto config = LevelConfig::syslog();
    EXPECT_EQ(config.levels["emerg"], 0);
    EXPECT_EQ(config.levels["alert"], 1);
    EXPECT_EQ(config.levels["crit"], 2);
    EXPECT_EQ(config.levels["error"], 3);
    EXPECT_EQ(config.levels["warning"], 4);
    EXPECT_EQ(config.levels["notice"], 5);
    EXPECT_EQ(config.levels["info"], 6);
    EXPECT_EQ(config.levels["debug"], 7);
    EXPECT_EQ(config.levels.size(), 8u);
}

TEST(LevelConfigTest, SyslogLevelsHaveColors) {
    auto config = LevelConfig::syslog();
    EXPECT_EQ(config.colors["emerg"], "red");
    EXPECT_EQ(config.colors["alert"], "yellow");
    EXPECT_EQ(config.colors["crit"], "red");
    EXPECT_EQ(config.colors["error"], "red");
    EXPECT_EQ(config.colors["warning"], "red");
    EXPECT_EQ(config.colors["notice"], "yellow");
    EXPECT_EQ(config.colors["info"], "green");
    EXPECT_EQ(config.colors["debug"], "blue");
    EXPECT_EQ(config.colors.size(), 8u);
}

// --- cli preset tests ---

TEST(LevelConfigTest, CliLevelsCorrectPriorities) {
    auto config = LevelConfig::cli();
    EXPECT_EQ(config.levels["error"], 0);
    EXPECT_EQ(config.levels["warn"], 1);
    EXPECT_EQ(config.levels["help"], 2);
    EXPECT_EQ(config.levels["data"], 3);
    EXPECT_EQ(config.levels["info"], 4);
    EXPECT_EQ(config.levels["debug"], 5);
    EXPECT_EQ(config.levels["prompt"], 6);
    EXPECT_EQ(config.levels["verbose"], 7);
    EXPECT_EQ(config.levels["input"], 8);
    EXPECT_EQ(config.levels["silly"], 9);
    EXPECT_EQ(config.levels.size(), 10u);
}

TEST(LevelConfigTest, CliLevelsHaveColors) {
    auto config = LevelConfig::cli();
    EXPECT_EQ(config.colors["error"], "red");
    EXPECT_EQ(config.colors["warn"], "yellow");
    EXPECT_EQ(config.colors["help"], "cyan");
    EXPECT_EQ(config.colors["data"], "grey");
    EXPECT_EQ(config.colors["info"], "green");
    EXPECT_EQ(config.colors["debug"], "blue");
    EXPECT_EQ(config.colors["prompt"], "grey");
    EXPECT_EQ(config.colors["verbose"], "cyan");
    EXPECT_EQ(config.colors["input"], "grey");
    EXPECT_EQ(config.colors["silly"], "magenta");
    EXPECT_EQ(config.colors.size(), 10u);
}

// --- isLevelEnabled tests ---

TEST(LevelConfigTest, NpmLevelFilteringSemantics) {
    // Verify the filtering rule: levels[configured] >= levels[message] means pass.
    // At level "info" (2): error(0), warn(1), info(2) pass; verbose(4) does not.
    auto config = LevelConfig::npm();

    EXPECT_TRUE(config.isLevelEnabled("info", "error"));   // 2 >= 0 -> pass
    EXPECT_TRUE(config.isLevelEnabled("info", "warn"));    // 2 >= 1 -> pass
    EXPECT_TRUE(config.isLevelEnabled("info", "info"));    // 2 >= 2 -> pass
    EXPECT_FALSE(config.isLevelEnabled("info", "verbose")); // 2 < 4 -> filter
    EXPECT_FALSE(config.isLevelEnabled("info", "debug"));  // 2 < 5 -> filter
    EXPECT_FALSE(config.isLevelEnabled("info", "silly"));  // 2 < 6 -> filter
}

TEST(LevelConfigTest, IsLevelEnabledErrorOnlyAcceptsError) {
    auto config = LevelConfig::npm();
    EXPECT_TRUE(config.isLevelEnabled("error", "error"));
    EXPECT_FALSE(config.isLevelEnabled("error", "warn"));
    EXPECT_FALSE(config.isLevelEnabled("error", "info"));
}

TEST(LevelConfigTest, IsLevelEnabledSillyAcceptsAll) {
    auto config = LevelConfig::npm();
    EXPECT_TRUE(config.isLevelEnabled("silly", "error"));
    EXPECT_TRUE(config.isLevelEnabled("silly", "warn"));
    EXPECT_TRUE(config.isLevelEnabled("silly", "info"));
    EXPECT_TRUE(config.isLevelEnabled("silly", "verbose"));
    EXPECT_TRUE(config.isLevelEnabled("silly", "debug"));
    EXPECT_TRUE(config.isLevelEnabled("silly", "silly"));
}

TEST(LevelConfigTest, IsLevelEnabledUnknownLevelReturnsFalse) {
    auto config = LevelConfig::npm();
    EXPECT_FALSE(config.isLevelEnabled("info", "unknown"));
    EXPECT_FALSE(config.isLevelEnabled("unknown", "info"));
    EXPECT_FALSE(config.isLevelEnabled("unknown", "unknown"));
}

TEST(LevelConfigTest, IsLevelEnabledSyslog) {
    auto config = LevelConfig::syslog();
    EXPECT_TRUE(config.isLevelEnabled("warning", "emerg"));    // 4 >= 0
    EXPECT_TRUE(config.isLevelEnabled("warning", "error"));    // 4 >= 3
    EXPECT_TRUE(config.isLevelEnabled("warning", "warning"));  // 4 >= 4
    EXPECT_FALSE(config.isLevelEnabled("warning", "notice"));  // 4 < 5
    EXPECT_FALSE(config.isLevelEnabled("warning", "info"));    // 4 < 6
}

// --- maxLevel tests ---

TEST(LevelConfigTest, MaxLevelNpm) {
    auto config = LevelConfig::npm();
    EXPECT_EQ(config.maxLevel(), 6); // silly = 6
}

TEST(LevelConfigTest, MaxLevelSyslog) {
    auto config = LevelConfig::syslog();
    EXPECT_EQ(config.maxLevel(), 7); // debug = 7
}

TEST(LevelConfigTest, MaxLevelCli) {
    auto config = LevelConfig::cli();
    EXPECT_EQ(config.maxLevel(), 9); // silly = 9
}

TEST(LevelConfigTest, MaxLevelEmpty) {
    LevelConfig config;
    EXPECT_EQ(config.maxLevel(), -1);
}

TEST(LevelConfigTest, MaxLevelCustom) {
    LevelConfig config;
    config.levels["a"] = 3;
    config.levels["b"] = 1;
    config.levels["c"] = 5;
    EXPECT_EQ(config.maxLevel(), 5);
}
