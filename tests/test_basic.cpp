#include "gtest/gtest.h"

TEST(BasicTest, FirstCheck) {
    int x = 5;
    int y = 5;
    EXPECT_EQ(x, y);
}