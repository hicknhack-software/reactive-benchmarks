#include "../common.h"

void PlainLoop(benchmark::State& state) {
    while (state.KeepRunning()) {
        for(auto i = 1; i <= ITERATION_COUNT; i++) {
            const auto j = ELEMENT_TYPE(i);
            const auto k = ADD_VALUE(j);
            if (FILTER_VALUE(k)) {
                doNotOptimize(k);
            }
        }
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(PlainLoop)->MinTime(0.1);

void LambdaLoop(benchmark::State& state) {
    while (state.KeepRunning()) {
        for(auto i = 1; i <= ITERATION_COUNT; i++) {
            [](auto j){
                [](auto k){
                    if (FILTER_VALUE(k)) {
                        doNotOptimize(k);
                    }
                }(ADD_VALUE(j));
            }(ELEMENT_TYPE(i));
        }
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(LambdaLoop)->MinTime(0.1);

BENCHMARK_MAIN()
