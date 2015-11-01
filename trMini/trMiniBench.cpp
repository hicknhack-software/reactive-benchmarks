#if defined(_MSC_VER) && !defined(__clang__)
#pragma inline_depth(255)
#endif

#include "../common.h"

#include <iostream>

#include "trMini.h"
//#include "../MyRange/range.h"

#include <iterator>

#undef ITERATION_COUNT
#define ITERATION_COUNT 5

void TrMiniLoop(benchmark::State& state) {
    using namespace trMini;

    auto input = enumerate(1) | take(ITERATION_COUNT);

    int output = 0;

    auto effect = [&](int m){
        std::cout << "sink " << m;
        std::cout << " " << output;
        std::cout << std::endl;
        doNotOptimize(m);
        output += m;
    };

    while (state.KeepRunning()) {
//        std::cout << "start\n";
        run(input
//            | map([](auto &&j){ return ADD_VALUE(j); })
//            | filter([](auto &&k){ return FILTER_VALUE(k); })
//            | map([](auto &&j){ std::cout << "map " << j << std::endl; return ADD_VALUE(j); })
//            | filter([](auto &&k){ std::cout << "filter " << k << std::endl; return FILTER_VALUE(k); })
//            | sink(effect)
            );
        std::cout << "output " << output << "\n";
    }
//        doNotOptimize(output * 2);
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(TrMiniLoop)->MinTime(0.1);

BENCHMARK_MAIN()
