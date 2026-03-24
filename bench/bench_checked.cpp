#include <benchmark/benchmark.h>
#include "crabnum.h"

#include <cstddef>
#include <cstdint>
#include <limits>
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

static void BM_CheckedAdd_Num(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        cn::i32 sum{0};
        for (std::size_t i = 0; i < N; ++i) {
            if (auto r = sum.checked_add(cn::i32{g_i32[i]})) {
                sum = *r;
            }
        }
        benchmark::DoNotOptimize(sum.value());
    }
}

static void BM_CheckedAdd_Manual(benchmark::State &state) {
    EnsureData();
    for (auto _: state) {
        std::int32_t sum = 0;
        for (std::size_t i = 0; i < N; ++i) {
            auto v = g_i32[i];
            if ((v > 0 && sum > std::numeric_limits<std::int32_t>::max() - v) ||
                (v < 0 && sum < std::numeric_limits<std::int32_t>::min() - v)) {
                continue;
            }
            sum += v;
        }
        benchmark::DoNotOptimize(sum);
    }
}

BENCHMARK(BM_CheckedAdd_Num);
BENCHMARK(BM_CheckedAdd_Manual);
