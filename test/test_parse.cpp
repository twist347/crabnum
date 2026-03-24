#include <gtest/gtest.h>

#include "crabnum.h"

template<typename>
class IntSuite : public ::testing::Test {
};

using IntTypes = ::testing::Types<
    std::int8_t, std::int16_t, std::int32_t, std::int64_t,
    std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;
TYPED_TEST_SUITE(IntSuite, IntTypes);

template<typename>
class FloatSuite : public ::testing::Test {
};

using FloatTypes = ::testing::Types<float, double>;
TYPED_TEST_SUITE(FloatSuite, FloatTypes);

// --- to_string ---

TYPED_TEST(IntSuite, ToStringBasic) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{42}}.to_string(), "42");
    EXPECT_EQ(cn::Num<T>{T{0}}.to_string(), "0");
}

TEST(Parse, ToStringFloat) {
    const auto s = cn::f64{3.14}.to_string();
    EXPECT_FALSE(s.empty());
    const auto parsed = cn::f64::from_string(s);
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->value(), double{3.14});
}

// --- from_string ---

TYPED_TEST(IntSuite, FromStringValid) {
    using T = TypeParam;
    const auto res = cn::Num<T>::from_string("42");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(*res, cn::Num<T>{T{42}});
}

TYPED_TEST(IntSuite, FromStringInvalid) {
    using T = TypeParam;
    const auto res = cn::Num<T>::from_string("abc");
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(IntSuite, FromStringTrailingChars) {
    using T = TypeParam;
    const auto res = cn::Num<T>::from_string("42abc");
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(IntSuite, FromStringEmpty) {
    using T = TypeParam;
    const auto res = cn::Num<T>::from_string("");
    EXPECT_FALSE(res.has_value());
}

TYPED_TEST(FloatSuite, FromStringValid) {
    using T = TypeParam;
    const auto res = cn::Num<T>::from_string("2.5");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->value(), T{2.5});
}

// --- parse (partial) ---

TYPED_TEST(IntSuite, ParsePartial) {
    using T = TypeParam;
    const auto res = cn::Num<T>::parse("42abc");
    ASSERT_TRUE(res.has_value());
    const auto [num, ptr] = *res;
    EXPECT_EQ(num, cn::Num<T>{T{42}});
    EXPECT_EQ(std::string_view(ptr, 3), "abc");
}

// --- write_to ---

TEST(Parse, WriteTo) {
    char buf[64]{};
    const cn::i32 a{12345};
    const auto res = a.write_to(buf, sizeof(buf));
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(std::string_view(buf, *res - buf), "12345");
}

TEST(Parse, WriteToSmallBuffer) {
    char buf[2]{};
    const cn::i32 a{12345};
    const auto res = a.write_to(buf, sizeof(buf));
    EXPECT_FALSE(res.has_value());
}
