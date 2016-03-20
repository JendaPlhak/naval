#include "printer.h"
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


class MinMaxHeap {
private:
    std::vector<int> m_min;
    std::vector<int> m_max;

    static constexpr size_t UNDEFINED = std::numeric_limits<size_t>::max();

public:
    int min() const;
    int max() const;

    void insert(int x);
    void delete_min();
    void delete_max();

private:
    static size_t get_parent(size_t idx);
    template <class Tail>
    std::pair<size_t, size_t> get_children(size_t idx) const;

    template <class Tail>
    std::vector<int>& get_heap();
    template <class Tail>
    const std::vector<int>& get_heap() const;

    int pop_tail();

    template <class Tail>
    void heapify_up(size_t idx);

    template <class Tail>
    void heapify_down(size_t idx);
    template <class Tail>
    void balance_up(size_t idx);
    template <class Tail>
    boost::optional<size_t> get_direction(size_t parent) const;

//---------------------Debug functions.-----------------------------------------
    void check_invariants() const;
    void assert_heap_invariant() const;

    int linear_max() const;
    int linear_min() const;

    void show() const;
//------------------------------------------------------------------------------
};

int
MinMaxHeap::min() const
{
    assert(m_min.size() + m_max.size() != 0);
    assert(m_min.size() != 0);
    return m_min.front();
}

int
MinMaxHeap::max() const
{
    assert(m_min.size() + m_max.size() != 0);
    if (m_max.size() != 0) {
        return m_max.front();
    } else {
        return m_min.front();
    }
}

void
MinMaxHeap::insert(int x)
{
    assert(m_min.size() - m_max.size() <= 1);

    if (m_max.size() != 0 && this->max() < x) {
        int old_max = this->max();
        m_max.front() = x;
        this->insert(old_max);
        return;
    }// append

    size_t idx;
    if (m_min.size() == m_max.size()) {
        idx = m_min.size();
        m_min.push_back(x);
        this->heapify_up<Min>(idx);
    } else {
        idx = m_max.size();
        m_max.push_back(x);

        this->heapify_up<Max>(idx);
        if (m_max.at(idx) < m_min.at(idx)) {
            std::swap(m_max.at(idx), m_min.at(idx));
            this->heapify_up<Min>(idx);
            this->heapify_up<Max>(idx);
        }

    }

    assert(m_min.size() - m_max.size() <= 1);
    this->check_invariants();
}

void
MinMaxHeap::delete_min()
{

    if (m_min.size() == 0) {
        return;
    }
    if (m_min.size() == 1 && m_max.size() == 0) {
        this->pop_tail();
        return;
    }

    m_min.front() = this->pop_tail();
    this->heapify_down<Min>(0);

    this->check_invariants();
}

void
MinMaxHeap::delete_max()
{
    if (m_min.size() == 0) {
        return;
    }
    if (m_min.size() == 1) {
        this->pop_tail();
        return;
    }

    m_max.front() = this->pop_tail();
    this->heapify_down<Max>(0);

    this->check_invariants();
}

size_t
MinMaxHeap::get_parent(size_t idx)
{
    if (idx == 0) {
        return UNDEFINED;
    }
    return std::floor((idx - 1) / 2.);
}

template <class Tail>
std::pair<size_t, size_t>
MinMaxHeap::get_children(size_t idx) const
{
    const auto& heap = this->get_heap<Tail>();
    size_t left  = 2 * idx + 1;
    size_t right = 2 * idx + 2;
    auto filter = [&] (size_t idx) {
        return idx < heap.size() ? idx : UNDEFINED;
    };
    return std::make_pair(filter(left), filter(right));
}

template <>
std::vector<int>&
MinMaxHeap::get_heap<Min>()
{
    return m_min;
}

template <>
std::vector<int>&
MinMaxHeap::get_heap<Max>()
{
    return m_max;
}

template <>
const std::vector<int>&
MinMaxHeap::get_heap<Min>() const
{
    return m_min;
}

template <>
const std::vector<int>&
MinMaxHeap::get_heap<Max>() const
{
    return m_max;
}

int
MinMaxHeap::pop_tail()
{
    int tail;
    if (m_min.size() == m_max.size()) {
        tail = m_max.back();
        m_max.pop_back();
    } else {
        tail = m_min.back();
        m_min.pop_back();
    }
    return tail;
}

template <class Tail>
void
MinMaxHeap::heapify_up(size_t idx)
{
    auto& heap = this->get_heap<Tail>();
    size_t parent = get_parent(idx);
    while (parent != UNDEFINED) {
        if (Tail::greater(heap.at(idx), heap.at(parent))) {
            std::swap(heap.at(parent), heap.at(idx));
        }
        idx    = parent;
        parent = get_parent(idx);
    }
}

