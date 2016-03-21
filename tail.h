#pragma once

template <typename T>
struct Max;

template <typename T>
struct Min {
    using Opposite = Max<T>;
    template <class ForwardIterator>
    static ForwardIterator
    extreme_element(ForwardIterator first, ForwardIterator last)
    {
        return std::min_element(first, last);
    }
    static bool
    greater(T lhs, T rhs)
    {
        return lhs < rhs;
    }
};

template <typename T>
struct Max {
    using Opposite = Min<T>;
    template <class ForwardIterator>
    static ForwardIterator
    extreme_element(ForwardIterator first, ForwardIterator last)
    {
        return std::max_element(first, last);
    }
    static bool
    greater(T lhs, T rhs)
    {
        return lhs > rhs;
    }
};
