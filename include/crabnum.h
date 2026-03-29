#pragma once

#include <type_traits>
#include <cmath>
#include <optional>
#include <expected>
#include <limits>
#include <utility>
#include <ostream>
#include <istream>
#include <compare>
#include <concepts>
#include <cstdint>
#include <cstddef>
#include <string>
#include <array>
#include <bit>
#include <charconv>
#include <format>
#include <numeric>
#include <numbers>

static_assert(sizeof(float) == 4, "float must be 32-bit");
static_assert(sizeof(double) == 8, "double must be 64-bit");

namespace cn {
    template<typename T>
    concept Integral = std::integral<T> && !std::same_as<T, bool>;

    template<typename T>
    concept Floating = std::floating_point<T>;

    template<typename T>
    concept Number = (Integral<T> || Floating<T>) && !std::same_as<T, bool>;

    template<Number T>
    class Num {
    public:
        using value_type = T;

        constexpr Num() = default;

        explicit constexpr Num(T v) : m_v{v} {
        }

        [[nodiscard]] constexpr auto value() const noexcept -> T { return m_v; }

        explicit constexpr operator T() const noexcept { return m_v; }

        /* ========== CAST ========== */

        template<Number U>
        [[nodiscard]] constexpr auto as_raw() const noexcept -> U {
            return static_cast<U>(m_v);
        }

        template<typename U>
        [[nodiscard]] constexpr auto as() const noexcept -> std::remove_cvref_t<U> {
            using Out = std::remove_cvref_t<U>;
            return Out{static_cast<Out::value_type>(m_v)};
        }

        // Returns nullopt if the value cannot be represented exactly in U.
        template<Number U>
        [[nodiscard]] constexpr auto try_as() const noexcept -> std::optional<Num<U> > {
            const auto casted = static_cast<U>(m_v);
            // round-trip check: cast back and compare
            if (static_cast<T>(casted) != m_v) {
                return std::nullopt;
            }
            // sign check: a negative value cast to unsigned may round-trip but be wrong
            if constexpr (std::signed_integral<T> && std::unsigned_integral<U>) {
                if (m_v < T{0}) {
                    return std::nullopt;
                }
            }
            if constexpr (std::unsigned_integral<T> && std::signed_integral<U>) {
                if (casted < U{0}) {
                    return std::nullopt;
                }
            }
            return Num<U>{casted};
        }

        // Clamps value to target type's range before casting.
        template<Number U>
        [[nodiscard]] constexpr auto saturating_as() const noexcept -> Num<U> {
            if constexpr (std::same_as<T, U>) {
                return *this;
            } else if constexpr (Floating<T> && Integral<U>) {
                // float -> int: clamp to int range
                if (std::isnan(m_v)) {
                    return Num<U>{U{0}};
                }
                using W = long double;
                const auto wide = static_cast<W>(m_v);
                if (wide <= static_cast<W>(std::numeric_limits<U>::min())) {
                    return Num<U>{std::numeric_limits<U>::min()};
                }
                if (wide >= static_cast<W>(std::numeric_limits<U>::max())) {
                    return Num<U>{std::numeric_limits<U>::max()};
                }
                return Num<U>{static_cast<U>(m_v)};
            } else if constexpr (std::signed_integral<T> && std::unsigned_integral<U>) {
                if (m_v < T{0}) {
                    return Num<U>{U{0}};
                }
                auto u = static_cast<std::make_unsigned_t<T>>(m_v);
                if (u > std::numeric_limits<U>::max()) {
                    return Num<U>{std::numeric_limits<U>::max()};
                }
                return Num<U>{static_cast<U>(u)};
            } else if constexpr (std::unsigned_integral<T> && std::signed_integral<U>) {
                if (m_v > static_cast<std::make_unsigned_t<U>>(std::numeric_limits<U>::max())) {
                    return Num<U>{std::numeric_limits<U>::max()};
                }
                return Num<U>{static_cast<U>(m_v)};
            } else {
                // same signedness integers, or float->float
                if (m_v < std::numeric_limits<U>::lowest()) {
                    return Num<U>{std::numeric_limits<U>::lowest()};
                }
                if (m_v > std::numeric_limits<U>::max()) {
                    return Num<U>{std::numeric_limits<U>::max()};
                }
                return Num<U>{static_cast<U>(m_v)};
            }
        }

        [[nodiscard]] constexpr auto write_to(
            char *buf, std::size_t len
        ) const noexcept -> std::expected<char *, std::errc> {
            if constexpr (Integral<T>) {
                const auto [ptr, ec] = std::to_chars(buf, buf + len, m_v);
                if (ec != std::errc{}) {
                    return std::unexpected{ec};
                }
                return ptr;
            } else {
                using Raw = std::conditional_t<sizeof(T) <= 4, float, double>;
                const auto [ptr, ec] = std::to_chars(
                    buf, buf + len,
                    static_cast<Raw>(m_v),
                    std::chars_format::general,
                    std::numeric_limits<T>::max_digits10
                );
                if (ec != std::errc{}) {
                    return std::unexpected{ec};
                }
                return ptr;
            }
        }

