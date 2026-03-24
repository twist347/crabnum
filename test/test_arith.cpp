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
class FloatSuite : public ::testing::Test {
};

using FloatTypes = ::testing::Types<float, double>;
TYPED_TEST_SUITE(FloatSuite, FloatTypes);

template<typename>
class AllNumSuite : public ::testing::Test {
};

using AllNumTypes = ::testing::Types<
    std::int8_t, std::int16_t, std::int32_t, std::int64_t,
    std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t,
    float, double>;
TYPED_TEST_SUITE(AllNumSuite, AllNumTypes);

template<typename>
class IntSuite : public ::testing::Test {
};

using IntTypes = ::testing::Types<
    std::int8_t, std::int16_t, std::int32_t, std::int64_t,
    std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;
TYPED_TEST_SUITE(IntSuite, IntTypes);

// --- basic arithmetic ---

TYPED_TEST(SignedIntSuite, BasicArith) {
    using T = TypeParam;
    constexpr cn::Num<T> a{T{7}}, b{T{4}};

    EXPECT_EQ(a + b, cn::Num<T>{T{11}});
    EXPECT_EQ(a - b, cn::Num<T>{T{3}});
    EXPECT_EQ(a * b, cn::Num<T>{T{28}});
    EXPECT_EQ(a / b, cn::Num<T>{T{1}});
    EXPECT_EQ(a % b, cn::Num<T>{T{3}});
}

TYPED_TEST(UnsignedIntSuite, BasicArith) {
    using T = TypeParam;
    constexpr cn::Num<T> a{T{7}}, b{T{4}};

    EXPECT_EQ(a + b, cn::Num<T>{T{11}});
    EXPECT_EQ(a - b, cn::Num<T>{T{3}});
    EXPECT_EQ(a * b, cn::Num<T>{T{28}});
    EXPECT_EQ(a / b, cn::Num<T>{T{1}});
    EXPECT_EQ(a % b, cn::Num<T>{T{3}});
}

TYPED_TEST(FloatSuite, BasicArith) {
    using T = TypeParam;
    constexpr cn::Num<T> a{T{7}}, b{T{4}};

    EXPECT_EQ(a + b, cn::Num<T>{T{11}});
    EXPECT_EQ(a - b, cn::Num<T>{T{3}});
    EXPECT_EQ(a * b, cn::Num<T>{T{28}});
    EXPECT_EQ(a / b, cn::Num<T>{T{1.75}});
}

// --- unary ---

TYPED_TEST(SignedIntSuite, UnaryMinus) {
    using T = TypeParam;
    constexpr cn::Num<T> a{T{5}};
    EXPECT_EQ(-a, cn::Num<T>{T{-5}});
    EXPECT_EQ(+a, a);
}

TYPED_TEST(FloatSuite, UnaryMinus) {
    using T = TypeParam;
    const cn::Num<T> a{T{2.5}};
    EXPECT_EQ(-a, cn::Num<T>{T{-2.5}});
    EXPECT_EQ(+a, a);
}

// --- increment / decrement ---

TYPED_TEST(AllNumSuite, IncrDecr) {
    using T = TypeParam;
    cn::Num<T> a{T{5}};

    const auto pre = ++a;
    EXPECT_EQ(a, cn::Num<T>{T{6}});
    EXPECT_EQ(pre, cn::Num<T>{T{6}});

    const auto post = a++;
    EXPECT_EQ(a, cn::Num<T>{T{7}});
    EXPECT_EQ(post, cn::Num<T>{T{6}});

    --a;
    EXPECT_EQ(a, cn::Num<T>{T{6}});

    const auto postd = a--;
    EXPECT_EQ(a, cn::Num<T>{T{5}});
    EXPECT_EQ(postd, cn::Num<T>{T{6}});
}

// --- compound assignment ---

TYPED_TEST(AllNumSuite, CompoundAssignment) {
    using T = TypeParam;
    cn::Num<T> a{T{10}};

    a += cn::Num<T>{T{5}};
    EXPECT_EQ(a, cn::Num<T>{T{15}});

    a -= cn::Num<T>{T{3}};
    EXPECT_EQ(a, cn::Num<T>{T{12}});

    a *= cn::Num<T>{T{2}};
    EXPECT_EQ(a, cn::Num<T>{T{24}});

    a /= cn::Num<T>{T{4}};
    EXPECT_EQ(a, cn::Num<T>{T{6}});
}

TYPED_TEST(IntSuite, CompoundModulo) {
    using T = TypeParam;
    cn::Num<T> a{T{17}};
    a %= cn::Num<T>{T{5}};
    EXPECT_EQ(a, cn::Num<T>{T{2}});
}

// --- comparison ---

TYPED_TEST(AllNumSuite, Comparison) {
    using T = TypeParam;
    constexpr cn::Num<T> a{T{3}}, b{T{5}}, c{T{3}};

    EXPECT_TRUE(a == c);
    EXPECT_TRUE(a != b);
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a <= c);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b >= a);
    EXPECT_TRUE(a >= c);
}

// --- limits ---

TYPED_TEST(AllNumSuite, Limits) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>::MAX_VAL().value(), std::numeric_limits<T>::max());
    EXPECT_EQ(cn::Num<T>::MIN_VAL().value(), std::numeric_limits<T>::lowest());
}

// --- value / explicit cast ---

TYPED_TEST(AllNumSuite, ValueAndCast) {
    using T = TypeParam;
    constexpr cn::Num<T> a{T{42}};
    EXPECT_EQ(a.value(), T{42});
    EXPECT_EQ(static_cast<T>(a), T{42});
}

// --- default construction ---

TYPED_TEST(AllNumSuite, DefaultConstruction) {
    using T = TypeParam;
    constexpr cn::Num<T> a{};
    EXPECT_EQ(a.value(), T{0});
}
