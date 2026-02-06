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
#include <sstream>
#include <iomanip>
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

        constexpr T value() const noexcept { return m_v; }

        explicit constexpr operator T() const noexcept { return m_v; }

        /* ---------- CAST ---------- */

        template<Number U>
        constexpr auto as() const noexcept -> U {
            return static_cast<U>(m_v);
        }

        template<typename U>
        constexpr auto as() const noexcept -> std::remove_cvref_t<U> {
            using Out = std::remove_cvref_t<U>;
            return Out{static_cast<Out::value_type>(m_v)};
        }

        // TODO: add try_as

        template<Number U>
        constexpr auto as_raw() const noexcept -> U {
            return static_cast<U>(m_v);
        }

        auto to_string() const -> std::string {
            if constexpr (Integral<T>) {
                std::array<char, 64> buf{};
                auto [ptr, ec] = std::to_chars(buf.data(), buf.data() + buf.size(), m_v);
                return std::string(buf.data(), ptr);
            } else {
                // Floating<T>
                std::ostringstream oss;
                oss << std::setprecision(std::numeric_limits<T>::max_digits10) << m_v;
                return oss.str();
            }
        }

        /* ---------- LIMITS ---------- */

        inline static constexpr Num MIN{std::numeric_limits<T>::lowest()};
        inline static constexpr Num MAX{std::numeric_limits<T>::max()};

        /* ---------- COMMON MATH ---------- */

        constexpr auto abs() const noexcept -> Num {
            if constexpr (std::unsigned_integral<T>) {
                return *this;
            } else if constexpr (Floating<T>) {
                return Num{static_cast<T>(std::abs(m_v))};
            } else {
                // signed integral
                return m_v < 0 ? Num{static_cast<T>(-m_v)} : *this;
            }
        }

        constexpr auto min(Num other) const noexcept -> Num { return m_v < other.m_v ? *this : other; }
        constexpr auto max(Num other) const noexcept -> Num { return m_v < other.m_v ? other : *this; }

        constexpr auto clamp(Num lo, Num hi) const noexcept -> Num {
            return this->max(lo).min(hi);
        }

        constexpr auto sqrt() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::sqrt(m_v))};
        }

        constexpr auto cbrt() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::cbrt(m_v))};
        }

        constexpr auto exp() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::exp(m_v))};
        }

        constexpr auto log() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::log(m_v))};
        }

        constexpr auto log10() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::log10(m_v))};
        }

        constexpr auto pow(T e) const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::pow(m_v, e))};
        }

        template<Floating U>
        constexpr auto pow(Num<U> e) const -> Num<std::common_type_t<T, U> > requires Floating<T> {
            using R = std::common_type_t<T, U>;
            return Num<R>{static_cast<R>(std::pow(this->as<R>(), e.template as<R>()))};
        }

        // trig funcs

        constexpr auto sin() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::sin(m_v))};
        }

        constexpr auto cos() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::cos(m_v))};
        }

        constexpr auto tan() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::tan(m_v))};
        }

        constexpr auto asin() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::asin(m_v))};
        }

        constexpr auto acos() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::acos(m_v))};
        }

        constexpr auto atan() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::atan(m_v))};
        }

        template<Floating U>
        constexpr auto atan2(Num<U> x) const -> Num<std::common_type_t<T, U> > requires Floating<T> {
            using R = std::common_type_t<T, U>;
            return Num<R>{static_cast<R>(std::atan2(this->as<R>(), x.template as<R>()))}; // y.atan2(x)
        }

        template<Floating U>
        constexpr auto hypot(Num<U> x) const -> Num<std::common_type_t<T, U> > requires Floating<U> {
            using R = std::common_type_t<T, U>;
            return Num<R>{static_cast<R>(std::hypot(this->as<R>(), x.template as<R>()))};
        }

        // rounding

        constexpr auto floor() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::floor(m_v))};
        }

        constexpr auto ceil() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::ceil(m_v))};
        }

        constexpr auto round() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::round(m_v))};
        }

        constexpr auto trunc() const -> Num requires Floating<T> {
            return Num{static_cast<T>(std::trunc(m_v))};
        }

        constexpr auto fract() const -> Num requires Floating<T> {
            return Num{static_cast<T>(m_v - std::trunc(m_v))};
        }

        // predicates

        constexpr auto is_nan() const noexcept -> bool requires Floating<T> {
            return std::isnan(m_v);
        }

        constexpr auto is_finite() const noexcept -> bool requires Floating<T> {
            return std::isfinite(m_v);
        }

        constexpr auto is_inf() const noexcept -> bool requires Floating<T> {
            return std::isinf(m_v);
        }

        /* ---------- UNARY ---------- */

        constexpr auto operator+() const noexcept -> Num { return *this; }
        constexpr auto operator-() const noexcept -> Num { return Num{static_cast<T>(-m_v)}; }

        /* ---------- INCR/DECR ---------- */

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

        /* ---------- COMPOUND ASSIGNMENTS (STRICT: SAME T ONLY) ---------- */

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

        /* ---------- COMPARISON ---------- */

        constexpr bool operator==(const Num &) const noexcept = default;

        constexpr auto operator<=>(const Num &) const noexcept = default;

    private:
        T m_v{};
    };

    /* ---------- Num op Num ---------- */

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

    /* ---------- Num op T ---------- */

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

    /* ---------- T op Num ---------- */

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


    template<Number T>
    inline auto operator<<(std::ostream &os, const Num<T> &x) -> std::ostream & {
        return os << x.to_string();
    }

    /* ---------- STREAMS ---------- */

    template<Number T>
    inline auto operator>>(std::istream &is, Num<T> &x) -> std::istream & {
        if constexpr (std::same_as<T, std::int8_t> || std::same_as<T, std::uint8_t>) {
            int tmp{};
            if (is >> tmp) {
                if (tmp < static_cast<int>(std::numeric_limits<T>::min()) ||
                    tmp > static_cast<int>(std::numeric_limits<T>::max())) {
                    is.setstate(std::ios::failbit);
                } else {
                    x = Num<T>{static_cast<T>(tmp)};
                }
            }
            return is;
        } else {
            T tmp{};
            if (is >> tmp) x = Num<T>{tmp};
            return is;
        }
    }

    template<typename T, Number U>
    constexpr auto as(U u) noexcept -> Num<T> {
        return Num<T>{static_cast<T>(u)};
    }

    /* ---------- ALIASES ---------- */

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

