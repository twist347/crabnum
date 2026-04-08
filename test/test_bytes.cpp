#include <gtest/gtest.h>

#include "crabnum.h"

TEST(Bytes, U32RoundTripNe) {
    const cn::u32 x{0x12345678u};
    const auto bytes = x.to_ne_bytes();
    EXPECT_EQ(cn::u32::from_ne_bytes(bytes), x);
}

TEST(Bytes, U32Le) {
    const cn::u32 x{0x12345678u};
    const auto bytes = x.to_le_bytes();
    EXPECT_EQ(bytes[0], 0x78);
    EXPECT_EQ(bytes[1], 0x56);
    EXPECT_EQ(bytes[2], 0x34);
    EXPECT_EQ(bytes[3], 0x12);
    EXPECT_EQ(cn::u32::from_le_bytes(bytes), x);
}

TEST(Bytes, U32Be) {
    const cn::u32 x{0x12345678u};
    const auto bytes = x.to_be_bytes();
    EXPECT_EQ(bytes[0], 0x12);
    EXPECT_EQ(bytes[1], 0x34);
    EXPECT_EQ(bytes[2], 0x56);
    EXPECT_EQ(bytes[3], 0x78);
    EXPECT_EQ(cn::u32::from_be_bytes(bytes), x);
}

TEST(Bytes, I16NegativeRoundTrip) {
    const cn::i16 x{-12345};
    EXPECT_EQ(cn::i16::from_le_bytes(x.to_le_bytes()), x);
    EXPECT_EQ(cn::i16::from_be_bytes(x.to_be_bytes()), x);
}

TEST(Bytes, U8) {
    const cn::u8 x{0xABu};
    EXPECT_EQ(x.to_le_bytes()[0], 0xAB);
    EXPECT_EQ(x.to_be_bytes()[0], 0xAB);
    EXPECT_EQ(cn::u8::from_ne_bytes({0xAB}), x);
}

TEST(Bytes, U64Le) {
    const cn::u64 x{0x0123456789ABCDEFull};
    const auto bytes = x.to_le_bytes();
    EXPECT_EQ(bytes[0], 0xEF);
    EXPECT_EQ(bytes[7], 0x01);
    EXPECT_EQ(cn::u64::from_le_bytes(bytes), x);
}

TEST(Bytes, F32RoundTrip) {
    const cn::f32 x{3.14f};
    EXPECT_EQ(cn::f32::from_le_bytes(x.to_le_bytes()).value(), 3.14f);
    EXPECT_EQ(cn::f32::from_be_bytes(x.to_be_bytes()).value(), 3.14f);
    EXPECT_EQ(cn::f32::from_ne_bytes(x.to_ne_bytes()).value(), 3.14f);
}

TEST(Bytes, F64RoundTrip) {
    const cn::f64 x{2.718281828};
    EXPECT_EQ(cn::f64::from_le_bytes(x.to_le_bytes()).value(), 2.718281828);
    EXPECT_EQ(cn::f64::from_be_bytes(x.to_be_bytes()).value(), 2.718281828);
}

TEST(Bytes, LeBeMirrorOnMultiByte) {
    const cn::u32 x{0xDEADBEEFu};
    const auto le = x.to_le_bytes();
    const auto be = x.to_be_bytes();
    EXPECT_EQ(le[0], be[3]);
    EXPECT_EQ(le[1], be[2]);
    EXPECT_EQ(le[2], be[1]);
    EXPECT_EQ(le[3], be[0]);
}
