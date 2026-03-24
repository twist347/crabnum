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
    const cn::Num<T> a{T{3}}, b{T{7}};
    EXPECT_EQ(a.min(b), a);
    EXPECT_EQ(a.max(b), b);
    EXPECT_EQ(b.min(a), a);
    EXPECT_EQ(b.max(a), b);
}

TYPED_TEST(AllNumSuite, Clamp) {
    using T = TypeParam;
    const cn::Num<T> lo{T{2}}, hi{T{8}};
    EXPECT_EQ(cn::Num<T>{T{5}}.clamp(lo, hi), cn::Num<T>{T{5}});
    EXPECT_EQ(cn::Num<T>{T{1}}.clamp(lo, hi), lo);
    EXPECT_EQ(cn::Num<T>{T{10}}.clamp(lo, hi), hi);
}

// --- gcd ---

TYPED_TEST(IntSuite, GcdBasic) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{12}}.gcd(cn::Num<T>{T{8}}).value(), T{4});
}

TYPED_TEST(IntSuite, GcdWithZero) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{7}}.gcd(cn::Num<T>{T{0}}).value(), T{7});
    EXPECT_EQ(cn::Num<T>{T{0}}.gcd(cn::Num<T>{T{7}}).value(), T{7});
}

TYPED_TEST(IntSuite, GcdBothZero) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{0}}.gcd(cn::Num<T>{T{0}}).value(), T{0});
}

TYPED_TEST(IntSuite, GcdCoprime) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{7}}.gcd(cn::Num<T>{T{11}}).value(), T{1});
}

TYPED_TEST(SignedIntSuite, GcdNegative) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{-12}}.gcd(cn::Num<T>{T{8}}).value(), T{4});
    EXPECT_EQ(cn::Num<T>{T{12}}.gcd(cn::Num<T>{T{-8}}).value(), T{4});
}

// --- lcm ---

TYPED_TEST(IntSuite, LcmBasic) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{4}}.lcm(cn::Num<T>{T{6}}).value(), T{12});
}

TYPED_TEST(IntSuite, LcmWithZero) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5}}.lcm(cn::Num<T>{T{0}}).value(), T{0});
    EXPECT_EQ(cn::Num<T>{T{0}}.lcm(cn::Num<T>{T{5}}).value(), T{0});
}

TYPED_TEST(IntSuite, LcmEqual) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{7}}.lcm(cn::Num<T>{T{7}}).value(), T{7});
}

// --- midpoint ---

TYPED_TEST(AllNumSuite, MidpointBasic) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{2}}.midpoint(cn::Num<T>{T{4}}).value(), T{3});
}

TYPED_TEST(AllNumSuite, MidpointSame) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5}}.midpoint(cn::Num<T>{T{5}}).value(), T{5});
}

TYPED_TEST(UnsignedIntSuite, MidpointLargeValues) {
    using T = TypeParam;
    // midpoint of MAX and MAX-2 should be MAX-1, no overflow
    const auto max = cn::Num<T>::MAX_VAL();
    const auto max2 = cn::Num<T>{static_cast<T>(std::numeric_limits<T>::max() - 2)};
    EXPECT_EQ(max.midpoint(max2).value(), static_cast<T>(std::numeric_limits<T>::max() - 1));
}

TYPED_TEST(SignedIntSuite, MidpointNegative) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{-4}}.midpoint(cn::Num<T>{T{-2}}).value(), T{-3});
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
    const auto e = cn::consts::e<T>;
    const auto one = cn::Num<T>{T{1}};
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
    const auto pi = cn::consts::pi<T>;
    EXPECT_EQ(pi.sin().value(), static_cast<T>(std::sin(pi.value())));
    EXPECT_EQ(pi.cos().value(), static_cast<T>(std::cos(pi.value())));
    EXPECT_EQ(pi.tan().value(), static_cast<T>(std::tan(pi.value())));
}

TYPED_TEST(FloatSuite, AsinAcosAtan) {
    using T = TypeParam;
    const auto half = cn::Num<T>{T{0.5}};
    EXPECT_EQ(half.asin().value(), static_cast<T>(std::asin(T{0.5})));
    EXPECT_EQ(half.acos().value(), static_cast<T>(std::acos(T{0.5})));
    EXPECT_EQ(half.atan().value(), static_cast<T>(std::atan(T{0.5})));
}

TYPED_TEST(FloatSuite, Atan2) {
    using T = TypeParam;
    const cn::Num<T> y{T{1}}, x{T{1}};
    const auto res = y.atan2(x);
    EXPECT_EQ(res.value(), static_cast<T>(std::atan2(T{1}, T{1})));
}

TYPED_TEST(FloatSuite, Hypot) {
    using T = TypeParam;
    const cn::Num<T> a{T{3}}, b{T{4}};
    EXPECT_EQ(a.hypot(b).value(), static_cast<T>(std::hypot(T{3}, T{4})));
}

// --- exp2 / expm1 / log1p ---

TYPED_TEST(FloatSuite, Exp2) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{3}}.exp2().value(), static_cast<T>(std::exp2(T{3})));
}

