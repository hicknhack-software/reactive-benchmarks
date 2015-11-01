#include "../common.h"

#include <algorithm>

#define ABL_REDUCE_ALWAYS_VARIADIC 1
#include <atria/xform/transducer/range.hpp>
#include <atria/xform/transducer/filter.hpp>
#include <atria/xform/transducer/map.hpp>
#include <atria/xform/transducer/sink.hpp>
#include <atria/xform/run.hpp>
#include <atria/xform/sequence.hpp>

#include <atria/xform/transducer/enumerate.hpp>
#include <atria/xform/transducer/take.hpp>

void AtriaRange(benchmark::State& state) {
    using namespace atria::xform;
    using namespace atria::prelude;

    auto rng = range(1, ITERATION_COUNT);
    while (state.KeepRunning()) {
        run(
                    comp(
                        rng,
                        map([](auto j){ return ADD_VALUE(j); }),
                        filter([](auto k){ return FILTER_VALUE(k); }),
                        sink([](auto m){ doNotOptimize(m); })
                        ));
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(AtriaRange)->MinTime(0.2);

void AtriaSequence(benchmark::State& state) {
    using namespace atria::xform;
    using namespace atria::prelude;

    auto seq = sequence(range(1, ITERATION_COUNT));
    while (state.KeepRunning()) {
        run(
                    comp(
                        map([](auto j){ return ADD_VALUE(j); }),
                        filter([](auto k){ return FILTER_VALUE(k); }),
                        sink([](auto m){ doNotOptimize(m); })
                        ),
                    seq);
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(AtriaSequence)->MinTime(0.5);

void AtriaEnumerate(benchmark::State& state) {
    using namespace atria::xform;
    using namespace atria::prelude;

    auto input = comp(enumerate_from(1), take(ITERATION_COUNT));
    while (state.KeepRunning()) {
        run(
                    comp(
                        input,
                        map([](auto j){ return ADD_VALUE(j); }),
                        filter([](auto k){ return FILTER_VALUE(k); }),
                        sink([](auto m){ doNotOptimize(m); })
                        )
                    );
    }
    state.SetItemsProcessed(ITERATION_COUNT * state.iterations());
    state.SetBytesProcessed(ITERATION_COUNT * state.iterations() * sizeof(ELEMENT_TYPE));
}
BENCHMARK(AtriaEnumerate)->MinTime(0.3);

BENCHMARK_MAIN()
