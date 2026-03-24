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
#include <charconv>
#include <stdfloat>
#include <numbers>

namespace cn {
    template<typename T>
    concept Number = (std::integral<T> || std::floating_point<T>) && !std::same_as<T, bool>;

    template<typename T>
    concept Integral = std::integral<T> && !std::same_as<T, bool>;

    template<typename T>
    concept Floating = std::floating_point<T>;

    template<Number T>
    class Num {
    public:
        using value_type = T;

        constexpr Num() = default;

        constexpr Num(T v) : m_v{v} {
        }

        [[nodiscard]] constexpr auto value() const noexcept -> T { return m_v; }

        explicit constexpr operator T() const noexcept { return m_v; }

        /* ========== CAST ========== */

        template<Number U>
        constexpr auto as_raw() const noexcept -> U {
            return static_cast<U>(m_v);
        }

        template<typename U>
        constexpr auto as() const noexcept -> std::remove_cvref_t<U> {
            using Out = std::remove_cvref_t<U>;
            return Out{static_cast<Out::value_type>(m_v)};
        }

        constexpr auto write_to(char *buf, std::size_t len) const -> std::expected<char *, std::errc> {
            if constexpr (Integral<T>) {
                auto [ptr, ec] = std::to_chars(buf, buf + len, m_v);
                if (ec != std::errc{}) {
                    return std::unexpected{ec};
                }
                return ptr;
            } else {
                using Raw = std::conditional_t<sizeof(T) <= 4, float, double>;
                auto [ptr, ec] = std::to_chars(
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

        static constexpr auto parse(std::string_view sv) -> std::expected<std::pair<Num, const char *>, std::errc> {
            if (sv.empty()) {
                return std::unexpected{std::errc::invalid_argument};
            }

            T val{};
            if constexpr (Integral<T>) {
                auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
                if (ec != std::errc{}) {
                    return std::unexpected{ec};
                }
                return std::pair{Num{val}, ptr};
            } else {
                using Raw = std::conditional_t<sizeof(T) <= 4, float, double>;
                Raw raw{};
                auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), raw);
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

        static constexpr auto from_string(std::string_view sv) -> std::expected<Num, std::errc> {
            auto res = parse(sv);
            if (!res) {
                return std::unexpected{res.error()};
            }
            auto [num, ptr] = *res;
            if (ptr != sv.data() + sv.size()) {
                return std::unexpected{std::errc::invalid_argument};
            }
            return num;
        }

        /* ========== LIMITS ========== */

        static constexpr Num MIN{std::numeric_limits<T>::lowest()};
        static constexpr Num MAX{std::numeric_limits<T>::max()};

        /* ========== COMMON MATH ========== */

        [[nodiscard]] constexpr auto abs() const noexcept -> Num {
            if constexpr (std::unsigned_integral<T>) {
                return *this;
            } else if constexpr (Floating<T>) {
                return Num{static_cast<T>(std::abs(m_v))};
            } else {
                // signed integral
                return m_v < 0 ? Num{static_cast<T>(-m_v)} : *this;
            }
        }

        [[nodiscard]] constexpr auto min(Num other) const noexcept -> Num { return m_v < other.m_v ? *this : other; }
        [[nodiscard]] constexpr auto max(Num other) const noexcept -> Num { return m_v < other.m_v ? other : *this; }

        [[nodiscard]] constexpr auto clamp(Num lo, Num hi) const noexcept -> Num {
            return this->max(lo).min(hi);
        }

        [[nodiscard]] constexpr auto sqrt() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::sqrt(m_v))};
        }

