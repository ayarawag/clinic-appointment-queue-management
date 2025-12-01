#include <gtest/gtest.h>

TEST(BasicTest, FirstCheck) {
    int x = 5;
    int y = 5;
    EXPECT_EQ(x, y);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}