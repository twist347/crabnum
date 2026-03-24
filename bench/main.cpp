#include <benchmark/benchmark.h>

#include "crabnum.h"

#include <vector>
#include <cstddef>

static std::vector<std::float64_t> g_data;

static void EnsureData(std::size_t n) {
    if (g_data.size() >= n) return;
    g_data.resize(n);
    for (std::size_t i = 0; i < n; ++i) {
        g_data[i] = 0.001 * static_cast<std::float64_t>(i) + 0.123;
    }
}

static void BM_Sum_PrimitiveF64(benchmark::State &state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    EnsureData(n);

    for (auto _: state) {
        double sum = 0.0;
        for (std::size_t i = 0; i < n; ++i) {
            sum += g_data[i];
        }
        benchmark::DoNotOptimize(sum);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

static void BM_Sum_CrabnumF64_AddEq(benchmark::State &state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    EnsureData(n);

    for (auto _: state) {
        cn::f64 sum{0.0};
        for (std::size_t i = 0; i < n; ++i) {
            sum += g_data[i]; // operator+=(U)
        }
        benchmark::DoNotOptimize(sum.value());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
}

BENCHMARK(BM_Sum_PrimitiveF64)->RangeMultiplier(2)->Range(1 << 10, 1 << 24);
BENCHMARK(BM_Sum_CrabnumF64_AddEq)->RangeMultiplier(2)->Range(1 << 10, 1 << 24);
