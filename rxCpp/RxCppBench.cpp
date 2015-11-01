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
BENCHMARK(PlainLoop);

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
BENCHMARK(LambdaLoop);

#include "rxcpp/rx.hpp"
void RxCppTest1(benchmark::State& state) {
    //using namespace std::literals;
    auto os = rxcpp::observable<>::range(1, ITERATION_COUNT).map([](auto x){ return ELEMENT_TYPE(x); });
    while (state.KeepRunning()) {
        os.map([](const auto x) {
            return ADD_VALUE(x);
        }).filter([](const auto x) {
            return FILTER_VALUE(x);
        }).subscribe([](const auto x) {
            doNotOptimize(x);
        });
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(RxCppTest1);

//#include <vector>
//void VectorLoop(benchmark::State& state) {
//    static std::vector<ELEMENT_TYPE> vec;
//    vec.reserve(ITERATION_COUNT);
//    for(auto i = 1; i <= ITERATION_COUNT; i++) vec.push_back(i);

//    while (state.KeepRunning()) {
//        for(auto i : vec) {
//            const auto j = ADD_VALUE(i);
//            if (FILTER_VALUE(j)) {
//                doNotOptimize(j);
//            }
//        }
//    }
//    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
//    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
//}
//BENCHMARK(VectorLoop);

//#include <list>
//void ListLoop(benchmark::State& state) {
//    static std::list<ELEMENT_TYPE> list;
//    for(auto i = 1; i <= ITERATION_COUNT; i++) list.push_back(i);

//    while (state.KeepRunning()) {
//        for(auto i : list) {
//            const auto j = ADD_VALUE(i);
//            if (FILTER_VALUE(j)) {
//                doNotOptimize(j);
//            }
//        }
//    }
//    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
//    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
//}
//BENCHMARK(ListLoop);

BENCHMARK_MAIN()
