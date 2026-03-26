#include <gtest/gtest.h>
#include <polycpp/winston/formats/timestamp.hpp>
#include <polycpp/winston/formats/label.hpp>
#include <polycpp/winston/formats/ms.hpp>
#include <polycpp/winston/formats/errors.hpp>
#include <polycpp/winston/formats/metadata.hpp>
#include <polycpp/winston/formats/colorize.hpp>
#include <polycpp/winston/formats/splat.hpp>
#include <polycpp/winston/formats/align.hpp>
#include <polycpp/winston/formats/pad_levels.hpp>
#include <polycpp/winston/formats/uncolorize.hpp>
#include <polycpp/winston/formats/json.hpp>
#include <polycpp/winston/formats/simple.hpp>
#include <polycpp/winston/formats/printf.hpp>
#include <polycpp/winston/formats/logstash.hpp>
#include <polycpp/winston/formats/pretty_print.hpp>
#include <polycpp/winston/formats/cli.hpp>
#include <polycpp/winston/colorizer.hpp>
#include <polycpp/winston/level_config.hpp>
#include <polycpp/core/json.hpp>

using namespace polycpp::winston;
using namespace polycpp::winston::formats;
using polycpp::JsonValue;
using polycpp::JsonObject;
namespace JSON = polycpp::JSON;

// Helper to make a basic LogInfo
LogInfo makeInfo(const std::string& lvl, const std::string& msg) {
    LogInfo info;
    info.level = lvl;
    info.originalLevel = lvl;
    info.message = msg;
    return info;
}

// ============================================================
// Timestamp tests
// ============================================================

TEST(TimestampFormatTest, DefaultAddsIsoTimestamp) {
    auto fmt = timestamp();
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->metadata.count("timestamp"));
    // ISO timestamp looks like "2026-03-26T..."
    auto ts = result->metadata["timestamp"].asString();
    EXPECT_FALSE(ts.empty());
    EXPECT_NE(ts.find("T"), std::string::npos); // Contains 'T' separator
}

TEST(TimestampFormatTest, CustomFunction) {
    auto fmt = timestamp({.fn = []{ return "custom-time"; }});
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->metadata["timestamp"].asString(), "custom-time");
}

TEST(TimestampFormatTest, CustomAlias) {
    auto fmt = timestamp({.alias = "time"});
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->metadata.count("time"));
    EXPECT_FALSE(result->metadata.count("timestamp"));
}

// ============================================================
// Label tests
// ============================================================

TEST(LabelFormatTest, AddsLabelToMetadata) {
    auto fmt = label({.label = "my-app"});
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->metadata["label"].asString(), "my-app");
}

TEST(LabelFormatTest, MessageModePrependsToMessage) {
    auto fmt = label({.label = "my-app", .message = true});
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "[my-app] hello");
    EXPECT_FALSE(result->metadata.count("label")); // Not in metadata
}

// ============================================================
// Ms tests
// ============================================================

TEST(MsFormatTest, AddsElapsedTime) {
    auto fmt = ms();
    auto info1 = makeInfo("info", "first");
    auto result1 = fmt->transform(info1);
    ASSERT_TRUE(result1.has_value());
    EXPECT_TRUE(result1->metadata.count("ms"));
    auto msStr = result1->metadata["ms"].asString();
    EXPECT_EQ(msStr[0], '+'); // Starts with '+'
}

TEST(MsFormatTest, SecondCallShowsElapsed) {
    auto fmt = ms();
    auto info1 = makeInfo("info", "first");
    fmt->transform(info1);

    // Second call should show some elapsed time
    auto info2 = makeInfo("info", "second");
    auto result2 = fmt->transform(info2);
    ASSERT_TRUE(result2.has_value());
    auto msStr = result2->metadata["ms"].asString();
    EXPECT_EQ(msStr[0], '+');
}

// ============================================================
// Errors tests
// ============================================================

TEST(ErrorsFormatTest, ExtractsErrorFields) {
    auto fmt = errors({.stack = true});
    auto info = makeInfo("error", "");

    // Simulate an error stored in metadata
    JsonObject errorObj;
    errorObj["message"] = JsonValue("something broke");
    errorObj["stack"] = JsonValue("Error: something broke\n    at main.cpp:42");
    errorObj["name"] = JsonValue("Error");
    info.metadata["error"] = JsonValue(errorObj);

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Error message should be extracted to info.message if message was empty
    EXPECT_EQ(result->message, "something broke");
    EXPECT_TRUE(result->metadata.count("stack"));
}

