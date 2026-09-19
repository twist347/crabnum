#include <gtest/gtest.h>

#include <unordered_set>
#include <format>
#include <numeric>
#include <vector>
#include <algorithm>

#include "crabnum.h"

TEST(Compat, StdHash) {
    std::unordered_set<cn::i32> s;
    s.insert(cn::i32{1});
    s.insert(cn::i32{2});
    s.insert(cn::i32{1});
    EXPECT_EQ(s.size(), 2u);
}

TEST(Compat, StdFormat) {
    const auto s = std::format("{}", cn::i32{42});
    EXPECT_EQ(s, "42");
}

TEST(Compat, StdFormatFloat) {
    const auto s = std::format("{}", cn::f64{2.5});
    EXPECT_EQ(s, "2.5");
}

TEST(Compat, StdAccumulate) {
    const std::vector<cn::i32> v{cn::i32{1}, cn::i32{2}, cn::i32{3}};
    const auto sum = std::accumulate(v.cbegin(), v.cend(), cn::i32{0});
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

TEST(Compat, NumericLimitsInt) {
    using L = std::numeric_limits<cn::i32>;
    using B = std::numeric_limits<std::int32_t>;
    static_assert(L::is_specialized);
    static_assert(L::is_signed && L::is_integer && L::is_exact && L::is_bounded);
    static_assert(L::digits == B::digits);
    static_assert(L::digits10 == B::digits10);
    static_assert(L::radix == B::radix);
    EXPECT_EQ(L::min(), cn::i32::MIN_VAL());
    EXPECT_EQ(L::lowest(), cn::i32::MIN_VAL());
    EXPECT_EQ(L::max(), cn::i32::MAX_VAL());
}

TEST(Compat, NumericLimitsUnsigned) {
    using L = std::numeric_limits<cn::u8>;
    static_assert(!L::is_signed && L::is_integer && L::is_modulo);
    EXPECT_EQ(L::min(), cn::u8{0});
    EXPECT_EQ(L::max(), cn::u8{255});
}

TEST(Compat, NumericLimitsFloat) {
    using L = std::numeric_limits<cn::f64>;
    using B = std::numeric_limits<double>;
    static_assert(L::is_specialized && L::is_signed && !L::is_integer);
    static_assert(L::is_iec559 == B::is_iec559);
    static_assert(L::has_infinity && L::has_quiet_NaN);
    static_assert(L::max_digits10 == B::max_digits10);
    // MIN_VAL() is Rust's f64::MIN (== lowest()), min() is the smallest positive normal
    EXPECT_EQ(L::lowest(), cn::f64::MIN_VAL());
    EXPECT_EQ(L::min(), cn::f64::MIN_POSITIVE_VAL());
    EXPECT_LT(L::lowest().value(), 0.0);
    EXPECT_GT(L::min().value(), 0.0);
    EXPECT_EQ(L::epsilon(), cn::f64::EPS_VAL());
    EXPECT_EQ(L::infinity(), cn::f64::INF_VAL());
    EXPECT_TRUE(L::quiet_NaN().is_nan());
    EXPECT_EQ(L::denorm_min().value(), B::denorm_min());
}

// Generic code written against numeric_limits now works with Num
namespace {
    template<typename T>
    constexpr auto span_of() -> T {
        return std::numeric_limits<T>::max();
    }
}

TEST(Compat, NumericLimitsIsGenericallyUsable) {
    EXPECT_EQ(span_of<cn::i16>(), cn::i16{32767});
    EXPECT_EQ(span_of<std::int16_t>(), std::int16_t{32767});
}

TEST(Compat, NumConcepts) {
    static_assert(cn::IsNum<cn::i32> && cn::IsNum<cn::f64> && cn::IsNum<cn::u8>);
    static_assert(cn::IsIntNum<cn::i32> && !cn::IsIntNum<cn::f64>);
    static_assert(cn::IsSignedNum<cn::i32> && !cn::IsSignedNum<cn::u32>);
    static_assert(cn::IsUnsignedNum<cn::u32> && !cn::IsUnsignedNum<cn::i32>);
    static_assert(cn::IsFloatNum<cn::f64> && !cn::IsFloatNum<cn::i32>);
    // non-Num types must not satisfy IsNum, including ones that have a value_type
    static_assert(!cn::IsNum<int>);
    static_assert(!cn::IsNum<double>);
    static_assert(!cn::IsNum<std::string>);
    static_assert(!cn::IsNum<std::vector<std::vector<int> > >);
    SUCCEED();
}
