#pragma once

//State reducer(State s, Element e) {
//    if (e % 2 == 0)
//        return s.next(e + 1);
//    return s;
//}

template <typename Op>
struct mutation_state {
    constexpr explicit mutation_state(Op op)
        : op_m(op) {}

    template <typename Element>
    constexpr void next(Element e) const {
        op_m(e);
    }

private:
    Op op_m;
};

template <typename Source, typename Mutation>
struct mutate_expression {
    using this_t = mutate_expression;

    constexpr mutate_expression(const Source *s, Mutation m)
        : source_m(s), mutation_m(m) {}

    template <typename Op>
    constexpr auto mutate(Op r) const {
        return mutate_expression< this_t, Op >(this, r);
    }

    template < typename Op >
    auto each(Op op) const {
        mutation_state<Op> state(op);
        source_m->each([=](auto e){ mutation_m(state, e); });
//        each_with_state( mutation_state<Op>(op) );
    }

//    template < typename Op >
//    constexpr auto each_with_state(mutation_state<Op> state) const {
//        source_m.each([=](auto e){ mutation_m(state, e); });
//    }

private:
    const Source* source_m;
    Mutation mutation_m;
};

template<typename Element>
struct int_range_t {
    constexpr int_range_t(Element start, Element count)
        : start_m(start), count_m(count) {}

    template <typename Op>
    auto each(Op op) const {
        auto e = start_m;
        const auto l = count_m;
        for (auto i = 0; i < l; i++, e++ ) op(e);
    }

    template <typename Op>
    constexpr auto mutate(Op r) const {
        return mutate_expression< int_range_t, Op >(this, r);
    }

private:
    Element start_m;
    Element count_m;
};

template<typename Element>
constexpr auto rx_int_range(Element start, Element count) {
    return int_range_t<Element>(start, count);
}