TEST(ErrorsFormatTest, NoStackWhenDisabled) {
    auto fmt = errors({.stack = false});
    auto info = makeInfo("error", "");

    JsonObject errorObj;
    errorObj["message"] = JsonValue("broke");
    errorObj["stack"] = JsonValue("Error: broke\n    at ...");
    info.metadata["error"] = JsonValue(errorObj);

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->metadata.count("stack"));
}

TEST(ErrorsFormatTest, DoesNotOverwriteExistingMessage) {
    auto fmt = errors({.stack = true});
    auto info = makeInfo("error", "existing message");

    JsonObject errorObj;
    errorObj["message"] = JsonValue("error message");
    errorObj["stack"] = JsonValue("Error: error message\n    at ...");
    info.metadata["error"] = JsonValue(errorObj);

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Should NOT overwrite existing message
    EXPECT_EQ(result->message, "existing message");
    EXPECT_TRUE(result->metadata.count("stack"));
}

TEST(ErrorsFormatTest, NoErrorKeyPassesThrough) {
    auto fmt = errors();
    auto info = makeInfo("info", "no error");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "no error");
}

// ============================================================
// Metadata tests
// ============================================================

TEST(MetadataFormatTest, NestsAllMetadata) {
    auto fmt = metadata();
    auto info = makeInfo("info", "hello");
    info.metadata["service"] = JsonValue("api");
    info.metadata["requestId"] = JsonValue("abc123");

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // All metadata should be nested under "metadata" key
    EXPECT_TRUE(result->metadata.count("metadata"));
    auto nested = result->metadata["metadata"].asObject();
    EXPECT_EQ(nested["service"].asString(), "api");
    EXPECT_EQ(nested["requestId"].asString(), "abc123");
}

TEST(MetadataFormatTest, FillExceptKeepsSpecifiedKeys) {
    auto fmt = metadata({.fillExcept = {"service"}});
    auto info = makeInfo("info", "hello");
    info.metadata["service"] = JsonValue("api");
    info.metadata["requestId"] = JsonValue("abc123");

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // "service" should stay at top level
    EXPECT_EQ(result->metadata["service"].asString(), "api");
    // "requestId" should be nested
    auto nested = result->metadata["metadata"].asObject();
    EXPECT_EQ(nested["requestId"].asString(), "abc123");
}

TEST(MetadataFormatTest, FillWithMovesOnlySpecifiedKeys) {
    auto fmt = metadata({.fillWith = {"requestId"}});
    auto info = makeInfo("info", "hello");
    info.metadata["service"] = JsonValue("api");
    info.metadata["requestId"] = JsonValue("abc123");

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // "service" stays at top level
    EXPECT_EQ(result->metadata["service"].asString(), "api");
    // Only "requestId" is nested
    auto nested = result->metadata["metadata"].asObject();
    EXPECT_EQ(nested["requestId"].asString(), "abc123");
    EXPECT_FALSE(nested.count("service"));
}

TEST(MetadataFormatTest, EmptyMetadataNoNestedKey) {
    auto fmt = metadata();
    auto info = makeInfo("info", "hello");

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // No nested key when metadata is empty
    EXPECT_FALSE(result->metadata.count("metadata"));
}

// ============================================================
// Colorize tests
// ============================================================

class ColorizeFormatTest : public ::testing::Test {
protected:
    void SetUp() override {
        Colorizer::addColors(LevelConfig::npm().colors);
    }
};

TEST_F(ColorizeFormatTest, ColorizesLevel) {
    auto fmt = colorize();
    auto info = makeInfo("error", "something failed");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Level should contain ANSI codes
    EXPECT_NE(result->level, "error");
    EXPECT_NE(result->level.find("\033["), std::string::npos);
    // Message should be unchanged
    EXPECT_EQ(result->message, "something failed");
}

TEST_F(ColorizeFormatTest, ColorizeAll) {
    auto fmt = colorize({.all = true});
    auto info = makeInfo("error", "something failed");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Both level and message should contain ANSI codes
    EXPECT_NE(result->level.find("\033["), std::string::npos);
    EXPECT_NE(result->message.find("\033["), std::string::npos);
}

