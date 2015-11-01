#pragma once

#include <functional>
#include <utility>
#include <type_traits>

namespace trMini {

template<typename State>
constexpr bool state_is_finished(const State& state);

struct state_none {};

template<typename Next, typename Data>
struct state_wrapper {
    Next next;
    Data data;

    bool is_finished() const {
        return state_is_finished(data) || state_is_finished(next);
    }
};

template<typename Next, typename Data>
constexpr auto wrap_state(Next&& next, Data&& data) {
    return state_wrapper<Next, Data> { std::forward<Next>(next), std::forward<Data>(data) };
}

namespace impl {

struct try_any_state {};
struct try_state_with_is_finished : try_any_state {};

template<typename State>
auto state_is_finished(try_state_with_is_finished, const State& state)
    -> decltype(state.is_finished()) {
    return state.is_finished();
}

template<typename State>
bool state_is_finished(try_any_state, const State&) {
    return false;
}

} // namespace impl

template<typename State>
constexpr bool state_is_finished(const State& state) {
    return impl::state_is_finished(impl::try_state_with_is_finished{}, state);
}

//template<typename Step, typename State, typename Range>
//auto reduce(Step&& step, State&& initial, Range&& range) {
//    auto state = step.init(std::forward<State>(initial));
//    for (auto x : range) {
//        auto next = step(std::move(state), x);
//        state = std::move(next);
//        if (state_is_finished(state)) break;
//    }
//    return step.complete(state);
//}

template<typename Step, typename State>
auto reduce(Step&& step, State&& initial) {
    auto state = step.init(std::forward<State>(initial));
    while (!state_is_finished(state)) {
        auto next = step(std::move(state));
        state = std::move(next);
    }
    return step.complete(state);
}

template<typename Init, typename Reduce, typename Complete>
struct OutputReducer {
    template<typename State>
    auto init(State&& state) {
        static_assert(std::is_same<
                      State,
                      std::result_of_t<Complete(std::result_of_t<Init(State)>)>>::value
                      , "complete function should return the state");
        return init_m(std::forward<State>(state));
    }

    template<typename State, typename ...Inputs>
    auto operator () (State&& state, Inputs&& ...inputs) {
        static_assert(std::is_same<
                      State,
                      std::result_of_t<Reduce(State, Inputs...)>>::value
                      , "reduce function should return the same state");
        return reduce_m(std::forward<State>(state), std::forward<Inputs>(inputs)...);
    }

    template<typename State>
    auto complete(State&& state) {
        return complete_m(std::forward<State>(state));
    }

    Init init_m;
    Reduce reduce_m;
    Complete complete_m;
};

template<typename Init, typename Do, typename Complete>
constexpr auto init_output_reducer(Init&& init, Do&& reduce_do, Complete&& complete) {
    return OutputReducer<Init, Do, Complete> {
        std::forward<Init>(init),
        std::forward<Do>(reduce_do),
        std::forward<Complete>(complete)
    };
}

template<typename Do>
constexpr auto output_reducer(Do&& reduce_do) {
    return init_output_reducer(
        [] (auto&& state) { return std::forward<decltype(state)>(state); },
        std::forward<Do>(reduce_do),
        [] (auto&& state) { return std::forward<decltype(state)>(state); }
    );
}

auto iterator_output = output_reducer([](auto&& output, auto&& value) {
    *output++ = std::forward<decltype(value)>(value);
    return std::forward<decltype(output)>(output);
});

auto first_output = output_reducer([](auto&& state, auto&& ...values) {
    (void)std::initializer_list<int>{(values,0)...}; // GCC requires one value for unnamed parameter packs
    return std::forward<decltype(state)>(state);
});

template<typename Step, typename ...Range>
auto run(Step&& step, Range&& ...range) {
    reduce(step(first_output), state_none{}, std::forward<Range>(range)...);
}

template<typename Step, typename Init, typename Do, typename Complete>
struct StepReducer {
    template<typename State>
    auto init(State&& state) {
        using InitState = std::result_of_t<Init(Step, State)>;
        using CompleteState = std::result_of_t<Complete(Step, InitState)>;
        static_assert(std::is_same<State, CompleteState>::value, "complete function should return the state");
        return init_m(step_m, std::forward<State>(state));
    }

    template<typename State, typename ...Inputs>
    auto operator () (State&& state, Inputs&& ...inputs) {
        using DoState = std::result_of_t<Do(Step, State, Inputs...)>;
        static_assert(std::is_same<State, DoState>::value, "reduce function should return the state");
        return do_m(step_m, std::forward<State>(state), std::forward<Inputs>(inputs)...);
    }

    template<typename State>
    auto complete(State&& state) {
        return complete_m(step_m, std::forward<decltype(state)>(state));
    }

    Step step_m;
    Init init_m;
    Do do_m;
    Complete complete_m;
};

template<typename Init, typename Do, typename Complete>
auto init_reducer(Init&& init, Do&& reduce_do, Complete&& complete) {
    return [=] (auto &&step) {
        return StepReducer<decltype(step), Init, Do, Complete> {
            std::forward<decltype(step)>(step),
            init,
            reduce_do,
            complete
        };
    };
}

template<typename Do>
auto reducer(Do&& reduce_do) {
    return init_reducer(
        [] (auto&& step, auto&& state) { return step.init(std::forward<decltype(state)>(state)); },
        std::forward<Do>(reduce_do),
        [] (auto&& step, auto&& state) { return step.complete(std::forward<decltype(state)>(state)); }
    );
}

template<typename Build>
struct Transducer {
    template<typename Step>
    auto operator() (Step &&step) const {
        return build_m(std::forward<Step>(step));
    }

