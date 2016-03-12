#pragma once

struct Max;

struct Min {
    using Opposite = Max;
    template <class ForwardIterator>
    static ForwardIterator
    extreme_element(ForwardIterator first, ForwardIterator last)
    {
        return std::min_element(first, last);
    }
    static bool
    greater(int lhs, int rhs)
    {
        return lhs < rhs;
    }
};

struct Max {
    using Opposite = Min;
    template <class ForwardIterator>
    static ForwardIterator
    extreme_element(ForwardIterator first, ForwardIterator last)
    {
        return std::max_element(first, last);
    }
    static bool
    greater(int lhs, int rhs)
    {
        return lhs > rhs;
    }
};
