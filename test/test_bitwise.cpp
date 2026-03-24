#include <gtest/gtest.h>
#include "crabnum.h"

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

// --- operators ---

TYPED_TEST(IntSuite, BitwiseAnd) {
    using T = TypeParam;
    cn::Num<T> a{T{0b1100}}, b{T{0b1010}};
    EXPECT_EQ((a & b), cn::Num<T>{T{0b1000}});
}

TYPED_TEST(IntSuite, BitwiseOr) {
    using T = TypeParam;
    cn::Num<T> a{T{0b1100}}, b{T{0b1010}};
    EXPECT_EQ((a | b), cn::Num<T>{T{0b1110}});
}

TYPED_TEST(IntSuite, BitwiseXor) {
    using T = TypeParam;
    cn::Num<T> a{T{0b1100}}, b{T{0b1010}};
    EXPECT_EQ((a ^ b), cn::Num<T>{T{0b0110}});
}

TYPED_TEST(IntSuite, BitwiseNot) {
    using T = TypeParam;
    cn::Num<T> a{T{0}};
    EXPECT_EQ((~a), cn::Num<T>{static_cast<T>(~T{0})});
}

TYPED_TEST(IntSuite, ShiftLeft) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{1}} << 3u, cn::Num<T>{T{8}});
}

TYPED_TEST(IntSuite, ShiftRight) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{8}} >> 2u, cn::Num<T>{T{2}});
}

// --- compound assignment ---

TYPED_TEST(IntSuite, BitwiseAndAssign) {
    using T = TypeParam;
    cn::Num<T> a{T{0b1100}};
    a &= cn::Num<T>{T{0b1010}};
    EXPECT_EQ(a, cn::Num<T>{T{0b1000}});
}

TYPED_TEST(IntSuite, BitwiseOrAssign) {
    using T = TypeParam;
    cn::Num<T> a{T{0b1100}};
    a |= cn::Num<T>{T{0b1010}};
    EXPECT_EQ(a, cn::Num<T>{T{0b1110}});
}

TYPED_TEST(IntSuite, BitwiseXorAssign) {
    using T = TypeParam;
    cn::Num<T> a{T{0b1100}};
    a ^= cn::Num<T>{T{0b1010}};
    EXPECT_EQ(a, cn::Num<T>{T{0b0110}});
}

TYPED_TEST(IntSuite, ShiftLeftAssign) {
    using T = TypeParam;
    cn::Num<T> a{T{1}};
    a <<= 3u;
    EXPECT_EQ(a, cn::Num<T>{T{8}});
}

TYPED_TEST(IntSuite, ShiftRightAssign) {
    using T = TypeParam;
    cn::Num<T> a{T{8}};
    a >>= 2u;
    EXPECT_EQ(a, cn::Num<T>{T{2}});
}

// --- count_ones / count_zeros ---

TYPED_TEST(UnsignedIntSuite, CountOnesZero) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0}}.count_ones(), 0u);
}

TYPED_TEST(UnsignedIntSuite, CountOnesMax) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().count_ones(), static_cast<std::uint32_t>(sizeof(T) * 8u));
}

TYPED_TEST(UnsignedIntSuite, CountOnesValue) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0b1011}}.count_ones(), 3u);
}

TYPED_TEST(UnsignedIntSuite, CountZerosValue) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0}}.count_zeros(), static_cast<std::uint32_t>(sizeof(T) * 8u));
}

// --- leading_zeros / leading_ones ---

TYPED_TEST(UnsignedIntSuite, LeadingZerosZero) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0}}.leading_zeros(), static_cast<std::uint32_t>(sizeof(T) * 8u));
}

TYPED_TEST(UnsignedIntSuite, LeadingZerosOne) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{1}}.leading_zeros(), static_cast<std::uint32_t>(sizeof(T) * 8u - 1u));
}

TYPED_TEST(UnsignedIntSuite, LeadingZerosMax) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().leading_zeros(), 0u);
}

TYPED_TEST(UnsignedIntSuite, LeadingOnesMax) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().leading_ones(), static_cast<std::uint32_t>(sizeof(T) * 8u));
}

TYPED_TEST(UnsignedIntSuite, LeadingOnesZero) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0}}.leading_ones(), 0u);
}

// --- trailing_zeros / trailing_ones ---

TYPED_TEST(UnsignedIntSuite, TrailingZerosZero) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0}}.trailing_zeros(), static_cast<std::uint32_t>(sizeof(T) * 8u));
}

TYPED_TEST(UnsignedIntSuite, TrailingZerosOne) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{1}}.trailing_zeros(), 0u);
}

TYPED_TEST(UnsignedIntSuite, TrailingZerosEight) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{8}}.trailing_zeros(), 3u);
}

TYPED_TEST(UnsignedIntSuite, TrailingOnesMax) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().trailing_ones(), static_cast<std::uint32_t>(sizeof(T) * 8u));
}

TYPED_TEST(UnsignedIntSuite, TrailingOnesValue) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0b0111}}.trailing_ones(), 3u);
}

// --- rotate ---

TEST(Bitwise, RotateLeftU8) {
    EXPECT_EQ(cn::u8{0b1000'0001}.rotate_left(1), cn::u8{0b0000'0011});
}

TEST(Bitwise, RotateRightU8) {
    EXPECT_EQ(cn::u8{0b0000'0011}.rotate_right(1), cn::u8{0b1000'0001});
}

TEST(Bitwise, RotateLeftFull) {
    EXPECT_EQ(cn::u8{42}.rotate_left(8), cn::u8{42});
}

TEST(Bitwise, RotateRightZero) {
    EXPECT_EQ(cn::u32{42}.rotate_right(0), cn::u32{42});
}

// --- reverse_bits ---

TEST(Bitwise, ReverseBitsU8) {
    EXPECT_EQ(cn::u8{0b1100'0000}.reverse_bits(), cn::u8{0b0000'0011});
}

TEST(Bitwise, ReverseBitsU8Sym) {
    EXPECT_EQ(cn::u8{0b1010'0101}.reverse_bits(), cn::u8{0b1010'0101});
}

TEST(Bitwise, ReverseBitsZero) {
    EXPECT_EQ(cn::u32{0}.reverse_bits(), cn::u32{0});
}

TEST(Bitwise, ReverseBitsMax) {
    EXPECT_EQ(cn::u8{255}.reverse_bits(), cn::u8{255});
}

TEST(Bitwise, ReverseBitsDoubleReverse) {
    cn::u32 val{0x12345678};
    EXPECT_EQ(val.reverse_bits().reverse_bits(), val);
}