        [[nodiscard]] static constexpr auto parse(
            std::string_view sv
        ) noexcept -> std::expected<std::pair<Num, const char *>, std::errc> {
            if (sv.empty()) {
                return std::unexpected{std::errc::invalid_argument};
            }

            T val{};
            if constexpr (Integral<T>) {
                const auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
                if (ec != std::errc{}) {
                    return std::unexpected{ec};
                }
                return std::pair{Num{val}, ptr};
            } else {
                using Raw = std::conditional_t<sizeof(T) <= 4, float, double>;
                Raw raw{};
                const auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), raw);
                if (ec != std::errc{}) {
                    return std::unexpected{ec};
                }
                return std::pair{Num{static_cast<T>(raw)}, ptr};
            }
        }

        [[nodiscard]] constexpr auto to_string() const -> std::string {
            std::array<char, 64> buf{};
            const auto res = write_to(buf.data(), buf.size());
            if (!res) {
                return std::string{};
            }
            return std::string(buf.data(), *res);
        }

        [[nodiscard]] static constexpr auto from_string(std::string_view sv) noexcept -> std::expected<Num, std::errc> {
            const auto res = parse(sv);
            if (!res) {
                return std::unexpected{res.error()};
            }
            const auto [num, ptr] = *res;
            if (ptr != sv.data() + sv.size()) {
                return std::unexpected{std::errc::invalid_argument};
            }
            return num;
        }

        /* ========== LIMITS ========== */

        [[nodiscard]] static constexpr auto MIN_VAL() noexcept -> Num { return Num{std::numeric_limits<T>::lowest()}; }
        [[nodiscard]] static constexpr auto MAX_VAL() noexcept -> Num { return Num{std::numeric_limits<T>::max()}; }
        [[nodiscard]] static constexpr auto EPS_VAL() noexcept -> Num { return Num{std::numeric_limits<T>::epsilon()}; }

        [[nodiscard]] static constexpr auto INF_VAL() noexcept -> Num {
            return Num{std::numeric_limits<T>::infinity()};
        }

        [[nodiscard]] static constexpr auto NAN_VAL() noexcept -> Num {
            return Num{std::numeric_limits<T>::quiet_NaN()};
        }

        /* ========== COMMON MATH ========== */

        [[nodiscard]] constexpr auto abs() const noexcept -> Num {
            if constexpr (std::unsigned_integral<T>) {
                return *this;
            } else if constexpr (Floating<T>) {
                return Num{static_cast<T>(std::abs(m_v))};
            } else {
                // signed integral: use unsigned negate to avoid UB on MIN_VAL
                if (m_v < T{0}) {
                    using U = std::make_unsigned_t<T>;
                    return Num{static_cast<T>(static_cast<U>(0) - static_cast<U>(m_v))};
                }
                return *this;
            }
        }

        [[nodiscard]] constexpr auto min(Num other) const noexcept -> Num {
            return m_v < other.m_v ? *this : other;
        }

        [[nodiscard]] constexpr auto max(Num other) const noexcept -> Num {
            return m_v < other.m_v ? other : *this;
        }

        [[nodiscard]] constexpr auto abs_diff(Num other) const noexcept {
            if constexpr (std::unsigned_integral<T>) {
                return m_v >= other.m_v
                           ? Num{static_cast<T>(m_v - other.m_v)}
                           : Num{static_cast<T>(other.m_v - m_v)};
            } else if constexpr (std::signed_integral<T>) {
                using U = std::make_unsigned_t<T>;
                const auto a = static_cast<U>(m_v);
                const auto b = static_cast<U>(other.m_v);
                return m_v >= other.m_v
                           ? Num<U>{static_cast<U>(a - b)}
                           : Num<U>{static_cast<U>(b - a)};
            } else {
                return Num{static_cast<T>(std::abs(m_v - other.m_v))};
            }
        }

        [[nodiscard]] constexpr auto clamp(Num lo, Num hi) const noexcept -> Num {
            return this->max(lo).min(hi);
        }

        [[nodiscard]] constexpr auto gcd(Num other) const noexcept -> Num requires Integral<T> {
            return Num{static_cast<T>(std::gcd(m_v, other.m_v))};
        }

        [[nodiscard]] constexpr auto lcm(Num other) const noexcept -> Num requires Integral<T> {
            return Num{static_cast<T>(std::lcm(m_v, other.m_v))};
        }

        [[nodiscard]] constexpr auto midpoint(Num other) const noexcept -> Num {
            return Num{std::midpoint(m_v, other.m_v)};
        }

        /* ========== POWERS & ROOTS ========== */

        [[nodiscard]] constexpr auto sqrt() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::sqrt(m_v))};
        }

        [[nodiscard]] constexpr auto cbrt() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::cbrt(m_v))};
        }

        template<Floating U>
        [[nodiscard]] constexpr auto pow(Num<U> e) const -> Num<std::common_type_t<T, U> > requires Floating<T> {
            using R = std::common_type_t<T, U>;
            return Num<R>{static_cast<R>(std::pow(this->as_raw<R>(), e.template as_raw<R>()))};
        }

        /* ========== EXPONENTIAL & LOGARITHMIC ========== */

        [[nodiscard]] constexpr auto exp() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::exp(m_v))};
        }

        [[nodiscard]] constexpr auto exp2() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::exp2(m_v))};
        }

        [[nodiscard]] constexpr auto expm1() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::expm1(m_v))};
        }

        [[nodiscard]] constexpr auto log() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::log(m_v))};
        }

        [[nodiscard]] constexpr auto log2() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::log2(m_v))};
        }

        [[nodiscard]] constexpr auto log10() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::log10(m_v))};
        }

        [[nodiscard]] constexpr auto log1p() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::log1p(m_v))};
        }

        /* ========== TRIGONOMETRY ========== */

        [[nodiscard]] constexpr auto sin() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::sin(m_v))};
        }

        [[nodiscard]] constexpr auto cos() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::cos(m_v))};
        }

        [[nodiscard]] constexpr auto tan() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::tan(m_v))};
        }

        [[nodiscard]] constexpr auto asin() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::asin(m_v))};
        }

        [[nodiscard]] constexpr auto acos() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::acos(m_v))};
        }

        [[nodiscard]] constexpr auto atan() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::atan(m_v))};
        }

        template<Floating U>
        [[nodiscard]] constexpr auto atan2(Num<U> x) const -> Num<std::common_type_t<T, U> > requires Floating<T> {
            using R = std::common_type_t<T, U>;
            return Num<R>{static_cast<R>(std::atan2(this->as_raw<R>(), x.template as_raw<R>()))};
        }

        template<Floating U>
        [[nodiscard]] constexpr auto hypot(Num<U> x) const -> Num<std::common_type_t<T, U> > {
            using R = std::common_type_t<T, U>;
            return Num<R>{static_cast<R>(std::hypot(this->as_raw<R>(), x.template as_raw<R>()))};
        }

        /* ========== HYPERBOLIC ========== */

        [[nodiscard]] constexpr auto sinh() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::sinh(m_v))};
        }

        [[nodiscard]] constexpr auto cosh() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::cosh(m_v))};
        }

        [[nodiscard]] constexpr auto tanh() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::tanh(m_v))};
        }

        [[nodiscard]] constexpr auto asinh() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::asinh(m_v))};
        }

        [[nodiscard]] constexpr auto acosh() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::acosh(m_v))};
        }

        [[nodiscard]] constexpr auto atanh() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::atanh(m_v))};
        }

        /* ========== SPECIAL FUNCTIONS ========== */

        [[nodiscard]] constexpr auto erf() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::erf(m_v))};
        }

        [[nodiscard]] constexpr auto erfc() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::erfc(m_v))};
        }

        [[nodiscard]] constexpr auto tgamma() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::tgamma(m_v))};
        }

        [[nodiscard]] constexpr auto lgamma() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::lgamma(m_v))};
        }

        /* ========== FLOAT UTILS ========== */

        [[nodiscard]] constexpr auto copysign(Num other) const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::copysign(m_v, other.m_v))};
        }

        [[nodiscard]] constexpr auto fmod(Num other) const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::fmod(m_v, other.m_v))};
        }

        [[nodiscard]] constexpr auto remainder(Num other) const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::remainder(m_v, other.m_v))};
        }

        [[nodiscard]] constexpr auto fma(Num a, Num b) const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::fma(m_v, a.m_v, b.m_v))};
        }

        [[nodiscard]] constexpr auto nextafter(Num other) const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::nextafter(m_v, other.m_v))};
        }

        [[nodiscard]] constexpr auto to_degrees() const noexcept -> Num requires Floating<T> {
            return Num{static_cast<T>(m_v * (T{180} / std::numbers::pi_v<T>))};
        }

        [[nodiscard]] constexpr auto to_radians() const noexcept -> Num requires Floating<T> {
            return Num{static_cast<T>(m_v * (std::numbers::pi_v<T> / T{180}))};
        }

        /* ========== ROUNDING ========== */

        [[nodiscard]] constexpr auto floor() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::floor(m_v))};
        }

        [[nodiscard]] constexpr auto ceil() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::ceil(m_v))};
        }

        [[nodiscard]] constexpr auto round() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::round(m_v))};
        }

        [[nodiscard]] constexpr auto trunc() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::trunc(m_v))};
        }

        [[nodiscard]] constexpr auto fract() const -> Num requires Floating<T> {
            return Num{static_cast<T>(m_v - std::trunc(m_v))};
        }

        /* ========== PREDICATES ========== */

        [[nodiscard]] constexpr auto is_nan() const noexcept -> bool requires Floating<T> {
            return std::isnan(m_v);
        }

        [[nodiscard]] constexpr auto is_finite() const noexcept -> bool requires Floating<T> {
            return std::isfinite(m_v);
        }

        [[nodiscard]] constexpr auto is_inf() const noexcept -> bool requires Floating<T> {
            return std::isinf(m_v);
        }

        [[nodiscard]] constexpr auto is_positive() const noexcept -> bool {
            return m_v > T{0};
        }

        [[nodiscard]] constexpr auto is_negative() const noexcept -> bool {
            if constexpr (std::unsigned_integral<T>) {
                return false;
            } else {
                return m_v < T{0};
            }
        }

        [[nodiscard]] constexpr auto signum() const noexcept -> Num {
            if constexpr (Floating<T>) {
                if (std::isnan(m_v)) {
                    return Num{std::numeric_limits<T>::quiet_NaN()};
                }
            }
            if (m_v > T{0}) {
                return Num{T{1}};
            }
            if constexpr (std::unsigned_integral<T>) {
                return Num{T{0}};
            } else {
                return m_v < T{0} ? Num{T{-1}} : Num{T{0}};
            }
        }

        /* ========== UNARY ========== */

        [[nodiscard]] constexpr auto operator+() const noexcept -> Num { return *this; }

        [[nodiscard]] constexpr auto operator-() const noexcept -> Num {
            if constexpr (std::signed_integral<T>) {
                // unsigned negate to avoid UB on MIN_VAL
                using U = std::make_unsigned_t<T>;
                return Num{static_cast<T>(static_cast<U>(0) - static_cast<U>(m_v))};
            } else {
                return Num{static_cast<T>(-m_v)};
            }
        }

        /* ========== INCR/DECR ========== */

        constexpr auto operator++() noexcept -> Num & {
            ++m_v;
            return *this;
        }

        constexpr auto operator++(int) noexcept -> Num {
            const Num tmp{*this};
            ++*this;
            return tmp;
        }

        constexpr auto operator--() noexcept -> Num & {
            --m_v;
            return *this;
        }

        constexpr auto operator--(int) noexcept -> Num {
            const Num tmp{*this};
            --*this;
            return tmp;
        }

        /* ========== COMPOUND ASSIGNMENTS ========== */

        constexpr auto operator+=(Num rhs) noexcept -> Num & {
            m_v += rhs.m_v;
            return *this;
        }

        constexpr auto operator-=(Num rhs) noexcept -> Num & {
            m_v -= rhs.m_v;
            return *this;
        }

        constexpr auto operator*=(Num rhs) noexcept -> Num & {
            m_v *= rhs.m_v;
            return *this;
        }

        constexpr auto operator/=(Num rhs) noexcept -> Num & {
            m_v /= rhs.m_v;
            return *this;
        }

        constexpr auto operator%=(Num rhs) noexcept -> Num & requires Integral<T> {
            m_v %= rhs.m_v;
            return *this;
        }

        /* ========== BITWISE OPERATORS ========== */

        constexpr auto operator&=(Num rhs) noexcept -> Num & requires Integral<T> {
            m_v &= rhs.m_v;
            return *this;
        }

        constexpr auto operator|=(Num rhs) noexcept -> Num & requires Integral<T> {
            m_v |= rhs.m_v;
            return *this;
        }

        constexpr auto operator^=(Num rhs) noexcept -> Num & requires Integral<T> {
            m_v ^= rhs.m_v;
            return *this;
        }

        [[nodiscard]] constexpr auto operator~() const noexcept -> Num requires Integral<T> {
            return Num{static_cast<T>(~m_v)};
        }

        constexpr auto operator<<=(std::uint32_t shift) noexcept -> Num & requires Integral<T> {
            m_v <<= shift;
            return *this;
        }

        constexpr auto operator>>=(std::uint32_t shift) noexcept -> Num & requires Integral<T> {
            m_v >>= shift;
            return *this;
        }

        /* ========== BITWISE INTRINSICS ========== */

        [[nodiscard]] constexpr auto count_ones() const noexcept
            -> std::uint32_t requires Integral<T> {
            return static_cast<std::uint32_t>(std::popcount(static_cast<std::make_unsigned_t<T>>(m_v)));
        }

        [[nodiscard]] constexpr auto count_zeros() const noexcept
            -> std::uint32_t requires Integral<T> {
            return static_cast<std::uint32_t>(sizeof(T) * 8u) - count_ones();
        }

        [[nodiscard]] constexpr auto leading_zeros() const noexcept
            -> std::uint32_t requires Integral<T> {
            return static_cast<std::uint32_t>(std::countl_zero(static_cast<std::make_unsigned_t<T>>(m_v)));
        }

        [[nodiscard]] constexpr auto leading_ones() const noexcept
            -> std::uint32_t requires Integral<T> {
            return static_cast<std::uint32_t>(std::countl_one(static_cast<std::make_unsigned_t<T>>(m_v)));
        }

        [[nodiscard]] constexpr auto trailing_zeros() const noexcept
            -> std::uint32_t requires Integral<T> {
            if (m_v == T{0}) {
                return static_cast<std::uint32_t>(sizeof(T) * 8u);
            }
            return static_cast<std::uint32_t>(std::countr_zero(static_cast<std::make_unsigned_t<T>>(m_v)));
        }

        [[nodiscard]] constexpr auto trailing_ones() const noexcept
            -> std::uint32_t requires Integral<T> {
            return static_cast<std::uint32_t>(std::countr_one(static_cast<std::make_unsigned_t<T>>(m_v)));
        }

        [[nodiscard]] constexpr auto rotate_left(std::uint32_t n) const noexcept
            -> Num requires Integral<T> {
            return Num{static_cast<T>(std::rotl(static_cast<std::make_unsigned_t<T>>(m_v), static_cast<int>(n)))};
        }

        [[nodiscard]] constexpr auto rotate_right(std::uint32_t n) const noexcept
            -> Num requires Integral<T> {
            return Num{static_cast<T>(std::rotr(static_cast<std::make_unsigned_t<T>>(m_v), static_cast<int>(n)))};
        }

        [[nodiscard]] constexpr auto reverse_bits() const noexcept
            -> Num requires Integral<T> {
            using U = std::make_unsigned_t<T>;
            U v = static_cast<U>(m_v);
            U result = 0;
            for (std::uint32_t i = 0; i < sizeof(T) * 8u; ++i) {
                result = static_cast<U>((result << 1u) | (v & U{1}));
                v >>= 1u;
            }
            return Num{static_cast<T>(result)};
        }

        /* ========== OVERFLOWING ARITHMETIC (base layer) ========== */

        [[nodiscard]] constexpr auto overflowing_add(Num rhs) const noexcept
            -> std::pair<Num, bool> requires Integral<T> {
            using U = std::make_unsigned_t<T>;
            const auto wrapped = static_cast<T>(static_cast<U>(m_v) + static_cast<U>(rhs.m_v));
            bool ov;
            if constexpr (std::unsigned_integral<T>) {
                ov = wrapped < m_v;
            } else {
                ov = (rhs.m_v > T{0} && wrapped < m_v) || (rhs.m_v < T{0} && wrapped > m_v);
            }
            return {Num{wrapped}, ov};
        }

        [[nodiscard]] constexpr auto overflowing_sub(Num rhs) const noexcept
            -> std::pair<Num, bool> requires Integral<T> {
            using U = std::make_unsigned_t<T>;
            const auto wrapped = static_cast<T>(static_cast<U>(m_v) - static_cast<U>(rhs.m_v));
            bool ov;
            if constexpr (std::unsigned_integral<T>) {
                ov = m_v < rhs.m_v;
            } else {
                ov = (rhs.m_v > T{0} && wrapped > m_v) || (rhs.m_v < T{0} && wrapped < m_v);
            }
            return {Num{wrapped}, ov};
        }

        [[nodiscard]] constexpr auto overflowing_mul(Num rhs) const noexcept
            -> std::pair<Num, bool> requires Integral<T> {
            using U = std::make_unsigned_t<T>;
            const auto wrapped = static_cast<T>(static_cast<U>(m_v) * static_cast<U>(rhs.m_v));
            if (m_v == T{0} || rhs.m_v == T{0}) {
                return {Num{T{0}}, false};
            }
            if constexpr (std::signed_integral<T>) {
                // handle -1 cases separately: back-division with -1 can itself be UB
                if (m_v == T{-1}) {
                    return {Num{wrapped}, rhs.m_v == std::numeric_limits<T>::min()};
                }
                if (rhs.m_v == T{-1}) {
                    return {Num{wrapped}, m_v == std::numeric_limits<T>::min()};
                }
            }
            // overflow iff back-division doesn't round-trip
            return {Num{wrapped}, wrapped / m_v != rhs.m_v};
        }

        [[nodiscard]] constexpr auto overflowing_div(Num rhs) const noexcept
            -> std::pair<Num, bool> requires Integral<T> {
            if constexpr (std::signed_integral<T>) {
                if (m_v == std::numeric_limits<T>::min() && rhs.m_v == T{-1}) {
                    return {Num{std::numeric_limits<T>::min()}, true};
                }
            }
            return {Num{static_cast<T>(m_v / rhs.m_v)}, false};
        }

        [[nodiscard]] constexpr auto overflowing_rem(Num rhs) const noexcept
            -> std::pair<Num, bool> requires Integral<T> {
            if constexpr (std::signed_integral<T>) {
                if (m_v == std::numeric_limits<T>::min() && rhs.m_v == T{-1}) {
                    return {Num{T{0}}, true};
                }
            }
            return {Num{static_cast<T>(m_v % rhs.m_v)}, false};
        }

        [[nodiscard]] constexpr auto overflowing_neg() const noexcept
            -> std::pair<Num, bool> requires Integral<T> {
            using U = std::make_unsigned_t<T>;
            const auto wrapped = Num{static_cast<T>(static_cast<U>(0) - static_cast<U>(m_v))};
            if constexpr (std::signed_integral<T>) {
                return {wrapped, m_v == std::numeric_limits<T>::min()};
            } else {
                return {wrapped, m_v != T{0}};
            }
        }

        [[nodiscard]] constexpr auto overflowing_abs() const noexcept
            -> std::pair<Num, bool> requires std::signed_integral<T> {
            if (m_v == std::numeric_limits<T>::min()) {
                return {Num{std::numeric_limits<T>::min()}, true};
            }
            return {Num{static_cast<T>(m_v < T{0} ? -m_v : m_v)}, false};
        }

        [[nodiscard]] constexpr auto overflowing_pow(std::uint32_t exp) const noexcept
            -> std::pair<Num, bool> requires Integral<T> {
            Num base{*this};
            Num res{T{1}};
            bool ov = false;

            while (exp > 0u) {
                if (exp & 1u) {
                    const auto [r, o] = res.overflowing_mul(base);
                    res = r;
                    ov = ov || o;
                }
                exp >>= 1u;
                if (exp > 0u) {
                    const auto [b, o] = base.overflowing_mul(base);
                    base = b;
                    ov = ov || o;
                }
            }
            return {res, ov};
        }

        [[nodiscard]] constexpr auto overflowing_shl(std::uint32_t shift) const noexcept
            -> std::pair<Num, bool> requires Integral<T> {
            constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u);
            return {Num{static_cast<T>(m_v << (shift % bits))}, shift >= bits};
        }

        [[nodiscard]] constexpr auto overflowing_shr(std::uint32_t shift) const noexcept
            -> std::pair<Num, bool> requires Integral<T> {
            constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u);
            return {Num{static_cast<T>(m_v >> (shift % bits))}, shift >= bits};
        }

        /* ========== WRAPPING ARITHMETIC ========== */

        [[nodiscard]] constexpr auto wrapping_add(Num rhs) const noexcept -> Num requires Integral<T> {
            return overflowing_add(rhs).first;
        }

        [[nodiscard]] constexpr auto wrapping_sub(Num rhs) const noexcept -> Num requires Integral<T> {
            return overflowing_sub(rhs).first;
        }

        [[nodiscard]] constexpr auto wrapping_mul(Num rhs) const noexcept
            -> Num requires Integral<T> {
            return overflowing_mul(rhs).first;
        }

        [[nodiscard]] constexpr auto wrapping_div(Num rhs) const noexcept
            -> Num requires Integral<T> {
            return overflowing_div(rhs).first;
        }

        [[nodiscard]] constexpr auto wrapping_rem(Num rhs) const noexcept
            -> Num requires Integral<T> {
            return overflowing_rem(rhs).first;
        }

        [[nodiscard]] constexpr auto wrapping_neg() const noexcept
            -> Num requires Integral<T> {
            return overflowing_neg().first;
        }

        [[nodiscard]] constexpr auto wrapping_abs() const noexcept
            -> Num requires std::signed_integral<T> {
            return overflowing_abs().first;
        }

        [[nodiscard]] constexpr auto wrapping_pow(std::uint32_t exp) const noexcept
            -> Num requires Integral<T> {
            return overflowing_pow(exp).first;
        }

        [[nodiscard]] constexpr auto wrapping_shl(std::uint32_t shift) const noexcept
            -> Num requires Integral<T> {
            return overflowing_shl(shift).first;
        }

        [[nodiscard]] constexpr auto wrapping_shr(std::uint32_t shift) const noexcept
            -> Num requires Integral<T> {
            return overflowing_shr(shift).first;
        }

        /* ========== CHECKED ARITHMETIC ========== */

        [[nodiscard]] constexpr auto checked_add(Num rhs) const noexcept
            -> std::optional<Num> requires Integral<T> {
            const auto [res, ov] = overflowing_add(rhs);
            return ov ? std::nullopt : std::optional{res};
        }

        [[nodiscard]] constexpr auto checked_sub(Num rhs) const noexcept
            -> std::optional<Num> requires Integral<T> {
            const auto [res, ov] = overflowing_sub(rhs);
            return ov ? std::nullopt : std::optional{res};
        }

        [[nodiscard]] constexpr auto checked_mul(Num rhs) const noexcept
            -> std::optional<Num> requires Integral<T> {
            const auto [res, ov] = overflowing_mul(rhs);
            return ov ? std::nullopt : std::optional{res};
        }

        [[nodiscard]] constexpr auto checked_div(Num rhs) const noexcept
            -> std::optional<Num> requires Integral<T> {
            if (rhs.m_v == T{0}) {
                return std::nullopt;
            }
            const auto [res, ov] = overflowing_div(rhs);
            return ov ? std::nullopt : std::optional{res};
        }

        [[nodiscard]] constexpr auto checked_rem(Num rhs) const noexcept
            -> std::optional<Num> requires Integral<T> {
            if (rhs.m_v == T{0}) {
                return std::nullopt;
            }
            const auto [res, ov] = overflowing_rem(rhs);
            return ov ? std::nullopt : std::optional{res};
        }

        [[nodiscard]] constexpr auto checked_neg() const noexcept
            -> std::optional<Num> requires std::signed_integral<T> {
            const auto [res, ov] = overflowing_neg();
            return ov ? std::nullopt : std::optional{res};
        }

        [[nodiscard]] constexpr auto checked_abs() const noexcept
            -> std::optional<Num> requires std::signed_integral<T> {
            const auto [res, ov] = overflowing_abs();
            return ov ? std::nullopt : std::optional{res};
        }

        [[nodiscard]] constexpr auto checked_pow(std::uint32_t exp) const noexcept
            -> std::optional<Num> requires Integral<T> {
            const auto [res, ov] = overflowing_pow(exp);
            return ov ? std::nullopt : std::optional{res};
        }

        [[nodiscard]] constexpr auto checked_shl(std::uint32_t shift) const noexcept
            -> std::optional<Num> requires Integral<T> {
            const auto [res, ov] = overflowing_shl(shift);
            return ov ? std::nullopt : std::optional{res};
        }

        [[nodiscard]] constexpr auto checked_shr(std::uint32_t shift) const noexcept
            -> std::optional<Num> requires Integral<T> {
            const auto [res, ov] = overflowing_shr(shift);
            return ov ? std::nullopt : std::optional{res};
        }

        /* ========== SATURATING ARITHMETIC ========== */

        [[nodiscard]] constexpr auto saturating_add(Num rhs) const noexcept
            -> Num requires Integral<T> {
            const auto [res, ov] = overflowing_add(rhs);
            if (!ov) {
                return res;
            }
            if constexpr (std::unsigned_integral<T>) {
                return MAX_VAL();
            } else {
                return rhs.m_v > T{0} ? MAX_VAL() : MIN_VAL();
            }
        }

        [[nodiscard]] constexpr auto saturating_sub(Num rhs) const noexcept
            -> Num requires Integral<T> {
            const auto [res, ov] = overflowing_sub(rhs);
            if (!ov) {
                return res;
            }
            if constexpr (std::unsigned_integral<T>) {
                return MIN_VAL();
            } else {
                return rhs.m_v > T{0} ? MIN_VAL() : MAX_VAL();
            }
        }

        [[nodiscard]] constexpr auto saturating_mul(Num rhs) const noexcept
            -> Num requires Integral<T> {
            const auto [res, ov] = overflowing_mul(rhs);
            if (!ov) {
                return res;
            }
            if constexpr (std::unsigned_integral<T>) {
                return MAX_VAL();
            } else {
                return (m_v > T{0}) == (rhs.m_v > T{0}) ? MAX_VAL() : MIN_VAL();
            }
        }

        [[nodiscard]] constexpr auto saturating_div(Num rhs) const noexcept
            -> Num requires Integral<T> {
            const auto [res, ov] = overflowing_div(rhs);
            return ov ? MAX_VAL() : res;
        }

        [[nodiscard]] constexpr auto saturating_neg() const noexcept
            -> Num requires std::signed_integral<T> {
            const auto [res, ov] = overflowing_neg();
            return ov ? MAX_VAL() : res;
        }

        [[nodiscard]] constexpr auto saturating_abs() const noexcept
            -> Num requires std::signed_integral<T> {
            const auto [res, ov] = overflowing_abs();
            return ov ? MAX_VAL() : res;
        }

        [[nodiscard]] constexpr auto saturating_pow(std::uint32_t exp) const noexcept
            -> Num requires Integral<T> {
            const auto [res, ov] = overflowing_pow(exp);
            if (!ov) {
                return res;
            }
            if constexpr (std::unsigned_integral<T>) {
                return MAX_VAL();
            } else {
                const bool result_negative = m_v < T{0} && (exp & 1u);
                return result_negative ? MIN_VAL() : MAX_VAL();
            }
        }

        /* ========== COMPARISON ========== */

        constexpr auto operator==(const Num &) const noexcept -> bool = default;

        constexpr auto operator<=>(const Num &) const noexcept = default;

    private:
        T m_v{};
    };

    /* ========== NUM CONCEPTS ========== */

    template<typename T>
    concept IsNum = requires { typename T::value_type; } && std::same_as<T, Num<typename T::value_type> >;

    template<typename T>
    concept IsIntNum = IsNum<T> && Integral<typename T::value_type>;

    template<typename T>
    concept IsSignedNum = IsNum<T> && std::signed_integral<typename T::value_type>;

    template<typename T>
    concept IsUnsignedNum = IsNum<T> && std::unsigned_integral<typename T::value_type>;

    template<typename T>
    concept IsFloatNum = IsNum<T> && Floating<typename T::value_type>;

    /* ========== Num op Num ========== */

    template<Number T>
    constexpr auto operator+(Num<T> lhs, Num<T> rhs) noexcept -> Num<T> {
        lhs += rhs;
        return lhs;
    }

    template<Number T>
    constexpr auto operator-(Num<T> lhs, Num<T> rhs) noexcept -> Num<T> {
        lhs -= rhs;
        return lhs;
    }

    template<Number T>
    constexpr auto operator*(Num<T> lhs, Num<T> rhs) noexcept -> Num<T> {
        lhs *= rhs;
        return lhs;
    }

    template<Number T>
    constexpr auto operator/(Num<T> lhs, Num<T> rhs) noexcept -> Num<T> {
        lhs /= rhs;
        return lhs;
    }

    template<Integral T>
    constexpr auto operator%(Num<T> lhs, Num<T> rhs) noexcept -> Num<T> {
        lhs %= rhs;
        return lhs;
    }

    /* ========== Num bitwise Num ========== */

    template<Integral T>
    constexpr auto operator&(Num<T> lhs, Num<T> rhs) noexcept -> Num<T> {
        lhs &= rhs;
        return lhs;
    }

    template<Integral T>
    constexpr auto operator|(Num<T> lhs, Num<T> rhs) noexcept -> Num<T> {
        lhs |= rhs;
        return lhs;
    }

    template<Integral T>
    constexpr auto operator^(Num<T> lhs, Num<T> rhs) noexcept -> Num<T> {
        lhs ^= rhs;
        return lhs;
    }

    template<Integral T>
    constexpr auto operator<<(Num<T> lhs, std::uint32_t shift) noexcept -> Num<T> {
        lhs <<= shift;
        return lhs;
    }

    template<Integral T>
    constexpr auto operator>>(Num<T> lhs, std::uint32_t shift) noexcept -> Num<T> {
        lhs >>= shift;
        return lhs;
    }

    /* ========== STREAMS ========== */

    template<Number T>
    auto operator<<(std::ostream &os, const Num<T> &x) -> std::ostream & {
        std::array<char, 64> buf{};
        if (auto res = x.write_to(buf.data(), buf.size())) {
            os.write(buf.data(), res.value() - buf.data());
        } else {
            os.setstate(std::ios::failbit);
        }
        return os;
    }

    template<Number T>
    auto operator>>(std::istream &is, Num<T> &x) -> std::istream & {
        std::array<char, 64> buf{};
        is >> std::ws;

        std::size_t len = 0;
        while (len < buf.size() - 1) {
            if (const auto ch = is.peek(); ch == std::istream::traits_type::eof() || std::isspace(ch)) {
                break;
            }
            buf[len++] = static_cast<char>(is.get());
        }

        auto res = Num<T>::parse(std::string_view{buf.data(), len});
        if (!res) {
            is.setstate(std::ios::failbit);
            return is;
        }
        auto [num, ptr] = *res;
        if (ptr != buf.data() + len) {
            is.setstate(std::ios::failbit);
            return is;
        }
        x = num;
        return is;
    }

    /* ========== ALIASES ========== */

    using i8 = Num<std::int8_t>;
    using i16 = Num<std::int16_t>;
    using i32 = Num<std::int32_t>;
    using i64 = Num<std::int64_t>;

    using u8 = Num<std::uint8_t>;
    using u16 = Num<std::uint16_t>;
    using u32 = Num<std::uint32_t>;
    using u64 = Num<std::uint64_t>;

    using isize = Num<std::ptrdiff_t>;
    using usize = Num<std::size_t>;

    using f32 = Num<float>;
    using f64 = Num<double>;
}

