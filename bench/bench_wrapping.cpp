#include <benchmark/benchmark.h>
#include "crabnum.h"

#include <cstddef>
#include <cstdint>
#include <vector>

static std::vector<std::int32_t> g_i32;

constexpr std::size_t N = 1 << 16;

static void EnsureData() {
    if (!g_i32.empty()) {
        return;
    }
    g_i32.resize(N);
    for (std::size_t i = 0; i < N; ++i) {
        g_i32[i] = static_cast<std::int32_t>(i % 1000) - 500;
    }
}

static void BM_WrappingAdd_Num(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        cn::i32 sum{0};
        for (std::size_t i = 0; i < N; ++i) {
            sum = sum.wrapping_add(cn::i32{g_i32[i]});
        }
        benchmark::DoNotOptimize(sum.value());
    }
}

static void BM_WrappingAdd_Manual(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        std::int32_t sum = 0;
        for (std::size_t i = 0; i < N; ++i) {
            sum = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(sum) + static_cast<std::uint32_t>(g_i32[i]));
        }
        benchmark::DoNotOptimize(sum);
    }
}

BENCHMARK(BM_WrappingAdd_Num);
BENCHMARK(BM_WrappingAdd_Manual);
