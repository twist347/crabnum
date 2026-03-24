#include <gtest/gtest.h>
#include "crabnum.h"

template<typename>
class SignedIntSuite : public ::testing::Test {
};

using SignedIntTypes = ::testing::Types<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;
TYPED_TEST_SUITE(SignedIntSuite, SignedIntTypes);

template<typename>
class UnsignedIntSuite : public ::testing::Test {
};

using UnsignedIntTypes = ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;
TYPED_TEST_SUITE(UnsignedIntSuite, UnsignedIntTypes);

template<typename>
class IntSuite : public ::testing::Test {
};

using IntTypes = ::testing::Types<
    std::int8_t, std::int16_t, std::int32_t, std::int64_t,
    std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;
TYPED_TEST_SUITE(IntSuite, IntTypes);

// --- overflowing_add ---

TYPED_TEST(IntSuite, OverflowingAddNormal) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{3}}.overflowing_add(cn::Num<T>{T{4}});
    EXPECT_EQ(res, cn::Num<T>{T{7}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(UnsignedIntSuite, OverflowingAddOverflow) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>::MAX_VAL().overflowing_add(cn::Num<T>{T{1}});
    EXPECT_EQ(res, cn::Num<T>{T{0}});
    EXPECT_TRUE(ov);
}

TYPED_TEST(SignedIntSuite, OverflowingAddOverflow) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>::MAX_VAL().overflowing_add(cn::Num<T>{T{1}});
    EXPECT_EQ(res, cn::Num<T>::MIN_VAL());
    EXPECT_TRUE(ov);
}

// --- overflowing_sub ---

TYPED_TEST(IntSuite, OverflowingSubNormal) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{10}}.overflowing_sub(cn::Num<T>{T{3}});
    EXPECT_EQ(res, cn::Num<T>{T{7}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(UnsignedIntSuite, OverflowingSubUnderflow) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{0}}.overflowing_sub(cn::Num<T>{T{1}});
    EXPECT_EQ(res, cn::Num<T>::MAX_VAL());
    EXPECT_TRUE(ov);
}

TYPED_TEST(SignedIntSuite, OverflowingSubOverflow) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>::MIN_VAL().overflowing_sub(cn::Num<T>{T{1}});
    EXPECT_EQ(res, cn::Num<T>::MAX_VAL());
    EXPECT_TRUE(ov);
}

// --- overflowing_mul ---

TYPED_TEST(IntSuite, OverflowingMulNormal) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{3}}.overflowing_mul(cn::Num<T>{T{4}});
    EXPECT_EQ(res, cn::Num<T>{T{12}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(UnsignedIntSuite, OverflowingMulOverflow) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>::MAX_VAL().overflowing_mul(cn::Num<T>{T{2}});
    EXPECT_EQ(res, cn::Num<T>::MAX_VAL().wrapping_mul(cn::Num<T>{T{2}}));
    EXPECT_TRUE(ov);
}

TYPED_TEST(SignedIntSuite, OverflowingMulMinByMinusOne) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>::MIN_VAL().overflowing_mul(cn::Num<T>{T{-1}});
    EXPECT_EQ(res, cn::Num<T>::MIN_VAL());
    EXPECT_TRUE(ov);
}

// --- overflowing_div ---

TYPED_TEST(IntSuite, OverflowingDivNormal) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{12}}.overflowing_div(cn::Num<T>{T{4}});
    EXPECT_EQ(res, cn::Num<T>{T{3}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(SignedIntSuite, OverflowingDivMinByMinusOne) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>::MIN_VAL().overflowing_div(cn::Num<T>{T{-1}});
    EXPECT_EQ(res, cn::Num<T>::MIN_VAL());
    EXPECT_TRUE(ov);
}

// --- overflowing_rem ---

TYPED_TEST(IntSuite, OverflowingRemNormal) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{10}}.overflowing_rem(cn::Num<T>{T{3}});
    EXPECT_EQ(res, cn::Num<T>{T{1}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(SignedIntSuite, OverflowingRemMinByMinusOne) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>::MIN_VAL().overflowing_rem(cn::Num<T>{T{-1}});
    EXPECT_EQ(res, cn::Num<T>{T{0}});
    EXPECT_TRUE(ov);
}

// --- overflowing_neg ---

TYPED_TEST(SignedIntSuite, OverflowingNegNormal) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{5}}.overflowing_neg();
    EXPECT_EQ(res, cn::Num<T>{T{-5}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(SignedIntSuite, OverflowingNegMin) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>::MIN_VAL().overflowing_neg();
    EXPECT_EQ(res, cn::Num<T>::MIN_VAL());
    EXPECT_TRUE(ov);
}

TYPED_TEST(UnsignedIntSuite, OverflowingNegZero) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{0}}.overflowing_neg();
    EXPECT_EQ(res, cn::Num<T>{T{0}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(UnsignedIntSuite, OverflowingNegNonZero) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{1}}.overflowing_neg();
    EXPECT_EQ(res, cn::Num<T>::MAX_VAL());
    EXPECT_TRUE(ov);
}

// --- overflowing_abs ---

TYPED_TEST(SignedIntSuite, OverflowingAbsNegative) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{-5}}.overflowing_abs();
    EXPECT_EQ(res, cn::Num<T>{T{5}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(SignedIntSuite, OverflowingAbsMin) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>::MIN_VAL().overflowing_abs();
    EXPECT_EQ(res, cn::Num<T>::MIN_VAL());
    EXPECT_TRUE(ov);
}

// --- overflowing_pow ---

TYPED_TEST(IntSuite, OverflowingPowNormal) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{2}}.overflowing_pow(3);
    EXPECT_EQ(res, cn::Num<T>{T{8}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(IntSuite, OverflowingPowZeroExp) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{5}}.overflowing_pow(0);
    EXPECT_EQ(res, cn::Num<T>{T{1}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(UnsignedIntSuite, OverflowingPowOverflow) {
    using T = TypeParam;
    constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u);
    auto [res, ov] = cn::Num<T>{T{2}}.overflowing_pow(bits);
    EXPECT_EQ(res, cn::Num<T>{T{0}});
    EXPECT_TRUE(ov);
}

// --- overflowing_shl ---

TYPED_TEST(IntSuite, OverflowingShlNormal) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{1}}.overflowing_shl(1);
    EXPECT_EQ(res, cn::Num<T>{T{2}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(IntSuite, OverflowingShlTooMuch) {
    using T = TypeParam;
    constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u);
    auto [res, ov] = cn::Num<T>{T{5}}.overflowing_shl(bits);
    EXPECT_EQ(res, cn::Num<T>{T{5}});
    EXPECT_TRUE(ov);
}

// --- overflowing_shr ---

TYPED_TEST(IntSuite, OverflowingShrNormal) {
    using T = TypeParam;
    auto [res, ov] = cn::Num<T>{T{8}}.overflowing_shr(2);
    EXPECT_EQ(res, cn::Num<T>{T{2}});
    EXPECT_FALSE(ov);
}

TYPED_TEST(IntSuite, OverflowingShrTooMuch) {
    using T = TypeParam;
    constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u);
    auto [res, ov] = cn::Num<T>{T{5}}.overflowing_shr(bits);
    EXPECT_EQ(res, cn::Num<T>{T{5}});
    EXPECT_TRUE(ov);
}