/* ========== std::hash ========== */

template<cn::Number T>
struct std::hash<cn::Num<T> > {
    constexpr std::size_t operator()(cn::Num<T> n) const noexcept {
        return std::hash<T>{}(n.value());
    }
};

/* ========== std::formatter ========== */

template<cn::Number T>
struct std::formatter<cn::Num<T> > : std::formatter<T> {
    auto format(cn::Num<T> n, auto &ctx) const {
        return std::formatter<T>::format(n.value(), ctx);
    }
};

/* ========== LITERALS ========== */

namespace cn::literals {
    constexpr i8 operator""_i8(unsigned long long v) noexcept { return i8{static_cast<std::int8_t>(v)}; }
    constexpr i16 operator""_i16(unsigned long long v) noexcept { return i16{static_cast<std::int16_t>(v)}; }
    constexpr i32 operator""_i32(unsigned long long v) noexcept { return i32{static_cast<std::int32_t>(v)}; }
    constexpr i64 operator""_i64(unsigned long long v) noexcept { return i64{static_cast<std::int64_t>(v)}; }

    constexpr u8 operator""_u8(unsigned long long v) noexcept { return u8{static_cast<std::uint8_t>(v)}; }
    constexpr u16 operator""_u16(unsigned long long v) noexcept { return u16{static_cast<std::uint16_t>(v)}; }
    constexpr u32 operator""_u32(unsigned long long v) noexcept { return u32{static_cast<std::uint32_t>(v)}; }
    constexpr u64 operator""_u64(unsigned long long v) noexcept { return u64{static_cast<std::uint64_t>(v)}; }

