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

// --- wrapping_add ---

TYPED_TEST(IntSuite, WrappingAddNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{3}}.wrapping_add(cn::Num<T>{T{4}}), cn::Num<T>{T{7}});
}

TYPED_TEST(UnsignedIntSuite, WrappingAddOverflow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().wrapping_add(cn::Num<T>{T{1}}), cn::Num<T>{T{0}});
}

TYPED_TEST(SignedIntSuite, WrappingAddOverflow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().wrapping_add(cn::Num<T>{T{1}}), cn::Num<T>::MIN_VAL());
}

TYPED_TEST(SignedIntSuite, WrappingAddUnderflow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().wrapping_add(cn::Num<T>{T{-1}}), cn::Num<T>::MAX_VAL());
}

// --- wrapping_sub ---

TYPED_TEST(IntSuite, WrappingSubNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{10}}.wrapping_sub(cn::Num<T>{T{3}}), cn::Num<T>{T{7}});
}

TYPED_TEST(UnsignedIntSuite, WrappingSubUnderflow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0}}.wrapping_sub(cn::Num<T>{T{1}}), cn::Num<T>::MAX_VAL());
}

TYPED_TEST(SignedIntSuite, WrappingSubOverflow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().wrapping_sub(cn::Num<T>{T{1}}), cn::Num<T>::MAX_VAL());
}

// --- wrapping_mul ---

TYPED_TEST(IntSuite, WrappingMulNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{3}}.wrapping_mul(cn::Num<T>{T{4}}), cn::Num<T>{T{12}});
}

TYPED_TEST(UnsignedIntSuite, WrappingMulOverflow) {
    using T = TypeParam;
    // MAX * 2 wraps: MAX*2 = (2^N - 1)*2 = 2^(N+1) - 2 ≡ -2 mod 2^N ≡ MAX - 1
    auto res = cn::Num<T>::MAX_VAL().wrapping_mul(cn::Num<T>{T{2}});
    using U = std::make_unsigned_t<T>;
    auto expected = static_cast<T>(static_cast<U>(std::numeric_limits<T>::max()) * U{2});
    EXPECT_EQ(res, cn::Num<T>{expected});
}

TYPED_TEST(SignedIntSuite, WrappingMulMinByMinusOne) {
    using T = TypeParam;
    // MIN * -1 wraps back to MIN
    EXPECT_EQ(cn::Num<T>::MIN_VAL().wrapping_mul(cn::Num<T>{T{-1}}), cn::Num<T>::MIN_VAL());
}

// --- wrapping_div ---

TYPED_TEST(IntSuite, WrappingDivNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{12}}.wrapping_div(cn::Num<T>{T{4}}), cn::Num<T>{T{3}});
}

TYPED_TEST(SignedIntSuite, WrappingDivMinByMinusOne) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().wrapping_div(cn::Num<T>{T{-1}}), cn::Num<T>::MIN_VAL());
}

// --- wrapping_rem ---

TYPED_TEST(IntSuite, WrappingRemNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{10}}.wrapping_rem(cn::Num<T>{T{3}}), cn::Num<T>{T{1}});
}

TYPED_TEST(SignedIntSuite, WrappingRemMinByMinusOne) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MIN_VAL().wrapping_rem(cn::Num<T>{T{-1}}), cn::Num<T>{T{0}});
}

// --- wrapping_neg ---

TYPED_TEST(SignedIntSuite, WrappingNegNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5}}.wrapping_neg(), cn::Num<T>{T{-5}});
}

TYPED_TEST(SignedIntSuite, WrappingNegMin) {
    using T = TypeParam;
    // -MIN wraps back to MIN
    EXPECT_EQ(cn::Num<T>::MIN_VAL().wrapping_neg(), cn::Num<T>::MIN_VAL());
}

TYPED_TEST(UnsignedIntSuite, WrappingNegZero) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0}}.wrapping_neg(), cn::Num<T>{T{0}});
}

TYPED_TEST(UnsignedIntSuite, WrappingNegOne) {
    using T = TypeParam;
    // 0 - 1 = MAX
    EXPECT_EQ(cn::Num<T>{T{1}}.wrapping_neg(), cn::Num<T>::MAX_VAL());
}

// --- wrapping_abs ---

TYPED_TEST(SignedIntSuite, WrappingAbsNegative) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{-5}}.wrapping_abs(), cn::Num<T>{T{5}});
}

TYPED_TEST(SignedIntSuite, WrappingAbsMin) {
    using T = TypeParam;
    // |MIN| can't fit → wraps back to MIN
    EXPECT_EQ(cn::Num<T>::MIN_VAL().wrapping_abs(), cn::Num<T>::MIN_VAL());
}

// --- wrapping_pow ---

TYPED_TEST(IntSuite, WrappingPowNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{2}}.wrapping_pow(3), cn::Num<T>{T{8}});
}

TYPED_TEST(IntSuite, WrappingPowZeroExp) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5}}.wrapping_pow(0), cn::Num<T>{T{1}});
}

TYPED_TEST(UnsignedIntSuite, WrappingPowOverflow) {
    using T = TypeParam;
    // 2^N wraps to 0 for N-bit unsigned
    constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u);
    EXPECT_EQ(cn::Num<T>{T{2}}.wrapping_pow(bits), cn::Num<T>{T{0}});
}

// --- wrapping_shl ---

TYPED_TEST(IntSuite, WrappingShlNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{1}}.wrapping_shl(1), cn::Num<T>{T{2}});
}

TYPED_TEST(IntSuite, WrappingShlModulo) {
    using T = TypeParam;
    constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u);
    // shift by bit-width wraps to shift by 0
    EXPECT_EQ(cn::Num<T>{T{5}}.wrapping_shl(bits), cn::Num<T>{T{5}});
}

// --- wrapping_shr ---

TYPED_TEST(IntSuite, WrappingShrNormal) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{8}}.wrapping_shr(2), cn::Num<T>{T{2}});
}

TYPED_TEST(IntSuite, WrappingShrModulo) {
    using T = TypeParam;
    constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u);
    EXPECT_EQ(cn::Num<T>{T{5}}.wrapping_shr(bits), cn::Num<T>{T{5}});
}
