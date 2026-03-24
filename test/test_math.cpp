#include <gtest/gtest.h>
#include "crabnum.h"

template<typename>
class FloatSuite : public ::testing::Test {
};

using FloatTypes = ::testing::Types<float, double>;
TYPED_TEST_SUITE(FloatSuite, FloatTypes);

template<typename>
class SignedIntSuite : public ::testing::Test {
};

using SignedIntTypes = ::testing::Types<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;
TYPED_TEST_SUITE(SignedIntSuite, SignedIntTypes);

template<typename>
class AllNumSuite : public ::testing::Test {
};

using AllNumTypes = ::testing::Types<
    std::int8_t, std::int16_t, std::int32_t, std::int64_t,
    std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t,
    float, double>;
TYPED_TEST_SUITE(AllNumSuite, AllNumTypes);

// --- abs ---

TYPED_TEST(SignedIntSuite, AbsPositive) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5}}.abs(), cn::Num<T>{T{5}});
}

TYPED_TEST(SignedIntSuite, AbsNegative) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{-5}}.abs(), cn::Num<T>{T{5}});
}

TYPED_TEST(SignedIntSuite, AbsZero) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0}}.abs(), cn::Num<T>{T{0}});
}

TYPED_TEST(FloatSuite, Abs) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{-3.5}}.abs(), cn::Num<T>{T{3.5}});
}

// --- min / max / clamp ---

TYPED_TEST(AllNumSuite, MinMax) {
    using T = TypeParam;
    cn::Num<T> a{T{3}}, b{T{7}};
    EXPECT_EQ(a.min(b), a);
    EXPECT_EQ(a.max(b), b);
    EXPECT_EQ(b.min(a), a);
    EXPECT_EQ(b.max(a), b);
}

TYPED_TEST(AllNumSuite, Clamp) {
    using T = TypeParam;
    cn::Num<T> lo{T{2}}, hi{T{8}};
    EXPECT_EQ(cn::Num<T>{T{5}}.clamp(lo, hi), cn::Num<T>{T{5}});
    EXPECT_EQ(cn::Num<T>{T{1}}.clamp(lo, hi), lo);
    EXPECT_EQ(cn::Num<T>{T{10}}.clamp(lo, hi), hi);
}

// --- sqrt, cbrt, exp, log, log2, log10, pow ---

TYPED_TEST(FloatSuite, Sqrt) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{4}}.sqrt(), cn::Num<T>{T{2}});
}

TYPED_TEST(FloatSuite, Cbrt) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{8}}.cbrt(), cn::Num<T>{T{2}});
}

TYPED_TEST(FloatSuite, ExpLog) {
    using T = TypeParam;
    auto e = cn::consts::e<T>;
    auto one = cn::Num<T>{T{1}};
    EXPECT_EQ(one.exp().value(), static_cast<T>(std::exp(T{1})));
    EXPECT_EQ(e.log().value(), static_cast<T>(std::log(e.value())));
}

TYPED_TEST(FloatSuite, Log2) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{8}}.log2().value(), static_cast<T>(std::log2(T{8})));
}

TYPED_TEST(FloatSuite, Log10) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{100}}.log10().value(), static_cast<T>(std::log10(T{100})));
}

TYPED_TEST(FloatSuite, Pow) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{2}}.pow(cn::Num<T>{T{3}}), cn::Num<T>{T{8}});
}

// --- trig ---

TYPED_TEST(FloatSuite, SinCosTan) {
    using T = TypeParam;
    auto pi = cn::consts::pi<T>;
    EXPECT_EQ(pi.sin().value(), static_cast<T>(std::sin(pi.value())));
    EXPECT_EQ(pi.cos().value(), static_cast<T>(std::cos(pi.value())));
    EXPECT_EQ(pi.tan().value(), static_cast<T>(std::tan(pi.value())));
}

TYPED_TEST(FloatSuite, AsinAcosAtan) {
    using T = TypeParam;
    auto half = cn::Num<T>{T{0.5}};
    EXPECT_EQ(half.asin().value(), static_cast<T>(std::asin(T{0.5})));
    EXPECT_EQ(half.acos().value(), static_cast<T>(std::acos(T{0.5})));
    EXPECT_EQ(half.atan().value(), static_cast<T>(std::atan(T{0.5})));
}

TYPED_TEST(FloatSuite, Atan2) {
    using T = TypeParam;
    cn::Num<T> y{T{1}}, x{T{1}};
    auto res = y.atan2(x);
    EXPECT_EQ(res.value(), static_cast<T>(std::atan2(T{1}, T{1})));
}

TYPED_TEST(FloatSuite, Hypot) {
    using T = TypeParam;
    cn::Num<T> a{T{3}}, b{T{4}};
    EXPECT_EQ(a.hypot(b).value(), static_cast<T>(std::hypot(T{3}, T{4})));
}

// --- rounding ---

TYPED_TEST(FloatSuite, Floor) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{2.7}}.floor(), cn::Num<T>{T{2}});
    EXPECT_EQ(cn::Num<T>{T{-2.3}}.floor(), cn::Num<T>{T{-3}});
}

TYPED_TEST(FloatSuite, Ceil) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{2.3}}.ceil(), cn::Num<T>{T{3}});
    EXPECT_EQ(cn::Num<T>{T{-2.7}}.ceil(), cn::Num<T>{T{-2}});
}

TYPED_TEST(FloatSuite, Round) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{2.5}}.round(), cn::Num<T>{T{3}});
    EXPECT_EQ(cn::Num<T>{T{2.4}}.round(), cn::Num<T>{T{2}});
}

TYPED_TEST(FloatSuite, Trunc) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{2.9}}.trunc(), cn::Num<T>{T{2}});
    EXPECT_EQ(cn::Num<T>{T{-2.9}}.trunc(), cn::Num<T>{T{-2}});
}

TYPED_TEST(FloatSuite, Fract) {
    using T = TypeParam;
    auto val = cn::Num<T>{T{3.75}}.fract();
    EXPECT_EQ(val.value(), static_cast<T>(T{3.75} - std::trunc(T{3.75})));
}

// --- predicates ---

TYPED_TEST(FloatSuite, IsNan) {
    using T = TypeParam;
    EXPECT_FALSE(cn::Num<T>{T{1}}.is_nan());
    EXPECT_TRUE(cn::Num<T>{std::numeric_limits<T>::quiet_NaN()}.is_nan());
}

TYPED_TEST(FloatSuite, IsFinite) {
    using T = TypeParam;
    EXPECT_TRUE(cn::Num<T>{T{1}}.is_finite());
    EXPECT_FALSE(cn::Num<T>{std::numeric_limits<T>::infinity()}.is_finite());
    EXPECT_FALSE(cn::Num<T>{std::numeric_limits<T>::quiet_NaN()}.is_finite());
}

TYPED_TEST(FloatSuite, IsInf) {
    using T = TypeParam;
    EXPECT_FALSE(cn::Num<T>{T{1}}.is_inf());
    EXPECT_TRUE(cn::Num<T>{std::numeric_limits<T>::infinity()}.is_inf());
    EXPECT_TRUE(cn::Num<T>{-std::numeric_limits<T>::infinity()}.is_inf());
}
