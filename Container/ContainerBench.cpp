#include "../common.h"

#include <vector>
void VectorLoop(benchmark::State& state) {
    static std::vector<ELEMENT_TYPE> vec;
    vec.reserve(ITERATION_COUNT);
    for(auto i = 1; i <= ITERATION_COUNT; i++) vec.push_back(i);

    while (state.KeepRunning()) {
        for(auto i : vec) {
            const auto j = ADD_VALUE(i);
            if (FILTER_VALUE(j)) {
                doNotOptimize(j);
            }
        }
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(VectorLoop)->MinTime(0.3);

#include <list>
void ListLoop(benchmark::State& state) {
    static std::list<ELEMENT_TYPE> list;
    for(auto i = 1; i <= ITERATION_COUNT; i++) list.push_back(i);

    while (state.KeepRunning()) {
        for(auto i : list) {
            const auto j = ADD_VALUE(i);
            if (FILTER_VALUE(j)) {
                doNotOptimize(j);
            }
        }
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(ListLoop)->MinTime(0.7);

BENCHMARK_MAIN()
