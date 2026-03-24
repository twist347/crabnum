# crabnum

[![CI](https://github.com/twist347/crabnum/actions/workflows/ci.yml/badge.svg)](https://github.com/twist347/crabnum/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Rust's numeric types in a single C++23 header.

`Num<T>` is a zero-cost wrapper around primitive numeric types that brings Rust-style safety and API to C++: checked, saturating, wrapping, and overflowing arithmetic, safe casts, bitwise intrinsics, and more.

## Quick start

```cpp
#include "crabnum.h"
using namespace cn::literals;

auto a = 42_i32;
auto b = cn::i32{8};

// basic arithmetic — same type only, no implicit mixing
auto c = a + b;              // cn::i32{50}
auto neg = -a;               // cn::i32{-42}

// checked — returns std::optional
auto safe = a.checked_add(b);           // optional(50)
auto overflow = cn::i32::MAX_VAL().checked_add(1_i32);  // nullopt

// saturating — clamps to MIN_VAL()/MAX_VAL()
cn::u8::MAX_VAL().saturating_add(cn::u8{1});  // 255

// wrapping — modular arithmetic, no UB
cn::u8::MAX_VAL().wrapping_add(cn::u8{1});    // 0
cn::i8::MAX_VAL().wrapping_add(cn::i8{1});    // -128

// overflowing — wrapping result + overflow flag
auto [res, ov] = cn::i32::MAX_VAL().overflowing_add(1_i32);  // {MIN_VAL(), true}
```

## Features

**Arithmetic:** `+`, `-`, `*`, `/`, `%` — `Num<T>` only, no implicit `T` mixing

**Safe arithmetic:**
- `checked_*` → `std::optional<Num>` — nullopt on overflow
- `saturating_*` → clamps to `MIN_VAL()`/`MAX_VAL()`
- `wrapping_*` → modular arithmetic, defined behavior for signed
- `overflowing_*` → `std::pair<Num, bool>` — wrapping result + overflow flag

Operations: `add`, `sub`, `mul`, `div`, `rem`, `neg`, `abs`, `pow`, `shl`, `shr`

**Casts:**
- `as<Num<U>>()` — unchecked cast
- `try_as<U>()` → `std::optional<Num<U>>` — nullopt if value doesn't fit
- `saturating_as<U>()` — clamps to target range

**Bitwise:** `&`, `|`, `^`, `~`, `<<`, `>>`, `count_ones`, `count_zeros`, `leading_zeros`, `trailing_zeros`, `leading_ones`, `trailing_ones`, `rotate_left`, `rotate_right`, `reverse_bits`

**Math:** `abs`, `min`, `max`, `abs_diff`, `clamp`, `gcd`, `lcm`, `midpoint`, `sqrt`, `cbrt`, `pow`, `exp`, `exp2`, `expm1`, `log`, `log2`, `log10`, `log1p`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`, `hypot`, `sinh`, `cosh`, `tanh`, `asinh`, `acosh`, `atanh`, `erf`, `erfc`, `tgamma`, `lgamma`, `copysign`, `fmod`, `remainder`, `fma`, `nextafter`, `to_degrees`, `to_radians`, `floor`, `ceil`, `round`, `trunc`, `fract`

**Predicates:** `is_nan`, `is_finite`, `is_inf`, `is_positive`, `is_negative`, `signum`

**Constants:** `MIN_VAL()`, `MAX_VAL()`, `EPS_VAL()`, `INF_VAL()`, `NAN_VAL()`

**Concepts:** `IsNum`, `IsIntNum`, `IsSignedNum`, `IsUnsignedNum`, `IsFloatNum`

**Parsing:** `to_string`, `from_string`, `parse`, `write_to`

**Compat:** `std::hash`, `std::format`, `std::ostream`/`std::istream`, `std::accumulate`, `std::sort`

## Types

```
cn::i8  cn::i16  cn::i32  cn::i64
cn::u8  cn::u16  cn::u32  cn::u64
cn::isize  cn::usize
cn::f32  cn::f64
```

Literals: `42_i32`, `255_u8`, `3.14_f64`, etc.

## Requirements

- C++23
- GCC 15+ or Clang 18+

## Usage

Header-only — copy `include/crabnum.h` into your project, or add via CMake:

```cmake
add_subdirectory(crabnum)
target_link_libraries(your_target crabnum::crabnum)
```

## License

[MIT](LICENSE)
