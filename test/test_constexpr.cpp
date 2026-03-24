#include <gtest/gtest.h>

#include "crabnum.h"

using namespace cn::literals;

// construction & value
static_assert(cn::i32{42}.value() == 42);
static_assert(cn::u8{0}.value() == 0);

// limits
static_assert(cn::i8::MAX_VAL().value() == 127);
static_assert(cn::i8::MIN_VAL().value() == -128);
static_assert(cn::u8::MAX_VAL().value() == 255);

// arithmetic
static_assert(cn::i32{3} + cn::i32{4} == cn::i32{7});
static_assert(cn::i32{10} - cn::i32{3} == cn::i32{7});
static_assert(cn::i32{3} * cn::i32{4} == cn::i32{12});
static_assert(cn::i32{12} / cn::i32{4} == cn::i32{3});
static_assert(cn::i32{10} % cn::i32{3} == cn::i32{1});

// unary
static_assert(-cn::i32{5} == cn::i32{-5});
static_assert(+cn::i32{5} == cn::i32{5});

// comparison
static_assert(cn::i32{3} < cn::i32{5});
static_assert(cn::i32{5} > cn::i32{3});
static_assert(cn::i32{3} == cn::i32{3});
static_assert(cn::i32{3} != cn::i32{4});

// abs / min / max / clamp
static_assert(cn::i32{-5}.abs() == cn::i32{5});
static_assert(cn::i32{3}.min(cn::i32{7}) == cn::i32{3});
static_assert(cn::i32{3}.max(cn::i32{7}) == cn::i32{7});
static_assert(cn::i32{1}.clamp(cn::i32{2}, cn::i32{8}) == cn::i32{2});

// checked
static_assert(cn::i32{3}.checked_add(cn::i32{4}).has_value());
static_assert(cn::i32{3}.checked_add(cn::i32{4}).value() == cn::i32{7});
static_assert(!cn::i32::MAX_VAL().checked_add(cn::i32{1}).has_value());
static_assert(cn::i32{3}.checked_mul(cn::i32{4}).value() == cn::i32{12});
static_assert(!cn::i32::MIN_VAL().checked_neg().has_value());

// saturating
static_assert(cn::i32::MAX_VAL().saturating_add(cn::i32{1}) == cn::i32::MAX_VAL());
static_assert(cn::i32::MIN_VAL().saturating_sub(cn::i32{1}) == cn::i32::MIN_VAL());
static_assert(cn::u8{200}.saturating_add(cn::u8{100}) == cn::u8::MAX_VAL());

// wrapping
static_assert(cn::u8::MAX_VAL().wrapping_add(cn::u8{1}) == cn::u8{0});
static_assert(cn::i8::MAX_VAL().wrapping_add(cn::i8{1}) == cn::i8::MIN_VAL());

// overflowing
static_assert(cn::i32{3}.overflowing_add(cn::i32{4}).first == cn::i32{7});
static_assert(!cn::i32{3}.overflowing_add(cn::i32{4}).second);
static_assert(cn::i32::MAX_VAL().overflowing_add(cn::i32{1}).second);

// bitwise
static_assert((cn::u8{0b1100} & cn::u8{0b1010}) == cn::u8{0b1000});
static_assert((cn::u8{0b1100} | cn::u8{0b1010}) == cn::u8{0b1110});
static_assert((cn::u8{0b1100} ^ cn::u8{0b1010}) == cn::u8{0b0110});
static_assert(cn::u8{0b1011}.count_ones() == 3);
static_assert(cn::u8{1}.leading_zeros() == 7);
static_assert(cn::u8{8}.trailing_zeros() == 3);
static_assert(cn::u8{0b1100'0000}.reverse_bits() == cn::u8{0b0000'0011});

// cast
static_assert(cn::i32{42}.as_raw<std::int64_t>() == 42);
static_assert(cn::i32{42}.as<cn::i64>().value() == 42);
static_assert(cn::i32{42}.try_as<std::int8_t>().has_value());
static_assert(!cn::i32{300}.try_as<std::int8_t>().has_value());
static_assert(cn::i32{300}.saturating_as<std::int8_t>().value() == 127);

// predicates
static_assert(cn::i32{5}.is_positive());
static_assert(cn::i32{-5}.is_negative());
static_assert(!cn::u32{0}.is_positive());

// signum
static_assert(cn::i32{5}.signum() == cn::i32{1});
static_assert(cn::i32{-5}.signum() == cn::i32{-1});
static_assert(cn::i32{0}.signum() == cn::i32{0});

// literals
static_assert(42_i32 == cn::i32{42});
static_assert(42_u8 == cn::u8{42});
static_assert(42_i64 == cn::i64{42});

// pow (checked/saturating/wrapping)
static_assert(cn::i32{2}.checked_pow(10).value() == cn::i32{1024});
static_assert(cn::u8{2}.saturating_pow(8) == cn::u8::MAX_VAL());
static_assert(cn::u8{2}.wrapping_pow(8) == cn::u8{0});

// gtest needs at least one test to run
TEST(Constexpr, CompileTimeChecksPass) {
    SUCCEED();
}