TEST_F(ColorizeFormatTest, ColorizeMessageOnly) {
    auto fmt = colorize({.level = false, .message = true});
    auto info = makeInfo("error", "something failed");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Level unchanged (no ANSI), message colorized
    EXPECT_EQ(result->level, "error");
    EXPECT_NE(result->message.find("\033["), std::string::npos);
}

// ============================================================
// Splat tests
// ============================================================

TEST(SplatFormatTest, InterpolatesPercentS) {
    auto fmt = splat();
    auto info = makeInfo("info", "hello %s");
    info.splat.push_back(JsonValue("world"));

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "hello world");
}

TEST(SplatFormatTest, InterpolatesPercentD) {
    auto fmt = splat();
    auto info = makeInfo("info", "count: %d");
    info.splat.push_back(JsonValue(42.0));

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "count: 42");
}

TEST(SplatFormatTest, InterpolatesPercentJ) {
    auto fmt = splat();
    auto info = makeInfo("info", "data: %j");
    JsonObject obj;
    obj["key"] = JsonValue("value");
    info.splat.push_back(JsonValue(obj));

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Should contain JSON representation
    EXPECT_NE(result->message.find("\"key\""), std::string::npos);
    EXPECT_NE(result->message.find("\"value\""), std::string::npos);
}

TEST(SplatFormatTest, NoSplatArgsPassesThrough) {
    auto fmt = splat();
    auto info = makeInfo("info", "hello world");
    // No splat args

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "hello world");
}

TEST(SplatFormatTest, ExtraObjectArgsMergedToMetadata) {
    auto fmt = splat();
    auto info = makeInfo("info", "hello %s");
    info.splat.push_back(JsonValue("world"));
    // Extra object arg after all specifiers consumed
    JsonObject extra;
    extra["requestId"] = JsonValue("abc123");
    info.splat.push_back(JsonValue(extra));

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "hello world");
    EXPECT_EQ(result->metadata["requestId"].asString(), "abc123");
}

TEST(SplatFormatTest, EscapedPercent) {
    auto fmt = splat();
    auto info = makeInfo("info", "100%% done");

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "100% done");
}

TEST(SplatFormatTest, InterpolatesPercentF) {
    auto fmt = splat();
    auto info = makeInfo("info", "pi: %f");
    info.splat.push_back(JsonValue(3.14));

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result->message.find("3.14"), std::string::npos);
}

TEST(SplatFormatTest, MultipleSpecifiers) {
    auto fmt = splat();
    auto info = makeInfo("info", "%s has %d items");
    info.splat.push_back(JsonValue("list"));
    info.splat.push_back(JsonValue(5.0));

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "list has 5 items");
}

// ============================================================
// Align tests
// ============================================================

TEST(AlignFormatTest, PrependsTab) {
    auto fmt = align();
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "\thello");
}

TEST(AlignFormatTest, EmptyMessage) {
    auto fmt = align();
    auto info = makeInfo("info", "");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "\t");
}

// ============================================================
// PadLevels tests
// ============================================================

TEST(PadLevelsFormatTest, PadsMessageBasedOnLevelLength) {
    auto fmt = padLevels({.levels = LevelConfig::npm().levels});
    // "verbose" is the longest npm level (7 chars)
    // "info" is 4 chars, so padding = 7 - 4 + 1 = 4 spaces

    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Message should be padded with leading spaces
    EXPECT_NE(result->message, "hello");
    EXPECT_TRUE(result->message.find("hello") != std::string::npos);
    // First chars should be spaces
    EXPECT_EQ(result->message[0], ' ');
}

TEST(PadLevelsFormatTest, LongestLevelGetsMinPadding) {
    auto fmt = padLevels({.levels = LevelConfig::npm().levels});
    auto info = makeInfo("verbose", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // "verbose" is the longest level, should get just 1 space padding
    EXPECT_EQ(result->message[0], ' ');
    EXPECT_EQ(result->message, " hello");
}

// ============================================================
// Uncolorize tests
// ============================================================

TEST(UncolorizeFormatTest, StripsAnsiFromLevelAndMessage) {
    // First colorize, then uncolorize
    Colorizer::addColors(LevelConfig::npm().colors);
    auto info = makeInfo("error", "something failed");
    info.level = Colorizer::colorize("error", info.level);
    info.message = Colorizer::colorize("error", info.message);

    auto fmt = uncolorize();
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->level, "error");
    EXPECT_EQ(result->message, "something failed");
}

