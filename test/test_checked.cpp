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

// --- checked_add ---

TYPED_TEST(IntSuite, CheckedAddNormal) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{3}}.checked_add(cn::Num<T>{T{4}});
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{7}});
}

TYPED_TEST(UnsignedIntSuite, CheckedAddOverflow) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MAX_VAL().checked_add(cn::Num<T>{T{1}});
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(SignedIntSuite, CheckedAddOverflowPositive) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MAX_VAL().checked_add(cn::Num<T>{T{1}});
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(SignedIntSuite, CheckedAddOverflowNegative) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MIN_VAL().checked_add(cn::Num<T>{T{-1}});
    EXPECT_FALSE(res.has_value());
}

// --- checked_sub ---

TYPED_TEST(IntSuite, CheckedSubNormal) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{10}}.checked_sub(cn::Num<T>{T{3}});
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{7}});
}

TYPED_TEST(UnsignedIntSuite, CheckedSubUnderflow) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{0}}.checked_sub(cn::Num<T>{T{1}});
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(SignedIntSuite, CheckedSubOverflow) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MIN_VAL().checked_sub(cn::Num<T>{T{1}});
    EXPECT_FALSE(res.has_value());
}

// --- checked_mul ---

TYPED_TEST(IntSuite, CheckedMulNormal) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{3}}.checked_mul(cn::Num<T>{T{4}});
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{12}});
}

TYPED_TEST(IntSuite, CheckedMulByZero) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MAX_VAL().checked_mul(cn::Num<T>{T{0}});
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{0}});
}

TYPED_TEST(UnsignedIntSuite, CheckedMulOverflow) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MAX_VAL().checked_mul(cn::Num<T>{T{2}});
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(SignedIntSuite, CheckedMulOverflow) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MAX_VAL().checked_mul(cn::Num<T>{T{2}});
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(SignedIntSuite, CheckedMulMinByMinusOne) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MIN_VAL().checked_mul(cn::Num<T>{T{-1}});
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(SignedIntSuite, CheckedMulMinusOneByMin) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{-1}}.checked_mul(cn::Num<T>::MIN_VAL());
    EXPECT_FALSE(res.has_value());
}

// --- checked_div ---

TYPED_TEST(IntSuite, CheckedDivNormal) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{12}}.checked_div(cn::Num<T>{T{4}});
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{3}});
}

TYPED_TEST(IntSuite, CheckedDivByZero) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{5}}.checked_div(cn::Num<T>{T{0}});
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(SignedIntSuite, CheckedDivMinByMinusOne) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MIN_VAL().checked_div(cn::Num<T>{T{-1}});
    EXPECT_FALSE(res.has_value());
}

// --- checked_rem ---

TYPED_TEST(IntSuite, CheckedRemNormal) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{10}}.checked_rem(cn::Num<T>{T{3}});
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{1}});
}

TYPED_TEST(IntSuite, CheckedRemByZero) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{5}}.checked_rem(cn::Num<T>{T{0}});
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(SignedIntSuite, CheckedRemMinByMinusOne) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MIN_VAL().checked_rem(cn::Num<T>{T{-1}});
    EXPECT_FALSE(res.has_value());
}

// --- checked_neg ---

TYPED_TEST(SignedIntSuite, CheckedNegNormal) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{5}}.checked_neg();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{-5}});
}

TYPED_TEST(SignedIntSuite, CheckedNegZero) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{0}}.checked_neg();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{0}});
}

TYPED_TEST(SignedIntSuite, CheckedNegMin) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MIN_VAL().checked_neg();
    EXPECT_FALSE(res.has_value());
}

// --- checked_abs ---

TYPED_TEST(SignedIntSuite, CheckedAbsPositive) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{5}}.checked_abs();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{5}});
}

TYPED_TEST(SignedIntSuite, CheckedAbsNegative) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{-5}}.checked_abs();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{5}});
}

TYPED_TEST(SignedIntSuite, CheckedAbsMin) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MIN_VAL().checked_abs();
    EXPECT_FALSE(res.has_value());
}

// --- checked_pow ---

TYPED_TEST(IntSuite, CheckedPowNormal) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{2}}.checked_pow(3);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{8}});
}

TYPED_TEST(IntSuite, CheckedPowZeroExp) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{5}}.checked_pow(0);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{1}});
}

TYPED_TEST(IntSuite, CheckedPowZeroBaseZeroExp) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{0}}.checked_pow(0);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{1}});
}

TYPED_TEST(IntSuite, CheckedPowOverflow) {
    using T = TypeParam;
    const auto res = cn::Num<T>::MAX_VAL().checked_pow(2);
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(UnsignedIntSuite, CheckedPowBaseTwo) {
    using T = TypeParam;
    constexpr auto bits = sizeof(T) * 8u;
    const auto res = cn::Num<T>{T{2}}.checked_pow(bits - 1);
    ASSERT_TRUE(res.has_value());

    const auto overflow = cn::Num<T>{T{2}}.checked_pow(bits);
    EXPECT_FALSE(overflow.has_value());
}

// --- checked_shl ---

TYPED_TEST(IntSuite, CheckedShlNormal) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{1}}.checked_shl(1);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{2}});
}

TYPED_TEST(IntSuite, CheckedShlZeroShift) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{5}}.checked_shl(0);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{5}});
}

TYPED_TEST(IntSuite, CheckedShlTooMuch) {
    using T = TypeParam;
    constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u);
    const auto res = cn::Num<T>{T{1}}.checked_shl(bits);
    EXPECT_FALSE(res.has_value());
}

// --- checked_shr ---

TYPED_TEST(IntSuite, CheckedShrNormal) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{8}}.checked_shr(2);
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{2}});
}

TYPED_TEST(IntSuite, CheckedShrTooMuch) {
    using T = TypeParam;
    constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u);
    const auto res = cn::Num<T>{T{1}}.checked_shr(bits);
    EXPECT_FALSE(res.has_value());
}
