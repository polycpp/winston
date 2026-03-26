#include <gtest/gtest.h>
#include <polycpp/winston/colorizer.hpp>
#include <polycpp/winston/level_config.hpp>

using namespace polycpp::winston;

class ColorizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Register npm colors for each test
        auto config = LevelConfig::npm();
        Colorizer::addColors(config.colors);
    }
};

TEST_F(ColorizerTest, ColorizeAppliesRegisteredColor) {
    auto result = Colorizer::colorize("error", "something went wrong");
    // The result should contain ANSI codes (red)
    EXPECT_NE(result, "something went wrong"); // Not unchanged
    EXPECT_NE(result.find("\033["), std::string::npos); // Contains escape codes
}

TEST_F(ColorizerTest, ColorizeUnknownLevelReturnsUnchanged) {
    auto result = Colorizer::colorize("nonexistent", "hello");
    EXPECT_EQ(result, "hello");
}

TEST_F(ColorizerTest, StripRemovesAnsiCodes) {
    auto colored = Colorizer::colorize("error", "hello");
    auto stripped = Colorizer::strip(colored);
    EXPECT_EQ(stripped, "hello");
}

TEST_F(ColorizerTest, StripOnPlainStringIsNoop) {
    EXPECT_EQ(Colorizer::strip("hello"), "hello");
}

TEST_F(ColorizerTest, StripOnEmptyStringIsNoop) {
    EXPECT_EQ(Colorizer::strip(""), "");
}

TEST_F(ColorizerTest, AddColorsOverwritesExisting) {
    Colorizer::addColors({{"error", "blue"}}); // Override red -> blue
    auto result = Colorizer::colorize("error", "test");
    auto stripped = Colorizer::strip(result);
    EXPECT_EQ(stripped, "test");
    // Verify the color was changed
    auto colors = Colorizer::allColors();
    EXPECT_EQ(colors["error"], "blue");
    // Restore original
    Colorizer::addColors({{"error", "red"}});
}

TEST_F(ColorizerTest, AllColorsReturnsCopy) {
    auto colors = Colorizer::allColors();
    EXPECT_FALSE(colors.empty());
    EXPECT_EQ(colors["error"], "red");
    EXPECT_EQ(colors["warn"], "yellow");
    EXPECT_EQ(colors["info"], "green");
}

TEST_F(ColorizerTest, ColorizeRoundTrips) {
    // colorize -> strip should return the original string
    std::string original = "test message 123";
    auto colored = Colorizer::colorize("info", original);
    auto stripped = Colorizer::strip(colored);
    EXPECT_EQ(stripped, original);
}

TEST_F(ColorizerTest, ColorizeRoundTripsAllLevels) {
    // Test round-trip for all npm levels
    std::vector<std::string> levels = {"error", "warn", "info", "http", "verbose", "debug", "silly"};
    std::string original = "test message";
    for (const auto& level : levels) {
        auto colored = Colorizer::colorize(level, original);
        auto stripped = Colorizer::strip(colored);
        EXPECT_EQ(stripped, original) << "Round-trip failed for level: " << level;
    }
}

TEST_F(ColorizerTest, ColorizeEmptyString) {
    auto result = Colorizer::colorize("error", "");
    auto stripped = Colorizer::strip(result);
    EXPECT_EQ(stripped, "");
}

TEST_F(ColorizerTest, AddColorsMergesNew) {
    Colorizer::addColors({{"custom_level", "cyan"}});
    auto result = Colorizer::colorize("custom_level", "hello");
    EXPECT_NE(result, "hello"); // Should be colorized
    auto stripped = Colorizer::strip(result);
    EXPECT_EQ(stripped, "hello");
}

TEST_F(ColorizerTest, AllColorsIsCopy) {
    // Modifying the returned map should not affect the registry
    auto colors = Colorizer::allColors();
    colors["error"] = "green";
    // The registry should still have "red"
    auto colors2 = Colorizer::allColors();
    EXPECT_EQ(colors2["error"], "red");
}

TEST_F(ColorizerTest, ColorizeDifferentLevelsDifferentOutput) {
    auto errorColored = Colorizer::colorize("error", "test");
    auto infoColored = Colorizer::colorize("info", "test");
    // Different levels produce different ANSI codes
    EXPECT_NE(errorColored, infoColored);
    // But both strip to the same original
    EXPECT_EQ(Colorizer::strip(errorColored), Colorizer::strip(infoColored));
}