TEST(UncolorizeFormatTest, RawStripsFormattedMessage) {
    auto info = makeInfo("info", "hello");
    info.formattedMessage = "\033[31mred text\033[39m";

    auto fmt = uncolorize({.raw = true});
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->formattedMessage, "red text");
}

TEST(UncolorizeFormatTest, PlainTextUnchanged) {
    auto info = makeInfo("info", "plain text");
    auto fmt = uncolorize();
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->level, "info");
    EXPECT_EQ(result->message, "plain text");
}

// ============================================================
// JSON format tests
// ============================================================

TEST(JsonFormatTest, SerializesToJson) {
    auto fmt = json();
    auto info = makeInfo("info", "hello");
    info.metadata["service"] = JsonValue("api");

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->formattedMessage.empty());

    // Parse back to verify
    auto parsed = JSON::parse(result->formattedMessage);
    auto obj = parsed.asObject();
    EXPECT_EQ(obj["level"].asString(), "info");
    EXPECT_EQ(obj["message"].asString(), "hello");
    EXPECT_EQ(obj["service"].asString(), "api");
}

TEST(JsonFormatTest, IndentedOutput) {
    auto fmt = json({.space = 2});
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Indented JSON should contain newlines
    EXPECT_NE(result->formattedMessage.find('\n'), std::string::npos);
}

TEST(JsonFormatTest, CompactOutput) {
    auto fmt = json({.space = 0});
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Compact JSON should not contain newlines
    EXPECT_EQ(result->formattedMessage.find('\n'), std::string::npos);
}

// ============================================================
// Simple format tests
// ============================================================

TEST(SimpleFormatTest, BasicOutput) {
    auto fmt = simple();
    auto info = makeInfo("info", "hello world");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->formattedMessage, "info: hello world");
}

TEST(SimpleFormatTest, WithMetadata) {
    auto fmt = simple();
    auto info = makeInfo("error", "disk full");
    info.metadata["code"] = JsonValue(507.0);

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->formattedMessage.find("error: disk full") == 0);
    // Should contain JSON-serialized metadata
    EXPECT_NE(result->formattedMessage.find("507"), std::string::npos);
}

TEST(SimpleFormatTest, EmptyMetadata) {
    auto fmt = simple();
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // No trailing JSON when metadata is empty
    EXPECT_EQ(result->formattedMessage, "info: hello");
}

// ============================================================
// Printf format tests
// ============================================================

TEST(PrintfFormatTest, CustomTemplate) {
    auto fmt = printf([](const LogInfo& info) {
        return "[" + info.level + "] " + info.message;
    });
    auto info = makeInfo("warn", "check this");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->formattedMessage, "[warn] check this");
}

TEST(PrintfFormatTest, AccessMetadata) {
    auto fmt = printf([](const LogInfo& info) {
        std::string svc;
        auto it = info.metadata.find("service");
        if (it != info.metadata.end()) {
            svc = it->second.asString();
        }
        return info.level + " [" + svc + "]: " + info.message;
    });
    auto info = makeInfo("info", "started");
    info.metadata["service"] = JsonValue("api");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->formattedMessage, "info [api]: started");
}

// ============================================================
// Logstash format tests
// ============================================================

TEST(LogstashFormatTest, ProducesLogstashJson) {
    auto fmt = logstash();
    auto info = makeInfo("info", "hello world");

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());

    auto parsed = JSON::parse(result->formattedMessage);
    auto obj = parsed.asObject();
    EXPECT_EQ(obj["@message"].asString(), "hello world");
    EXPECT_EQ(obj["level"].asString(), "info");
    EXPECT_TRUE(obj.count("@timestamp"));
    EXPECT_TRUE(obj.count("@fields"));
}

TEST(LogstashFormatTest, IncludesMetadataInFields) {
    auto fmt = logstash();
    auto info = makeInfo("error", "failed");
    info.metadata["service"] = JsonValue("api");

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());

    auto parsed = JSON::parse(result->formattedMessage);
    auto fields = parsed.asObject()["@fields"].asObject();
    EXPECT_EQ(fields["service"].asString(), "api");
}

