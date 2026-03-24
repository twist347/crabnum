#include <benchmark/benchmark.h>
#include "crabnum.h"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <vector>

static std::vector<std::uint64_t> g_u64;

constexpr std::size_t N = 1 << 16;

static void EnsureData() {
    if (!g_u64.empty()) {
        return;
    }
    g_u64.resize(N);
    for (std::size_t i = 0; i < N; ++i) {
        g_u64[i] = static_cast<std::uint64_t>(i * 7 + 13);
    }
}

static void BM_Popcount_Num(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        std::uint32_t total = 0;
        for (std::size_t i = 0; i < N; ++i) {
            total += cn::u64{g_u64[i]}.count_ones();
        }
        benchmark::DoNotOptimize(total);
    }
}

static void BM_Popcount_Std(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        std::uint32_t total = 0;
        for (std::size_t i = 0; i < N; ++i) {
            total += static_cast<std::uint32_t>(std::popcount(g_u64[i]));
        }
        benchmark::DoNotOptimize(total);
    }
}

static void BM_LeadingZeros_Num(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        std::uint32_t total = 0;
        for (std::size_t i = 0; i < N; ++i) {
            total += cn::u64{g_u64[i]}.leading_zeros();
        }
        benchmark::DoNotOptimize(total);
    }
}

static void BM_LeadingZeros_Std(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        std::uint32_t total = 0;
        for (std::size_t i = 0; i < N; ++i) {
            total += static_cast<std::uint32_t>(std::countl_zero(g_u64[i]));
        }
        benchmark::DoNotOptimize(total);
    }
}

BENCHMARK(BM_Popcount_Num);
BENCHMARK(BM_Popcount_Std);
BENCHMARK(BM_LeadingZeros_Num);
BENCHMARK(BM_LeadingZeros_Std);