    Build build_m;
};

template<typename Build>
auto transducer(Build&& build) {
    return Transducer<Build> { std::forward<Build>(build) };
}

template<typename Build, typename Output>
auto operator |(Transducer<Build> transducer, Output &&output) {
    return transducer.build_m(std::forward<Output>(output));
}

template<typename Outer, typename Inner>
auto operator |(Transducer<Outer> outer, Transducer<Inner> inner) {
    return transducer([=](auto&& step) {
        return outer(inner(std::forward<decltype(step)>(step)));
    });
}

template<typename Integral>
auto enumerate(const Integral&& start = {}) {
    struct enum_state {
        Integral current;
    };
    return transducer(init_reducer(
        [=] (auto&& step, auto&& state) {
            auto init = step.init(std::forward<decltype(state)>(state));
            return wrap_state(std::move(init), enum_state{start});
        },
        [] (auto&& step, auto&& state, auto&& ...inputs) {
            auto data = state.data;
            auto next = step(std::move(state.next), data.current, std::forward<decltype(inputs)>(inputs)...);
            return wrap_state(std::move(next), enum_state{++data.current});
        },
        [] (auto&& step, auto&& state) {
            return step.complete(std::move(state.next));
        }
    ));
}

template<typename Range>
auto iterate(const Range& range) {
    using it = std::decay_t<decltype(std::begin(range))>;
    struct iterate_state {
        it current;
        it end;
        bool is_finished() const { return current == end; }
    };
    return transducer(init_reducer(
        [=] (auto&& step, auto&& state) {
            auto init = step.init(std::forward<decltype(state)>(state));
            return wrap_state(std::move(init), iterate_state{std::begin(range), std::end(range)});
        },
        [] (auto&& step, auto&& state, auto&& ...inputs) {
            auto data = state.data;
            // std::cout << "iter " << *(data.current) << "\n";
            auto next = step(std::move(state.next), *(data.current), std::forward<decltype(inputs)>(inputs)...);
            return wrap_state(std::move(next), iterate_state{++data.current, data.end});
        },
        [] (auto&& step, auto&& state) {
            return step.complete(std::move(state.next));
        }
    ));
}

template<typename Number>
auto take(Number num) {
    struct take_state {
        Number count;
        bool is_finished() const { return count == 0; }
    };
    std::cout << "take " << num << "\n";
    return transducer(init_reducer(
        [=] (auto&& step, auto&& state) {
            std::cout << "init " << num << "\n";
            auto init = step.init(std::forward<decltype(state)>(state));
            return wrap_state(std::move(init), take_state{num});
        },
        [] (auto&& step, auto&& state, auto&& ...inputs) {
            auto data = std::move(state.data);
            std::cout << "count " << data.count << "\n";
            auto next = step(std::move(state.next), std::forward<decltype(inputs)>(inputs)...);
            return wrap_state(std::move(next), take_state{data.count - 1});
        },
        [] (auto&& step, auto&& state) {
            return step.complete(std::move(state.next));
        }
    ));
}

template<typename Fn>
constexpr auto sink(Fn&& fn) {
    return transducer(reducer(
        [=] (auto&& step, auto&& state, auto&& ...inputs) {
            fn(std::forward<decltype(inputs)>(inputs)...);
            return step(std::forward<decltype(state)>(state));
        }
    ));
}

template<typename Fn>
constexpr auto map(Fn&& fn) {
    return transducer(reducer(
        [=] (auto&& step, auto&& state, auto&& ...inputs) {
            auto output = fn(std::forward<decltype(inputs)>(inputs)...);
            return step(std::forward<decltype(state)>(state), std::move(output));
        }
    ));
}

template<typename Fn>
constexpr auto filter(Fn&& pred) {
    return transducer(reducer(
        [=] (auto&& step, auto&& state, auto&& ...inputs) {
            return pred(inputs...) ?
                        step(std::forward<decltype(state)>(state), std::forward<decltype(inputs)>(inputs)...)
                      : std::forward<decltype(state)>(state);
        }
    ));
}

} // namespace trMini

//template<typename Step, typename ...Ranges>
//constexpr auto sequence(Step step, Ranges ...ranges) {

//    using value_t = std::result_of_t< step(std::result_of_t< std::begin(Ranges) >... )>;
//    using cache_t = vector<value_t>;
//    using state_t = cache_t*;

//    struct range {
//        struct iterator : public std::iterator< std::forward_iterator_tag, value_t> {
//            explicit iterator(range range) noexcept
//                : valid_m(true), range_m(range)
//            {}
//            explicit iterator(range range) noexcept = default;

//            bool operator == (const iterator &ot) const noexcept {
//                return valid_m == ot.valid_m && range_m == ot.range_m;
//            }
//            bool operator != (const iterator &ot) const noexcept {
//                return ! (*this == ot);
//            }

//            auto operator*() const noexcept {
//                return *state_m;
//            }
//            auto operator*() noexcept {
//                return *state_m;
//            }

//            auto operator++() noexcept {

//                return (iterator&)*this;
//            }

//            auto operator++(int) noexcept {
//                auto __tmp = *this;
//                ++(*this);
//                return __tmp;
//            }

//        private:
//            state state_m;
//            range range_m;
//            bool valid_m = false;
//        };

//        range(Step step, Ranges... ranges)
//            : step_m(step), ranges_m(ranges...)
//        {}

//        iterator begin() const { return { make_data() }; }
//        iterator end() const { return { boost::none }; }

//    private:
//        Step step_m;
//        std::tuple<Ranges...> ranges_m;
//    };

//    return range(step, ranges);
//}