        [[nodiscard]] constexpr auto cbrt() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::cbrt(m_v))};
        }

        [[nodiscard]] constexpr auto exp() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::exp(m_v))};
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

        [[nodiscard]] constexpr auto pow(T e) const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::pow(m_v, e))};
        }

        template<Floating U>
        [[nodiscard]] constexpr auto pow(Num<U> e) const -> Num<std::common_type_t<T, U> > requires Floating<T> {
            using R = std::common_type_t<T, U>;
            return Num<R>{static_cast<R>(std::pow(this->as<R>(), e.template as<R>()))};
        }

        // trig funcs

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
            return Num<R>{static_cast<R>(std::atan2(this->as<R>(), x.template as<R>()))}; // y.atan2(x)
        }

        template<Floating U>
        [[nodiscard]] constexpr auto hypot(Num<U> x) const -> Num<std::common_type_t<T, U> > {
            using R = std::common_type_t<T, U>;
            return Num<R>{static_cast<R>(std::hypot(this->as<R>(), x.template as<R>()))};
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

        /* ========== UNARY ========== */

        constexpr auto operator+() const noexcept -> Num { return *this; }
        constexpr auto operator-() const noexcept -> Num { return Num{static_cast<T>(-m_v)}; }

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

        /* ========== COMPOUND ASSIGNMENTS (STRICT: SAME T ONLY) ========== */

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

        // primitive same-type only
        constexpr auto operator+=(T rhs) noexcept -> Num & {
            m_v += rhs;
            return *this;
        }

        constexpr auto operator-=(T rhs) noexcept -> Num & {
            m_v -= rhs;
            return *this;
        }

        constexpr auto operator*=(T rhs) noexcept -> Num & {
            m_v *= rhs;
            return *this;
        }

        constexpr auto operator/=(T rhs) noexcept -> Num & {
            m_v /= rhs;
            return *this;
        }

        constexpr auto operator%=(T rhs) noexcept -> Num & requires Integral<T> {
            m_v %= rhs;
            return *this;
        }

        /* ========== CHECKED ARITHMETIC ========== */

        [[nodiscard]] constexpr auto checked_add(Num rhs) const noexcept
            -> std::optional<Num> requires Integral<T> {
            T res{};
            if (add_overflows(m_v, rhs.m_v, res))
                return std::nullopt;
            return Num{res};
        }

        [[nodiscard]] constexpr auto checked_sub(Num rhs) const noexcept
            -> std::optional<Num> requires Integral<T> {
            T res{};
            if (sub_overflows(m_v, rhs.m_v, res))
                return std::nullopt;
            return Num{res};
        }

        [[nodiscard]] constexpr auto checked_mul(Num rhs) const noexcept
            -> std::optional<Num> requires Integral<T> {
            T res{};
            if (mul_overflows(m_v, rhs.m_v, res))
                return std::nullopt;
            return Num{res};
        }

        // Two nullopt cases:
        //   1. division by zero
        //   2. signed MIN / -1  →  result would be MAX+1 (hardware trap on x86)
        [[nodiscard]] constexpr auto checked_div(Num rhs) const noexcept
            -> std::optional<Num> requires Integral<T> {
            if (rhs.m_v == T{0}) {
                return std::nullopt;
            }
            if constexpr (std::signed_integral<T>) {
                if (m_v == std::numeric_limits<T>::min() && rhs.m_v == T{-1}) {
                    return std::nullopt;
                }
            }
            return Num{static_cast<T>(m_v / rhs.m_v)};
        }

        // Same two guards as checked_div.
        [[nodiscard]] constexpr auto checked_rem(Num rhs) const noexcept
            -> std::optional<Num> requires Integral<T> {
            if (rhs.m_v == T{0}) {
                return std::nullopt;
            }
            if constexpr (std::signed_integral<T>) {
                if (m_v == std::numeric_limits<T>::min() && rhs.m_v == T{-1}) {
                    return std::nullopt;
                }
            }
            return Num{static_cast<T>(m_v % rhs.m_v)};
        }

        // Only for signed: unsigned negation is meaningless.
        // nullopt when value == MIN  (−MIN == MAX+1, doesn't fit)
        [[nodiscard]] constexpr auto checked_neg() const noexcept
            -> std::optional<Num> requires std::signed_integral<T> {
            if (m_v == std::numeric_limits<T>::min()) {
                return std::nullopt;
            }
            return Num{static_cast<T>(-m_v)};
        }

        // Only for signed: unsigned abs is identity, adding it would be noise.
        // nullopt when value == MIN  (|MIN| == MAX+1, doesn't fit)
        [[nodiscard]] constexpr auto checked_abs() const noexcept
            -> std::optional<Num> requires std::signed_integral<T> {
            if (m_v == std::numeric_limits<T>::min()) {
                return std::nullopt;
            }
            return Num{static_cast<T>(m_v < T{0} ? -m_v : m_v)};
        }

        // Binary exponentiation with overflow check at every multiplication.
        // Exponent is uint32_t — same as Rust's i32::checked_pow(u32).
        // x.checked_pow(0) == Some(1) for all x, including 0 (0^0 = 1 by convention).
        [[nodiscard]] constexpr auto checked_pow(std::uint32_t exp) const noexcept
            -> std::optional<Num> requires Integral<T> {
            Num base{*this};
            Num res{T{1}};

            while (exp > 0u) {
                if (exp & 1u) {
                    T tmp{};
                    if (mul_overflows(res.m_v, base.m_v, tmp)) {
                        return std::nullopt;
                    }
                    res = Num{tmp};
                }
                exp >>= 1u;
                if (exp > 0u) {
                    // guard the squaring only when we'll use base again
                    T tmp{};
                    if (mul_overflows(base.m_v, base.m_v, tmp)) {
                        return std::nullopt;
                    }
                    base = Num{tmp};
                }
            }
            return res;
        }

        // nullopt when shift >= bit width of T.
        [[nodiscard]] constexpr auto checked_shl(std::uint32_t shift) const noexcept
            -> std::optional<Num> requires Integral<T> {
            if (constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u); shift >= bits) {
                return std::nullopt;
            }
            return Num{static_cast<T>(m_v << shift)};
        }

        [[nodiscard]] constexpr auto checked_shr(std::uint32_t shift) const noexcept
            -> std::optional<Num> requires Integral<T> {
            if (constexpr auto bits = static_cast<std::uint32_t>(sizeof(T) * 8u); shift >= bits) {
                return std::nullopt;
            }
            return Num{static_cast<T>(m_v >> shift)};
        }

        /* ========== COMPARISON ========== */

        constexpr bool operator==(const Num &) const noexcept = default;

        constexpr bool operator!=(const Num &) const noexcept = default;

        constexpr auto operator<=>(const Num &) const noexcept = default;

    private:
        [[nodiscard]] static constexpr auto add_overflows(T a, T b, T &out) noexcept
            -> bool requires Integral<T> {
            if constexpr (std::unsigned_integral<T>) {
                out = static_cast<T>(a + b);
                return out < a; // unsigned wraparound is well-defined, detect by result < a
            } else {
                // signed: check BEFORE performing the operation to avoid UB
                if (
                    (b > T{0} && a > std::numeric_limits<T>::max() - b) ||
                    (b < T{0} && a < std::numeric_limits<T>::min() - b)
                ) {
                    return true;
                }
                out = a + b;
                return false;
            }
        }

        [[nodiscard]] static constexpr auto sub_overflows(T a, T b, T &out) noexcept
            -> bool requires Integral<T> {
            if constexpr (std::unsigned_integral<T>) {
                out = static_cast<T>(a - b);
                return a < b; // unsigned: wraps if a < b
            } else {
                // a - b == a + (-b), but -b can itself overflow if b == MIN
                if (
                    (b < T{0} && a > std::numeric_limits<T>::max() + b) ||
                    (b > T{0} && a < std::numeric_limits<T>::min() + b)
                ) {
                    return true;
                }
                return false;
            }
        }

        [[nodiscard]] static constexpr auto mul_overflows(T a, T b, T &out) noexcept
            -> bool requires Integral<T> {
            if (a == T{0} || b == T{0}) {
                out = T{0};
                return false;
            }

            if constexpr (std::unsigned_integral<T>) {
                // a * b > MAX  <=>  a > MAX / b  (b != 0 already checked)
                if (a > std::numeric_limits<T>::max() / b) {
                    return true;
                }
                out = a * b;
                return false;
            } else {
                // signed: four quadrant checks, all using only / which is safe here
                // because we've already excluded 0, and handle -1 separately
                if (a == T{-1}) {
                    // -1 * b overflows only when b == MIN (result == MAX+1)
                    if (b == std::numeric_limits<T>::min()) {
                        return true;
                    }
                    out = -b;
                    return false;
                }
                if (b == T{-1}) {
                    if (a == std::numeric_limits<T>::min()) {
                        return true;
                    }
                    out = -a;
                    return false;
                }
                // both non-zero, neither is -1 → division is safe
                if (
                    (a > T{0} && b > T{0} && a > std::numeric_limits<T>::max() / b) ||
                    (a < T{0} && b < T{0} && a < std::numeric_limits<T>::max() / b) ||
                    (a > T{0} && b < T{0} && b < std::numeric_limits<T>::min() / a) ||
                    (a < T{0} && b > T{0} && a < std::numeric_limits<T>::min() / b)
                ) {
                    return true;
                }
                out = a * b;
                return false;
            }
        }

        T m_v{};
    };

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

    /* ========== Num op T ========== */

    template<Number T>
    constexpr auto operator+(Num<T> lhs, T rhs) noexcept -> Num<T> {
        lhs += rhs;
        return lhs;
    }

    template<Number T>
    constexpr auto operator-(Num<T> lhs, T rhs) noexcept -> Num<T> {
        lhs -= rhs;
        return lhs;
    }

    template<Number T>
    constexpr auto operator*(Num<T> lhs, T rhs) noexcept -> Num<T> {
        lhs *= rhs;
        return lhs;
    }

    template<Number T>
    constexpr auto operator/(Num<T> lhs, T rhs) noexcept -> Num<T> {
        lhs /= rhs;
        return lhs;
    }

    template<Integral T>
    constexpr auto operator%(Num<T> lhs, T rhs) noexcept -> Num<T> {
        lhs %= rhs;
        return lhs;
    }

    /* ========== T op Num ========== */

    template<Number T>
    constexpr auto operator+(T lhs, Num<T> rhs) noexcept -> Num<T> {
        rhs += lhs;
        return rhs;
    }

    template<Number T>
    constexpr auto operator-(T lhs, Num<T> rhs) noexcept -> Num<T> { return Num<T>{lhs} - rhs; }

    template<Number T>
    constexpr auto operator*(T lhs, Num<T> rhs) noexcept -> Num<T> {
        rhs *= lhs;
        return rhs;
    }

    template<Number T>
    constexpr auto operator/(T lhs, Num<T> rhs) noexcept -> Num<T> { return Num<T>{lhs} / rhs; }

    template<Integral T>
    constexpr auto operator%(T lhs, Num<T> rhs) noexcept -> Num<T> { return Num<T>{lhs} % rhs; }

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

    template<typename T, Number U>
    constexpr auto as(U u) noexcept -> Num<T> {
        return Num<T>{static_cast<T>(u)};
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

    using f32 = Num<std::float32_t>;
    using f64 = Num<std::float64_t>;
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

    constexpr f32 operator""_f32(long double v) noexcept { return f32{static_cast<std::float32_t>(v)}; }
    constexpr f64 operator""_f64(long double v) noexcept { return f64{static_cast<std::float64_t>(v)}; }

    constexpr f32 operator""_f32(unsigned long long v) noexcept { return f32{static_cast<std::float32_t>(v)}; }
    constexpr f64 operator""_f64(unsigned long long v) noexcept { return f64{static_cast<std::float64_t>(v)}; }
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
