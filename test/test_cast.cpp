#include <gtest/gtest.h>
#include "crabnum.h"

TEST(Cast, AsRawIntToInt) {
    cn::Num<std::int32_t> a{42};
    EXPECT_EQ(a.as_raw<std::int64_t>(), std::int64_t{42});
    EXPECT_EQ(a.as_raw<std::uint32_t>(), std::uint32_t{42});
    EXPECT_EQ(a.as_raw<std::int8_t>(), std::int8_t{42});
}

TEST(Cast, AsRawIntToFloat) {
    cn::Num<std::int32_t> a{7};
    EXPECT_EQ(a.as_raw<float>(), 7.0f);
    EXPECT_EQ(a.as_raw<double>(), 7.0);
}

TEST(Cast, AsRawFloatToInt) {
    cn::Num<double> a{3.14};
    EXPECT_EQ(a.as_raw<std::int32_t>(), 3);
}

TEST(Cast, AsNumToNum) {
    cn::i32 a{42};
    auto b = a.as<cn::i64>();
    static_assert(std::is_same_v<decltype(b), cn::i64>);
    EXPECT_EQ(b.value(), std::int64_t{42});
}

TEST(Cast, AsNarrow) {
    cn::i32 a{300};
    auto b = a.as<cn::i8>();
    static_assert(std::is_same_v<decltype(b), cn::i8>);
    EXPECT_EQ(b.value(), static_cast<std::int8_t>(300));
}

TEST(Cast, AsIntToFloat) {
    cn::i32 a{7};
    auto b = a.as<cn::f64>();
    static_assert(std::is_same_v<decltype(b), cn::f64>);
    EXPECT_EQ(b.value(), double{7});
}


// --- try_as ---

TEST(Cast, TryAsWideningOk) {
    auto res = cn::i8{42}.try_as<std::int32_t>();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->value(), 42);
}

TEST(Cast, TryAsNarrowingOk) {
    auto res = cn::i32{42}.try_as<std::int8_t>();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->value(), std::int8_t{42});
}

TEST(Cast, TryAsNarrowingFail) {
    auto res = cn::i32{300}.try_as<std::int8_t>();
    EXPECT_FALSE(res.has_value());
}

TEST(Cast, TryAsSignedToUnsignedOk) {
    auto res = cn::i32{42}.try_as<std::uint32_t>();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->value(), std::uint32_t{42});
}

TEST(Cast, TryAsSignedToUnsignedNegative) {
    auto res = cn::i32{-1}.try_as<std::uint32_t>();
    EXPECT_FALSE(res.has_value());
}

TEST(Cast, TryAsUnsignedToSignedOk) {
    auto res = cn::u32{42}.try_as<std::int32_t>();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->value(), 42);
}

TEST(Cast, TryAsUnsignedToSignedFail) {
    auto res = cn::u32{3'000'000'000u}.try_as<std::int32_t>();
    EXPECT_FALSE(res.has_value());
}

TEST(Cast, TryAsIntToFloatOk) {
    auto res = cn::i32{42}.try_as<double>();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->value(), double{42});
}

TEST(Cast, TryAsIntToFloatPrecisionLoss) {
    // large int64 that can't be exactly represented in float64
    auto res = cn::i64{(1LL << 53) + 1}.try_as<double>();
    EXPECT_FALSE(res.has_value());
}

TEST(Cast, TryAsFloatToIntOk) {
    auto res = cn::f64{42.0}.try_as<std::int32_t>();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->value(), 42);
}

TEST(Cast, TryAsFloatToIntFractional) {
    auto res = cn::f64{3.14}.try_as<std::int32_t>();
    EXPECT_FALSE(res.has_value());
}

TEST(Cast, TryAsSameType) {
    auto res = cn::i32{42}.try_as<std::int32_t>();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->value(), 42);
}

// --- saturating_as ---

TEST(Cast, SaturatingAsSameType) {
    EXPECT_EQ(cn::i32{42}.saturating_as<std::int32_t>().value(), 42);
}

TEST(Cast, SaturatingAsWideningOk) {
    EXPECT_EQ(cn::i8{42}.saturating_as<std::int32_t>().value(), 42);
}

TEST(Cast, SaturatingAsNarrowingClampMax) {
    EXPECT_EQ(cn::i32{300}.saturating_as<std::int8_t>().value(), std::int8_t{127});
}

TEST(Cast, SaturatingAsNarrowingClampMin) {
    EXPECT_EQ(cn::i32{-200}.saturating_as<std::int8_t>().value(), std::int8_t{-128});
}

TEST(Cast, SaturatingAsSignedToUnsignedNegative) {
    EXPECT_EQ(cn::i32{-5}.saturating_as<std::uint32_t>().value(), std::uint32_t{0});
}

TEST(Cast, SaturatingAsSignedToUnsignedOk) {
    EXPECT_EQ(cn::i32{42}.saturating_as<std::uint8_t>().value(), std::uint8_t{42});
}

TEST(Cast, SaturatingAsSignedToUnsignedClampMax) {
    EXPECT_EQ(cn::i32{300}.saturating_as<std::uint8_t>().value(), std::uint8_t{255});
}

TEST(Cast, SaturatingAsUnsignedToSignedClamp) {
    EXPECT_EQ(cn::u32{3'000'000'000u}.saturating_as<std::int32_t>().value(), std::numeric_limits<std::int32_t>::max());
}

TEST(Cast, SaturatingAsUnsignedToSignedOk) {
    EXPECT_EQ(cn::u32{42}.saturating_as<std::int32_t>().value(), 42);
}

TEST(Cast, SaturatingAsFloatToIntClampMax) {
    EXPECT_EQ(cn::f64{1e18}.saturating_as<std::int32_t>().value(), std::numeric_limits<std::int32_t>::max());
}

TEST(Cast, SaturatingAsFloatToIntClampMin) {
    EXPECT_EQ(cn::f64{-1e18}.saturating_as<std::int32_t>().value(), std::numeric_limits<std::int32_t>::min());
}

TEST(Cast, SaturatingAsFloatToIntNan) {
    EXPECT_EQ(cn::f64{std::numeric_limits<double>::quiet_NaN()}.saturating_as<std::int32_t>().value(), 0);
}

TEST(Cast, SaturatingAsFloatToIntOk) {
    EXPECT_EQ(cn::f64{42.9}.saturating_as<std::int32_t>().value(), 42);
}

TEST(Cast, SaturatingAsF64ToF32Clamp) {
    EXPECT_EQ(cn::f64{1e300}.saturating_as<float>().value(), std::numeric_limits<float>::max());
}
