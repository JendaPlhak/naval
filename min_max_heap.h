#pragma once

#include <boost/optional.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>

#include "tail.h"

template <typename T>
class MinMaxHeap {
private:
    std::vector<T> m_min;
    std::vector<T> m_max;

    static constexpr size_t UNDEFINED = std::numeric_limits<size_t>::max();

public:
    T min() const;
    T max() const;

    void insert(T x);
    T deleteMin();
    T deleteMax();

private:
    static size_t get_parent(size_t idx);
    template <class Tail>
    std::pair<size_t, size_t> get_children(size_t idx) const;

    // Bit of a hack to avoid problems with partial template specialization.
    std::vector<T>& get_heap(Min<T>);
    std::vector<T>& get_heap(Max<T>);
    const std::vector<T>& get_heap(Min<T>) const;
    const std::vector<T>& get_heap(Max<T>) const;

    T pop_tail();


    template <class Tail>
    void heapify_up(size_t idx);

    template <class Tail>
    void heapify_down(size_t idx);
    void balance_up(size_t idx);
    template <class Tail>
    boost::optional<size_t> get_direction(size_t parent) const;

//---------------------Debug functions.-----------------------------------------
// If these functions cause any compile-time problems due to problems with
// linear max and min, just throw them out.
    void check_invariants() const;
    void assert_heap_invariant() const;

    T linear_max() const;
    T linear_min() const;

    void show() const;
//------------------------------------------------------------------------------
};

#include "min_max_heap.tpp"
