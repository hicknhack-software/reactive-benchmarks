#include "../common.h"

#include "range.h"
void LambdaRange(benchmark::State& state) {
    while (state.KeepRunning()) {
        int_range(1, ITERATION_COUNT).each([](auto i){
            [](auto j){
                [](auto k){
                    if (FILTER_VALUE(k)) doNotOptimize(k);
                }(ADD_VALUE(j));
            }(ELEMENT_TYPE(i));
        });
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(LambdaRange)->MinTime(0.1);

BENCHMARK_MAIN()