/* ---------- LITERALS ---------- */

namespace cn::literals {
    constexpr i8 operator"" _i8(unsigned long long v) noexcept { return i8{static_cast<std::int8_t>(v)}; }
    constexpr i16 operator"" _i16(unsigned long long v) noexcept { return i16{static_cast<std::int16_t>(v)}; }
    constexpr i32 operator"" _i32(unsigned long long v) noexcept { return i32{static_cast<std::int32_t>(v)}; }
    constexpr i64 operator"" _i64(unsigned long long v) noexcept { return i64{static_cast<std::int64_t>(v)}; }

    constexpr u8 operator"" _u8(unsigned long long v) noexcept { return u8{static_cast<std::uint8_t>(v)}; }
    constexpr u16 operator"" _u16(unsigned long long v) noexcept { return u16{static_cast<std::uint16_t>(v)}; }
    constexpr u32 operator"" _u32(unsigned long long v) noexcept { return u32{static_cast<std::uint32_t>(v)}; }
    constexpr u64 operator"" _u64(unsigned long long v) noexcept { return u64{static_cast<std::uint64_t>(v)}; }

    constexpr isize operator"" _isize(unsigned long long v) noexcept { return isize{static_cast<std::ptrdiff_t>(v)}; }
    constexpr usize operator"" _usize(unsigned long long v) noexcept { return usize{static_cast<std::size_t>(v)}; }

    constexpr f32 operator"" _f32(long double v) noexcept { return f32{static_cast<std::float32_t>(v)}; }
    constexpr f64 operator"" _f64(long double v) noexcept { return f64{static_cast<std::float64_t>(v)}; }

    constexpr f32 operator"" _f32(unsigned long long v) noexcept { return f32{static_cast<std::float32_t>(v)}; }
    constexpr f64 operator"" _f64(unsigned long long v) noexcept { return f64{static_cast<std::float64_t>(v)}; }
}

/* ---------- CONSTS ---------- */

namespace cn::consts {
    // TODO: tau

    template<typename T>
    inline constexpr std::remove_cvref_t<T> e{std::numbers::e_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> log2e{std::numbers::log2e_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> log10e{std::numbers::log10e_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> pi{std::numbers::pi_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> inv_pi{std::numbers::inv_pi_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> inv_sqrtpi{std::numbers::inv_sqrtpi_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> ln2{std::numbers::ln2_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> ln10{std::numbers::ln10_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> sqrt2{std::numbers::sqrt2_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> sqrt3{std::numbers::sqrt3_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> inv_sqrt3{std::numbers::inv_sqrt3_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> egamma{std::numbers::egamma_v<T>};

    template<typename T>
    inline constexpr std::remove_cvref_t<T> phi{std::numbers::phi_v<T>};
}
