#include <gtest/gtest.h>
#include <polycpp/winston/winston.hpp>
#include <polycpp/winston/log_info.hpp>

TEST(WinstonTest, Version) {
    EXPECT_EQ(polycpp::winston::version(), "0.1.0");
}

TEST(WinstonTest, LogInfoDefaultConstruction) {
    polycpp::winston::LogInfo info;
    EXPECT_TRUE(info.level.empty());
    EXPECT_TRUE(info.originalLevel.empty());
    EXPECT_TRUE(info.message.empty());
    EXPECT_TRUE(info.formattedMessage.empty());
}
