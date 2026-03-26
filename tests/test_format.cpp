#include <gtest/gtest.h>
#include <polycpp/winston/format.hpp>

using namespace polycpp::winston;
using polycpp::JsonValue;

// A simple test format that adds a metadata key
class AddKeyFormat : public Format {
public:
    AddKeyFormat(const std::string& key, const std::string& value)
        : key_(key), value_(value) {}

    std::optional<LogInfo> transform(LogInfo info) override {
        info.metadata[key_] = JsonValue(value_);
        return info;
    }

private:
    std::string key_;
    std::string value_;
};

// A filter format that drops messages at a certain level
class FilterFormat : public Format {
public:
    explicit FilterFormat(const std::string& dropLevel = "silly")
        : dropLevel_(dropLevel) {}

    std::optional<LogInfo> transform(LogInfo info) override {
        if (info.originalLevel == dropLevel_) {
            return std::nullopt; // Drop the message
        }
        return info;
    }

private:
    std::string dropLevel_;
};

// A finalizing format that sets formattedMessage
class SimpleOutputFormat : public Format {
public:
    std::optional<LogInfo> transform(LogInfo info) override {
        info.formattedMessage = info.level + ": " + info.message;
        return info;
    }
};

// A format that modifies the message
class PrefixFormat : public Format {
public:
    explicit PrefixFormat(const std::string& prefix) : prefix_(prefix) {}

    std::optional<LogInfo> transform(LogInfo info) override {
        info.message = prefix_ + info.message;
        return info;
    }

private:
    std::string prefix_;
};

TEST(FormatTest, SingleFormatTransforms) {
    auto fmt = std::make_shared<AddKeyFormat>("service", "api");

    LogInfo info;
    info.level = "info";
    info.message = "hello";

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->metadata["service"].asString(), "api");
}

TEST(FormatTest, SingleFormatFilter) {
    auto fmt = std::make_shared<FilterFormat>("debug");

    LogInfo info("debug", "should drop");
    auto result = fmt->transform(info);
    EXPECT_FALSE(result.has_value());
}

TEST(FormatTest, SingleFormatPassesThrough) {
    auto fmt = std::make_shared<FilterFormat>("debug");

    LogInfo info("info", "should pass");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "should pass");
}

TEST(FormatTest, CombineChainFormats) {
    auto fmt = combine({
        std::make_shared<AddKeyFormat>("service", "api"),
        std::make_shared<AddKeyFormat>("hostname", "localhost"),
        std::make_shared<SimpleOutputFormat>()
    });

    LogInfo info;
    info.level = "info";
    info.message = "hello";

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->metadata["service"].asString(), "api");
    EXPECT_EQ(result->metadata["hostname"].asString(), "localhost");
    EXPECT_EQ(result->formattedMessage, "info: hello");
}

TEST(FormatTest, CombineShortCircuitsOnFilter) {
    // If a format returns nullopt, the chain stops
    auto fmt = combine({
        std::make_shared<FilterFormat>(),       // Drops "silly" messages
        std::make_shared<SimpleOutputFormat>()   // Should not be reached
    });

    LogInfo info;
    info.level = "silly";
    info.originalLevel = "silly";
    info.message = "should be dropped";

    auto result = fmt->transform(info);
    EXPECT_FALSE(result.has_value());
}

TEST(FormatTest, CombinePassesNonFilteredMessages) {
    auto fmt = combine({
        std::make_shared<FilterFormat>(),
        std::make_shared<SimpleOutputFormat>()
    });

    LogInfo info;
    info.level = "info";
    info.originalLevel = "info";
    info.message = "hello";

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->formattedMessage, "info: hello");
}

TEST(FormatTest, CombineEmptyList) {
    auto fmt = combine({});

    LogInfo info;
    info.level = "info";
    info.message = "pass through";

    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "pass through");
}

TEST(FormatTest, CombinePreservesOrderOfApplication) {
    // prefix1 then prefix2 should result in prefix2 + prefix1 + message
    auto fmt = combine({
        std::make_shared<PrefixFormat>("[1]"),
        std::make_shared<PrefixFormat>("[2]")
    });

    LogInfo info("info", "hello");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->message, "[2][1]hello");
}

TEST(FormatTest, CombineFilterMidChain) {
    // Format1 (adds key) -> Filter (drops error) -> Format2 (should not run)
    auto fmt = combine({
        std::make_shared<AddKeyFormat>("before", "yes"),
        std::make_shared<FilterFormat>("error"),
        std::make_shared<AddKeyFormat>("after", "no")
    });

    LogInfo info("error", "boom");
    auto result = fmt->transform(info);
    EXPECT_FALSE(result.has_value());
}

TEST(FormatTest, CombineFilterMidChainPassThrough) {
    // Same pipeline but with a non-filtered message
    auto fmt = combine({
        std::make_shared<AddKeyFormat>("before", "yes"),
        std::make_shared<FilterFormat>("error"),
        std::make_shared<AddKeyFormat>("after", "yes")
    });

    LogInfo info("info", "ok");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->metadata["before"].asString(), "yes");
    EXPECT_EQ(result->metadata["after"].asString(), "yes");
}

TEST(FormatTest, SingleFormat) {
    // Test that a single format within combine works
    auto fmt = combine({
        std::make_shared<SimpleOutputFormat>()
    });

    LogInfo info("warn", "low disk");
    auto result = fmt->transform(info);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->formattedMessage, "warn: low disk");
}
