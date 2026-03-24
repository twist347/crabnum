#include <gtest/gtest.h>
#include "crabnum.h"
#include <unordered_set>
#include <format>
#include <numeric>
#include <vector>
#include <algorithm>

TEST(Compat, StdHash) {
    std::unordered_set<cn::i32> s;
    s.insert(cn::i32{1});
    s.insert(cn::i32{2});
    s.insert(cn::i32{1});
    EXPECT_EQ(s.size(), 2u);
}

TEST(Compat, StdFormat) {
    auto s = std::format("{}", cn::i32{42});
    EXPECT_EQ(s, "42");
}

TEST(Compat, StdFormatFloat) {
    auto s = std::format("{}", cn::f64{2.5});
    EXPECT_EQ(s, "2.5");
}

TEST(Compat, StdAccumulate) {
    std::vector<cn::i32> v{cn::i32{1}, cn::i32{2}, cn::i32{3}};
    auto sum = std::accumulate(v.begin(), v.end(), cn::i32{0});
    EXPECT_EQ(sum, cn::i32{6});
}

TEST(Compat, StdSort) {
    std::vector<cn::i32> v{cn::i32{3}, cn::i32{1}, cn::i32{2}};
    std::sort(v.begin(), v.end());
    EXPECT_EQ(v[0], cn::i32{1});
    EXPECT_EQ(v[1], cn::i32{2});
    EXPECT_EQ(v[2], cn::i32{3});
}

TEST(Compat, StdMinMax) {
    EXPECT_EQ(std::min(cn::i32{3}, cn::i32{5}), cn::i32{3});
    EXPECT_EQ(std::max(cn::i32{3}, cn::i32{5}), cn::i32{5});
}
