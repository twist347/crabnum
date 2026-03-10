#include <numeric>

#include "gtest/gtest.h"
#include "crabnum.h"

// TODO: equality of floats

using namespace cn::literals;

/* ---------- TYPE REGISTRATION ---------- */

template<typename>
class SignedIntSuite : public ::testing::Test {
};

using SignedIntTypes = ::testing::Types<
    std::int8_t,
    std::int16_t,
    std::int32_t,
    std::int64_t
>;
TYPED_TEST_SUITE(SignedIntSuite, SignedIntTypes);

template<typename>
class UnsignedIntSuite : public ::testing::Test {
};

using UnsignedIntTypes = ::testing::Types<
    std::uint8_t,
    std::uint16_t,
    std::uint32_t,
    std::uint64_t
>;
TYPED_TEST_SUITE(UnsignedIntSuite, UnsignedIntTypes);

template<typename>
class FloatSuite : public ::testing::Test {
};

using FloatTypes = ::testing::Types<
    std::float32_t,
    std::float64_t
>;
TYPED_TEST_SUITE(FloatSuite, FloatTypes);

/* ---------- TEST ---------- */

/* ---------- SIGNED INT ---------- */

TYPED_TEST(SignedIntSuite, Arith) {
    using T = TypeParam;
    cn::Num<T> a{T{7}}, b{T{4}};

    EXPECT_EQ(a + b, T{11});
    EXPECT_EQ(a - b, T{3});
    EXPECT_EQ(a * b, T{28});
    EXPECT_EQ(a / b, T{1});
    EXPECT_EQ(a % b, T{3});

    ++a;
    EXPECT_EQ(a, T{8});
    --a;
    EXPECT_EQ(a, T{7});
}

TYPED_TEST(SignedIntSuite, Limits) {
    using T = TypeParam;

    ASSERT_EQ(cn::Num<T>::MAX, std::numeric_limits<T>::max());
    ASSERT_EQ(cn::Num<T>::MIN, std::numeric_limits<T>::min());
}

/* ---------- SIGNED INT ---------- */

TYPED_TEST(UnsignedIntSuite, Arith) {
    using T = TypeParam;
    cn::Num<T> a{T{7}}, b{T{4}};

    EXPECT_EQ(a + b, T{11});
    EXPECT_EQ(a - b, T{3});
    EXPECT_EQ(a * b, T{28});
    EXPECT_EQ(a / b, T{1});
    EXPECT_EQ(a % b, T{3});

    ++a;
    EXPECT_EQ(a, T{8});
    --a;
    EXPECT_EQ(a, T{7});
}

TYPED_TEST(UnsignedIntSuite, Limits) {
    using T = TypeParam;

    ASSERT_EQ(cn::Num<T>::MAX, std::numeric_limits<T>::max());
    ASSERT_EQ(cn::Num<T>::MIN, std::numeric_limits<T>::min());
}

/* ---------- FLOAT ---------- */

TYPED_TEST(FloatSuite, Arith) {
    using T = TypeParam;
    cn::Num<T> a{T{7}}, b{T{4}};

    EXPECT_EQ(a + b, T{11});
    EXPECT_EQ(a - b, T{3});
    EXPECT_EQ(a * b, T{28});
    EXPECT_EQ(a / b, T{1.75});

    ++a;
    EXPECT_EQ(a, T{8});
    --a;
    EXPECT_EQ(a, T{7});
}

TYPED_TEST(FloatSuite, CommonMath) {
    using T = TypeParam;
    const cn::Num<T> e = cn::consts::e<T>;
    const cn::Num<T> pi = cn::consts::pi<T>;

    EXPECT_EQ(e.sqrt(), std::sqrt(e.template as<T>()));
    EXPECT_EQ(e.cbrt(), std::cbrt(e.template as<T>()));
    EXPECT_EQ(pi.exp(), std::exp(pi.template as<T>()));
    EXPECT_EQ(pi.log(), std::log(pi.template as<T>()));
    EXPECT_EQ(pi.log10(), std::log10(pi.template as<T>()));
}

TYPED_TEST(FloatSuite, TrigFunc) {
    using T = TypeParam;
    const cn::Num<T> e = cn::consts::e<T>;
    const cn::Num<T> inv_pi = cn::consts::inv_pi<T>;

    EXPECT_EQ(e.sin(), std::sin(e.template as<T>()));
    EXPECT_EQ(e.cos(), std::cos(e.template as<T>()));
    EXPECT_EQ(e.tan(), std::tan(e.template as<T>()));
    EXPECT_EQ(inv_pi.asin(), std::asin(inv_pi.template as<T>()));
    EXPECT_EQ(inv_pi.acos(), std::acos(inv_pi.template as<T>()));
    EXPECT_EQ(inv_pi.atan(), std::atan(inv_pi.template as<T>()));
}

/* ---------- STD CONTAINER COMPAT ---------- */

TEST(A, B) {
    const std::vector<cn::i32> v{1, 2, 3};
    const auto val = std::accumulate(v.cbegin(), v.cend(), cn::i32{0});
    ASSERT_EQ(val, 6);
}

/* ---------- STD ALGO COMPAT ---------- */

