#pragma once
#include <utility>
#include <type_traits>

// GCC 4.9.2 and MSVC do not like this
//template<class _Ty1, class _Ty2>
//constexpr auto is_same_v = std::is_same< _Ty1, _Ty2 >::value;

template< typename Start, typename GoOn, typename Adder>
struct func_range_t;

template< typename Start, typename GoOn, typename Adder>
constexpr auto func_range(Start start, GoOn go_on, Adder adder) {
    return func_range_t<Start, GoOn, Adder>(start, go_on, adder);
}

template< typename Element>
constexpr auto int_range(Element begin, Element end, Element increment = 1) {
    return func_range(
                [begin]{ return begin; },
                [end](Element state){ return state <= end; },
                [increment](Element state){ return state + increment; }
            );
}


template< typename Start, typename GoOn, typename Adder>
struct func_range_t {
    using start_func_t = Start;
    using go_on_func_t = GoOn;
    using add_func_t = Adder;
    using element_t = std::result_of_t<Start()>;
    static_assert(std::is_same<bool, std::result_of_t<GoOn(element_t)>>::value, "invalid end constuct");
    static_assert(std::is_same<element_t, std::result_of_t<Adder(element_t)>>::value, "invalid add constuct");

    constexpr func_range_t() = default;
    constexpr func_range_t(start_func_t start_func, go_on_func_t go_on_func, add_func_t add_func)
        : start_func_m(start_func), go_on_func_m(go_on_func), add_func_m(add_func)
    {}

    constexpr func_range_t(const func_range_t&) = default;
    constexpr func_range_t(func_range_t&&) = default;
    // GCC 4.9.2 does not like constexpr here
    func_range_t& operator =(const func_range_t&) = default;
    func_range_t& operator =(func_range_t&&) = default;

    template< typename Each >
    // MSVC does not like constexpr here
    auto each(Each each) const {
        for(auto state = start_func_m(); go_on_func_m(state); state = add_func_m(state)) {
            each(state);
        }
    }

private:
    start_func_t start_func_m;
    go_on_func_t go_on_func_m;
    add_func_t add_func_m;
};
