#include "../common.h"

#include "rxMini.h"

void rxMiniLoop(benchmark::State& state) {
    while (state.KeepRunning()) {
        rx_int_range(1, ITERATION_COUNT)
                .mutate([](auto state, auto element){ state.next(ELEMENT_TYPE(element)); })
                .mutate([](auto state, auto element){ state.next(ADD_VALUE(element)); })
                .mutate([](auto state, auto element){ if (FILTER_VALUE(element)) state.next(element); })
                .each([](auto element){
//            if (FILTER_VALUE(element))
                doNotOptimize(element * 2);
        });
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(rxMiniLoop)->MinTime(0.1);

BENCHMARK_MAIN()