    constexpr isize operator""_isize(unsigned long long v) noexcept { return isize{static_cast<std::ptrdiff_t>(v)}; }
    constexpr usize operator""_usize(unsigned long long v) noexcept { return usize{static_cast<std::size_t>(v)}; }

    constexpr f32 operator""_f32(long double v) noexcept { return f32{static_cast<float>(v)}; }
    constexpr f64 operator""_f64(long double v) noexcept { return f64{static_cast<double>(v)}; }

    constexpr f32 operator""_f32(unsigned long long v) noexcept { return f32{static_cast<float>(v)}; }
    constexpr f64 operator""_f64(unsigned long long v) noexcept { return f64{static_cast<double>(v)}; }
}

/* ========== CONSTS ========== */

namespace cn::consts {
    template<typename T>
    inline constexpr Num<T> e{std::numbers::e_v<T>};

    template<typename T>
    inline constexpr Num<T> log2e{std::numbers::log2e_v<T>};

    template<typename T>
    inline constexpr Num<T> log10e{std::numbers::log10e_v<T>};

    template<typename T>
    inline constexpr Num<T> pi{std::numbers::pi_v<T>};

    template<typename T>
    inline constexpr Num<T> inv_pi{std::numbers::inv_pi_v<T>};

    template<typename T>
    inline constexpr Num<T> inv_sqrtpi{std::numbers::inv_sqrtpi_v<T>};

    template<typename T>
    inline constexpr Num<T> ln2{std::numbers::ln2_v<T>};

    template<typename T>
    inline constexpr Num<T> ln10{std::numbers::ln10_v<T>};

    template<typename T>
    inline constexpr Num<T> sqrt2{std::numbers::sqrt2_v<T>};

    template<typename T>
    inline constexpr Num<T> sqrt3{std::numbers::sqrt3_v<T>};

    template<typename T>
    inline constexpr Num<T> inv_sqrt3{std::numbers::inv_sqrt3_v<T>};

    template<typename T>
    inline constexpr Num<T> egamma{std::numbers::egamma_v<T>};

    template<typename T>
    inline constexpr Num<T> phi{std::numbers::phi_v<T>};

    template<typename T>
    inline constexpr Num<T> tau{std::numbers::pi_v<T> * Num<T>{2}};
}
