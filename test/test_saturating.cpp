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

// --- saturating_add ---

TYPED_TEST(IntSuite, SaturatingAddNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{3}}.saturating_add(cn::Num<T>{T{4}}), cn::Num<T>{T{7}});
}

TYPED_TEST(UnsignedIntSuite, SaturatingAddOverflow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().saturating_add(cn::Num<T>{T{1}}), cn::Num<T>::MAX_VAL());
}

TYPED_TEST(SignedIntSuite, SaturatingAddOverflowPositive) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().saturating_add(cn::Num<T>{T{1}}), cn::Num<T>::MAX_VAL());
}

TYPED_TEST(SignedIntSuite, SaturatingAddOverflowNegative) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().saturating_add(cn::Num<T>{T{-1}}), cn::Num<T>::MIN_VAL());
}

// --- saturating_sub ---

TYPED_TEST(IntSuite, SaturatingSubNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{10}}.saturating_sub(cn::Num<T>{T{3}}), cn::Num<T>{T{7}});
}

TYPED_TEST(UnsignedIntSuite, SaturatingSubUnderflow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0}}.saturating_sub(cn::Num<T>{T{1}}), cn::Num<T>::MIN_VAL());
}

TYPED_TEST(SignedIntSuite, SaturatingSubOverflow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().saturating_sub(cn::Num<T>{T{1}}), cn::Num<T>::MIN_VAL());
}

TYPED_TEST(SignedIntSuite, SaturatingSubOverflowNegativeRhs) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().saturating_sub(cn::Num<T>{T{-1}}), cn::Num<T>::MAX_VAL());
}

// --- saturating_mul ---

TYPED_TEST(IntSuite, SaturatingMulNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{3}}.saturating_mul(cn::Num<T>{T{4}}), cn::Num<T>{T{12}});
}

TYPED_TEST(UnsignedIntSuite, SaturatingMulOverflow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().saturating_mul(cn::Num<T>{T{2}}), cn::Num<T>::MAX_VAL());
}

TYPED_TEST(SignedIntSuite, SaturatingMulOverflowPositive) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().saturating_mul(cn::Num<T>{T{2}}), cn::Num<T>::MAX_VAL());
}

TYPED_TEST(SignedIntSuite, SaturatingMulOverflowNegative) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().saturating_mul(cn::Num<T>{T{-2}}), cn::Num<T>::MIN_VAL());
}

TYPED_TEST(SignedIntSuite, SaturatingMulMinByMinusOne) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().saturating_mul(cn::Num<T>{T{-1}}), cn::Num<T>::MAX_VAL());
}

// --- saturating_div ---

TYPED_TEST(IntSuite, SaturatingDivNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{12}}.saturating_div(cn::Num<T>{T{4}}), cn::Num<T>{T{3}});
}

TYPED_TEST(SignedIntSuite, SaturatingDivMinByMinusOne) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().saturating_div(cn::Num<T>{T{-1}}), cn::Num<T>::MAX_VAL());
}

// --- saturating_neg ---

TYPED_TEST(SignedIntSuite, SaturatingNegNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5}}.saturating_neg(), cn::Num<T>{T{-5}});
}

TYPED_TEST(SignedIntSuite, SaturatingNegMin) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().saturating_neg(), cn::Num<T>::MAX_VAL());
}

// --- saturating_abs ---

TYPED_TEST(SignedIntSuite, SaturatingAbsNegative) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{-5}}.saturating_abs(), cn::Num<T>{T{5}});
}

TYPED_TEST(SignedIntSuite, SaturatingAbsMin) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().saturating_abs(), cn::Num<T>::MAX_VAL());
}

// --- saturating_pow ---

TYPED_TEST(IntSuite, SaturatingPowNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{2}}.saturating_pow(3), cn::Num<T>{T{8}});
}

TYPED_TEST(IntSuite, SaturatingPowZeroExp) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5}}.saturating_pow(0), cn::Num<T>{T{1}});
}

TYPED_TEST(UnsignedIntSuite, SaturatingPowOverflow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().saturating_pow(2), cn::Num<T>::MAX_VAL());
}

TYPED_TEST(SignedIntSuite, SaturatingPowOverflowPositive) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().saturating_pow(2), cn::Num<T>::MAX_VAL());
}

TYPED_TEST(SignedIntSuite, SaturatingPowNegativeBaseEvenExp) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().saturating_pow(2), cn::Num<T>::MAX_VAL());
}

TYPED_TEST(SignedIntSuite, SaturatingPowNegativeBaseOddExp) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().saturating_pow(3), cn::Num<T>::MIN_VAL());
}
