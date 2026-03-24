#include <benchmark/benchmark.h>
#include "crabnum.h"

#include <cstddef>
#include <cstdint>
#include <vector>

static std::vector<std::int32_t> g_i32;
static std::vector<std::uint64_t> g_u64;
static std::vector<double> g_f64;

constexpr std::size_t N = 1 << 16;

static void EnsureData() {
    if (!g_i32.empty()) {
        return;
    }
    g_i32.resize(N);
    g_u64.resize(N);
    g_f64.resize(N);
    for (std::size_t i = 0; i < N; ++i) {
        g_i32[i] = static_cast<std::int32_t>(i % 1000) - 500;
        g_u64[i] = static_cast<std::uint64_t>(i * 7 + 13);
        g_f64[i] = 0.001 * static_cast<double>(i) + 0.123;
    }
}

static void BM_Sum_Primitive_i32(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        std::int32_t sum = 0;
        for (std::size_t i = 0; i < N; ++i) {
            sum += g_i32[i];
        }
        benchmark::DoNotOptimize(sum);
    }
}

static void BM_Sum_Num_i32(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        cn::i32 sum{0};
        for (std::size_t i = 0; i < N; ++i) {
            sum += cn::i32{g_i32[i]};
        }
        benchmark::DoNotOptimize(sum.value());
    }
}

static void BM_Sum_Primitive_u64(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        std::uint64_t sum = 0;
        for (std::size_t i = 0; i < N; ++i) {
            sum += g_u64[i];
        }
        benchmark::DoNotOptimize(sum);
    }
}

static void BM_Sum_Num_u64(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        cn::u64 sum{0};
        for (std::size_t i = 0; i < N; ++i) {
            sum += cn::u64{g_u64[i]};
        }
        benchmark::DoNotOptimize(sum.value());
    }
}

static void BM_Sum_Primitive_f64(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        double sum = 0.0;
        for (std::size_t i = 0; i < N; ++i) {
            sum += g_f64[i];
        }
        benchmark::DoNotOptimize(sum);
    }
}

static void BM_Sum_Num_f64(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        cn::f64 sum{0.0};
        for (std::size_t i = 0; i < N; ++i) {
            sum += cn::f64{g_f64[i]};
        }
        benchmark::DoNotOptimize(sum.value());
    }
}

BENCHMARK(BM_Sum_Primitive_i32);
BENCHMARK(BM_Sum_Num_i32);
BENCHMARK(BM_Sum_Primitive_u64);
BENCHMARK(BM_Sum_Num_u64);
BENCHMARK(BM_Sum_Primitive_f64);
BENCHMARK(BM_Sum_Num_f64);