template <class Tail>
void
MinMaxHeap::heapify_down(size_t idx)
{
    auto& heap    = this->get_heap<Tail>();
    auto children = this->get_children<Tail>(idx);
    size_t left  = children.first;
    size_t right = children.second;

    if (left == UNDEFINED) {
        // std::cout << "Balancing up\n";
        this->balance_up<Tail>(idx);
    } else if (right == UNDEFINED) {
        // std::cout << "Balancing up2\n";
        if (Tail::greater(heap.at(left), heap.at(idx))) {
            std::swap(heap.at(left), heap.at(idx));
        }
        this->balance_up<Tail>(left);
    } else {
        auto dir = this->get_direction<Tail>(idx);
        if (dir) {
            std::swap(heap.at(*dir), heap.at(idx));
            this->heapify_down<Tail>(*dir);
        } else {
            // this->balance_up<Tail>(idx);
        }
    }
}

template <class Tail>
boost::optional<size_t>
MinMaxHeap::get_direction(size_t parent) const
{
    const auto& heap = this->get_heap<Tail>();
    auto children    = this->get_children<Tail>(parent);
    size_t left  = children.first;
    size_t right = children.second;

    std::array<int,3> vals({heap.at(parent), heap.at(left), heap.at(right)});
    auto it    = Tail::extreme_element(vals.begin(), vals.end());
    size_t idx = it - vals.begin();

    switch (idx) {
        case 0:
            return boost::none;
        case 1:
            return left;
        case 2:
            return right;
        default:
            assert(false);
    }
}

template <class Tail>
void
MinMaxHeap::balance_up(size_t idx)
{
    if (idx >= m_max.size()) {
        return;
    }
    if (m_min.at(idx) > m_max.at(idx)) {
        std::swap(m_min.at(idx), m_max.at(idx));
        this->heapify_up<Min>(idx);
        this->heapify_up<Max>(idx);
    }
}

int
MinMaxHeap::linear_max() const
{
    int int_min = std::numeric_limits<int>::min();
    auto bin = [] (int a, int b) {
        return std::max(a, b);
    };
    return std::accumulate(begin(m_min), end(m_min),
        std::accumulate(begin(m_max), end(m_max), int_min, bin),
        bin);
}

int
MinMaxHeap::linear_min() const
{
    int int_min = std::numeric_limits<int>::max();
    auto bin = [] (int a, int b) {
        return std::min(a, b);
    };
    return std::accumulate(begin(m_min), end(m_min),
        std::accumulate(begin(m_max), end(m_max), int_min, bin),
        bin);
}

void
MinMaxHeap::check_invariants() const
{
    // show();
    // std::cout << std::endl;
    assert(m_min.size() == m_max.size() || m_min.size() - m_max.size() == 1);

    assert_heap_invariant();

    assert(this->linear_max() == this->max());
    assert(this->linear_min() == this->min());

    for (size_t i = 0; i < m_max.size(); ++i) {
        assert(m_max.at(i) >= m_min.at(i));
    }
}

void
MinMaxHeap::assert_heap_invariant() const
{
    for (size_t idx = 0; idx < m_min.size(); ++idx) {
        auto children = this->get_children<Min>(idx);
        size_t left  = children.first;
        size_t right = children.second;

        if (left != UNDEFINED) {
            assert(m_min.at(idx) <= m_min.at(left));
        }
        if (right != UNDEFINED) {
            assert(m_min.at(idx) <= m_min.at(right));
        }
    }

    for (size_t idx = 0; idx < m_max.size(); ++idx) {
        auto children = this->get_children<Max>(idx);
        size_t left  = children.first;
        size_t right = children.second;

        if (left != UNDEFINED) {
            assert(m_max.at(idx) >= m_max.at(left));
        }
        if (right != UNDEFINED) {
            assert(m_max.at(idx) >= m_max.at(right));
        }
    }
}

void
MinMaxHeap::show() const
{
    std::cout << "==================MIN==================\n";
    print_tree(m_min);
    std::cout << "==================MAX==================\n";
    print_tree(m_max);
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    auto random = []() {
        int max = 50;
        int min = -max;
        return min + (rand() % (int)(max - min + 1));
    };

    std::vector<int> test_permutation;
    for (int i = 0; i < 987; ++i) {
        test_permutation.push_back(random());
    }

    for (int test = 0; test < 10000; ++test) {
        // if (test % 10 == 0) {
        //     std::cout << "Test NO " << test << std::endl;
        // }
        std::random_shuffle(test_permutation.begin(), test_permutation.end());
        MinMaxHeap heap;
        for (const auto n : test_permutation) {
            // std::cout << "inserting: " << n << "\n";
            heap.insert(n);
            // std::cout << "inserting: " << n * 49739 % 50 << "\n";
            heap.insert(n * 49739 % 50);
            // std::cout << "Deleting min.. " << "\n";
            heap.delete_min();
        }
        for (const auto n : test_permutation) {
            (void) n;
            // std::cout << "Deleting min.. " << "\n";
            heap.delete_min();
            // std::cout << "Deleting max.. " << "\n";
            heap.delete_max();
        }
    }
    return 0;
}

