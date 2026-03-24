#include <gtest/gtest.h>

#include "crabnum.h"

using namespace cn::literals;

TEST(Literals, IntLiterals) {
    auto a = 42_i8;
    auto b = 42_i16;
    auto c = 42_i32;
    auto d = 42_i64;

    static_assert(std::is_same_v<decltype(a), cn::i8>);
    static_assert(std::is_same_v<decltype(b), cn::i16>);
    static_assert(std::is_same_v<decltype(c), cn::i32>);
    static_assert(std::is_same_v<decltype(d), cn::i64>);

    EXPECT_EQ(a.value(), std::int8_t{42});
    EXPECT_EQ(b.value(), std::int16_t{42});
    EXPECT_EQ(c.value(), std::int32_t{42});
    EXPECT_EQ(d.value(), std::int64_t{42});
}

TEST(Literals, UnsignedLiterals) {
    auto a = 42_u8;
    auto b = 42_u16;
    auto c = 42_u32;
    auto d = 42_u64;

    static_assert(std::is_same_v<decltype(a), cn::u8>);
    static_assert(std::is_same_v<decltype(b), cn::u16>);
    static_assert(std::is_same_v<decltype(c), cn::u32>);
    static_assert(std::is_same_v<decltype(d), cn::u64>);

    EXPECT_EQ(a.value(), std::uint8_t{42});
    EXPECT_EQ(b.value(), std::uint16_t{42});
    EXPECT_EQ(c.value(), std::uint32_t{42});
    EXPECT_EQ(d.value(), std::uint64_t{42});
}

TEST(Literals, SizeLiterals) {
    auto a = 42_isize;
    auto b = 42_usize;

    static_assert(std::is_same_v<decltype(a), cn::isize>);
    static_assert(std::is_same_v<decltype(b), cn::usize>);

    EXPECT_EQ(a.value(), std::ptrdiff_t{42});
    EXPECT_EQ(b.value(), std::size_t{42});
}

TEST(Literals, FloatLiteralsFromDouble) {
    auto a = 3.14_f32;
    auto b = 3.14_f64;

    static_assert(std::is_same_v<decltype(a), cn::f32>);
    static_assert(std::is_same_v<decltype(b), cn::f64>);
}

TEST(Literals, FloatLiteralsFromInt) {
    auto a = 42_f32;
    auto b = 42_f64;

    static_assert(std::is_same_v<decltype(a), cn::f32>);
    static_assert(std::is_same_v<decltype(b), cn::f64>);

    EXPECT_EQ(a.value(), float{42});
    EXPECT_EQ(b.value(), double{42});
}
