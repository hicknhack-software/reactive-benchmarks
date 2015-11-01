#include "../common.h"

#include <experimental/generator>

template<typename Src, typename Fn>
inline auto co_map(Src &&src, Fn &&fn) {
    for (auto i : src) yield fn(i);
}

template<typename Src, typename Fn>
inline auto co_filter(Src &&src, Fn &&fn) {
    for (auto i : src) if (fn(i)) yield i;
}

void CoroutineLoop(benchmark::State& state) {
    while (state.KeepRunning()) {
        auto i = []{ for(auto i = 1; i <= ITERATION_COUNT; i++) yield i; }();
        auto j = co_map(i, [](auto x){ return ADD_VALUE(x); });
        auto k = co_filter(j, [](auto x){ return FILTER_VALUE(x); });
        for (auto m : k) doNotOptimize(m);
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(CoroutineLoop);

BENCHMARK_MAIN()