TEST(LogstashFormatTest, UsesExistingTimestamp) {
    auto fmt = logstash();
    auto info = makeInfo("info", "hello");
    info.metadata["timestamp"] = JsonValue("2026-01-01T00:00:00.000Z");

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());

    auto parsed = JSON::parse(result->formattedMessage);
    EXPECT_EQ(parsed.asObject()["@timestamp"].asString(), "2026-01-01T00:00:00.000Z");
}

// ============================================================
// PrettyPrint format tests
// ============================================================

TEST(PrettyPrintFormatTest, ProducesInspectOutput) {
    auto fmt = prettyPrint();
    auto info = makeInfo("info", "hello");
    info.metadata["service"] = JsonValue("api");

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result->formattedMessage.empty());
    // Should contain object-like output (not JSON -- inspect uses different formatting)
}

TEST(PrettyPrintFormatTest, RespectsDepth) {
    auto fmt = prettyPrint({.depth = 0});
    auto info = makeInfo("info", "hello");
    JsonObject nested;
    nested["inner"] = JsonValue("value");
    info.metadata["outer"] = JsonValue(nested);

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // At depth 0, nested objects should be collapsed
    EXPECT_FALSE(result->formattedMessage.empty());
}

// ============================================================
// CLI format tests
// ============================================================

class CliFormatTest : public ::testing::Test {
protected:
    void SetUp() override {
        Colorizer::addColors(LevelConfig::npm().colors);
    }
};

TEST_F(CliFormatTest, ColorizesAndPads) {
    auto fmt = cli({.levels = LevelConfig::npm().levels});
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Level should be colorized (contain ANSI codes)
    EXPECT_NE(result->level.find("\033["), std::string::npos);
    // Message should be padded (leading space(s))
    EXPECT_EQ(result->message[0], ' ');
}

TEST_F(CliFormatTest, LongestLevelStillPadded) {
    auto fmt = cli({.levels = LevelConfig::npm().levels});
    auto info = makeInfo("verbose", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result->level.find("\033["), std::string::npos);
    // Even longest level gets 1 space
    EXPECT_EQ(result->message[0], ' ');
}

// ============================================================
// Combined pipeline tests (integration)
// ============================================================

TEST(FormatPipelineTest, TimestampThenJsonPipeline) {
    auto fmt = combine({
        timestamp(),
        json()
    });
    auto info = makeInfo("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());

    auto parsed = JSON::parse(result->formattedMessage);
    auto obj = parsed.asObject();
    EXPECT_EQ(obj["level"].asString(), "info");
    EXPECT_EQ(obj["message"].asString(), "hello");
    EXPECT_TRUE(obj.count("timestamp"));
}

TEST(FormatPipelineTest, LabelTimestampSimplePipeline) {
    auto fmt = combine({
        label({.label = "my-app"}),
        timestamp({.fn = []{ return "2026-03-26T12:00:00.000Z"; }}),
        simple()
    });
    auto info = makeInfo("info", "server started");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // Simple format includes metadata, so label and timestamp should appear
    EXPECT_TRUE(result->formattedMessage.find("info: server started") == 0);
    EXPECT_NE(result->formattedMessage.find("my-app"), std::string::npos);
}

TEST(FormatPipelineTest, ColorizeThenUncolorizePipeline) {
    Colorizer::addColors(LevelConfig::npm().colors);
    auto fmt = combine({
        colorize({.all = true}),
        uncolorize()
    });
    auto info = makeInfo("error", "boom");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    // After colorize then uncolorize, should be plain
    EXPECT_EQ(result->level, "error");
    EXPECT_EQ(result->message, "boom");
}

TEST(FormatPipelineTest, SplatThenSimplePipeline) {
    auto fmt = combine({
        splat(),
        simple()
    });
    LogInfo info("info", "hello %s");
    info.splat.push_back(JsonValue("world"));
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->formattedMessage, "info: hello world");
}

TEST(FormatPipelineTest, ErrorsThenJsonPipeline) {
    auto fmt = combine({
        errors({.stack = true}),
        json()
    });
    auto info = makeInfo("error", "");
    JsonObject errorObj;
    errorObj["message"] = JsonValue("crash");
    errorObj["stack"] = JsonValue("Error: crash\n    at main:1");
    info.metadata["error"] = JsonValue(errorObj);

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());

    auto parsed = JSON::parse(result->formattedMessage);
    auto obj = parsed.asObject();
    EXPECT_EQ(obj["message"].asString(), "crash");
    EXPECT_TRUE(obj.count("stack"));
}
