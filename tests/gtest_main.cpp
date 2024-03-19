#include "gtest/gtest.h"
#include <vector>

#include "../src/Reactor.h"

int main(int argc, char **argv) {
    les::Reactor reactor;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(MyTest, one) {
    auto vec = std::vector<int>();
    vec.push_back(1);
    ASSERT_EQ(vec[0], 1);
}