TYPED_TEST(FloatSuite, Expm1) {
    using T = TypeParam;
    const auto val = cn::Num<T>{T{0.001}}.expm1();
    EXPECT_EQ(val.value(), static_cast<T>(std::expm1(T{0.001})));
}

TYPED_TEST(FloatSuite, Log1p) {
    using T = TypeParam;
    const auto val = cn::Num<T>{T{0.001}}.log1p();
    EXPECT_EQ(val.value(), static_cast<T>(std::log1p(T{0.001})));
}

// --- special functions ---

TYPED_TEST(FloatSuite, Erf) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{1}}.erf().value(), static_cast<T>(std::erf(T{1})));
}

TYPED_TEST(FloatSuite, Erfc) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{1}}.erfc().value(), static_cast<T>(std::erfc(T{1})));
}

TYPED_TEST(FloatSuite, Tgamma) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5}}.tgamma().value(), static_cast<T>(std::tgamma(T{5})));
}

TYPED_TEST(FloatSuite, Lgamma) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5}}.lgamma().value(), static_cast<T>(std::lgamma(T{5})));
}

// --- float utils ---

TYPED_TEST(FloatSuite, Copysign) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{3}}.copysign(cn::Num<T>{T{-1}}).value(), T{-3});
    EXPECT_EQ(cn::Num<T>{T{-3}}.copysign(cn::Num<T>{T{1}}).value(), T{3});
}

TYPED_TEST(FloatSuite, Fmod) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5.5}}.fmod(cn::Num<T>{T{2}}).value(), static_cast<T>(std::fmod(T{5.5}, T{2})));
}

TYPED_TEST(FloatSuite, Remainder) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{5.5}}.remainder(cn::Num<T>{T{2}}).value(), static_cast<T>(std::remainder(T{5.5}, T{2})));
}

TYPED_TEST(FloatSuite, Fma) {
    using T = TypeParam;
    // this * a + b = 2 * 3 + 4 = 10
    EXPECT_EQ(cn::Num<T>{T{2}}.fma(cn::Num<T>{T{3}}, cn::Num<T>{T{4}}).value(), T{10});
}

TYPED_TEST(FloatSuite, Nextafter) {
    using T = TypeParam;
    const auto next = cn::Num<T>{T{1}}.nextafter(cn::Num<T>{T{2}});
    EXPECT_GT(next.value(), T{1});
    const auto prev = cn::Num<T>{T{1}}.nextafter(cn::Num<T>{T{0}});
    EXPECT_LT(prev.value(), T{1});
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
    const auto val = cn::Num<T>{T{3.75}}.fract();
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

// --- to_degrees / to_radians ---

TYPED_TEST(FloatSuite, ToDegrees) {
    using T = TypeParam;
    const auto pi = cn::consts::pi<T>;
    const auto deg = pi.to_degrees();
    EXPECT_NEAR(static_cast<double>(deg.value()), 180.0, 1e-10);
}

TYPED_TEST(FloatSuite, ToRadians) {
    using T = TypeParam;
    const auto deg = cn::Num<T>{T{180}};
    const auto rad = deg.to_radians();
    EXPECT_NEAR(static_cast<double>(rad.value()), static_cast<double>(std::numbers::pi_v<T>), 1e-10);
}

TYPED_TEST(FloatSuite, DegreesRadiansRoundTrip) {
    using T = TypeParam;
    const auto val = cn::Num<T>{T{45}};
    const auto roundtrip = val.to_radians().to_degrees();
    EXPECT_NEAR(static_cast<double>(roundtrip.value()), 45.0, 1e-10);
}

// --- abs_diff ---

TYPED_TEST(AllNumSuite, AbsDiffBasic) {
    using T = TypeParam;
    const cn::Num<T> a{T{7}}, b{T{3}};
    EXPECT_EQ(a.abs_diff(b).value(), b.abs_diff(a).value());
}

TYPED_TEST(SignedIntSuite, AbsDiffNegative) {
    using T = TypeParam;
    using U = std::make_unsigned_t<T>;
    const auto res = cn::Num<T>{T{-3}}.abs_diff(cn::Num<T>{T{4}});
    static_assert(std::is_same_v<decltype(res), const cn::Num<U>>);
    EXPECT_EQ(res.value(), U{7});
}

TYPED_TEST(SignedIntSuite, AbsDiffMinMax) {
    using T = TypeParam;
    using U = std::make_unsigned_t<T>;
    const auto res = cn::Num<T>::MIN_VAL().abs_diff(cn::Num<T>::MAX_VAL());
    EXPECT_EQ(res.value(), std::numeric_limits<U>::max());
}

TYPED_TEST(UnsignedIntSuite, AbsDiffUnsigned) {
    using T = TypeParam;
    EXPECT_EQ(cn::Num<T>{T{10}}.abs_diff(cn::Num<T>{T{3}}).value(), T{7});
    EXPECT_EQ(cn::Num<T>{T{3}}.abs_diff(cn::Num<T>{T{10}}).value(), T{7});
}

TYPED_TEST(FloatSuite, AbsDiffFloat) {
    using T = TypeParam;
    const auto res = cn::Num<T>{T{-2.5}}.abs_diff(cn::Num<T>{T{1.5}});
    EXPECT_EQ(res.value(), T{4});
}
