#include "printer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>


class MinMaxHeap {
private:
    std::vector<int> m_min;
    std::vector<int> m_max;

    static constexpr size_t UNDEFINED = std::numeric_limits<size_t>::max();

public:
    void insert(int x);
    void delete_min();
    void delete_max();

    int min() const {
        assert(m_min.size() + m_max.size() != 0);
        assert(m_min.size() != 0);
        return m_min.front();
    }
    int max() const {
        assert(m_min.size() + m_max.size() != 0);
        if (m_max.size() != 0) {
            return m_max.front();
        } else {
            return m_min.front();
        }
    }

private:
    static size_t get_parent(size_t idx)
    {
        if (idx == 0) {
            return UNDEFINED;
        }
        // std::cout << "idx = " <<idx << " parent = " << std::floor((idx - 1) / 2.) << std::endl;
        return std::floor((idx - 1) / 2.);
    }

    std::pair<size_t, size_t> get_children_min(size_t idx) const
    {
        int left  = 2 * idx + 1;
        int right = 2 * idx + 2;
        auto filter = [this] (int idx) {
            return idx < m_min.size() ? idx : UNDEFINED;
        };
        return std::make_pair(filter(left), filter(right));
    }

    std::pair<size_t, size_t> get_children_max(size_t idx) const
    {
        int left  = 2 * idx + 1;
        int right = 2 * idx + 2;
        auto filter = [this] (int idx) {
            return idx < m_max.size() ? idx : UNDEFINED;
        };
        return std::make_pair(filter(left), filter(right));
    }

    void balance_up(size_t idx);
    void propagate_up(size_t child);
    void propagate_down(size_t idx);

    void heapify_down(size_t idx);
    static size_t greater(const std::vector<int>& vec, size_t idx1, size_t idx2);

    static size_t smaller(const std::vector<int>& vec, size_t idx1, size_t idx2);
    int pop_tail();

    static void heapify(size_t idx,
        std::vector<int>& vec,
        std::function<bool(int,int)> cmp);

    void heapify_min(size_t idx);
    void heapify_max(size_t idx);

    void check_invariants() const;
    void assert_heap_invariant() const;

    int linear_max() const;
    int linear_min() const;

    void show() const;
};

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
        assert(m_max.at(i) > m_min.at(i));
    }
}

void
MinMaxHeap::assert_heap_invariant() const
{
    for (size_t idx = 0; idx < m_min.size(); ++idx) {
        auto children = this->get_children_min(idx);
        size_t left  = children.first;
        size_t right = children.second;

        if (left != UNDEFINED) {
            assert(m_min.at(idx) < m_min.at(left));
        }
        if (right != UNDEFINED) {
            assert(m_min.at(idx) < m_min.at(right));
        }
    }

    for (size_t idx = 0; idx < m_max.size(); ++idx) {
        auto children = this->get_children_max(idx);
        size_t left  = children.first;
        size_t right = children.second;

        if (left != UNDEFINED) {
            assert(m_max.at(idx) > m_max.at(left));
        }
        if (right != UNDEFINED) {
            assert(m_max.at(idx) > m_max.at(right));
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

void
MinMaxHeap::insert(int x)
{
    assert(m_min.size() - m_max.size() <= 1);

    if (m_max.size() != 0 && this->max() < x) {
        int old_max = this->max();
        m_max.front() = x;
        this->insert(old_max);
        return;
    }

    size_t idx;
    if (m_min.size() == m_max.size()) {
        idx = m_min.size();
        m_min.push_back(x);
        this->heapify_min(idx);
    } else {
        idx = m_max.size();
        m_max.push_back(x);

        this->heapify_max(idx);
        if (m_max.at(idx) < m_min.at(idx)) {
            std::swap(m_max.at(idx), m_min.at(idx));
        }
        this->heapify_min(idx);
        this->heapify_max(idx);
    }

    assert(m_min.size() - m_max.size() <= 1);
    this->check_invariants();
}

void
MinMaxHeap::heapify(size_t idx,
    std::vector<int>& vec,
    std::function<bool(int,int)> cmp)
{
    size_t parent = get_parent(idx);
    while (parent != UNDEFINED) {
        if (cmp(vec.at(idx), vec.at(parent))) {
            std::swap(vec.at(parent), vec.at(idx));
        }
        idx    = parent;
        parent = get_parent(idx);
    }
}

void
MinMaxHeap::heapify_max(size_t idx)
{
    this->heapify(idx, m_max, std::greater<int>());
}

void
MinMaxHeap::heapify_min(size_t idx)
{
    this->heapify(idx, m_min, std::less<int>());
}

void
MinMaxHeap::balance_up(size_t idx)
{
    if (idx >= m_max.size()) {
        return;
    }
    if (m_min.at(idx) > m_max.at(idx)) {
        std::swap(m_min.at(idx), m_max.at(idx));
        this->heapify_min(idx);
    }
}

void
MinMaxHeap::propagate_up(size_t child)
{
    size_t parent = this->get_parent(child);
    if (parent == UNDEFINED) {
        return;
    }

    assert(child < m_min.size());
    if (m_min.at(parent) > m_min.at(child)) {
        std::swap(m_min.at(parent), m_min.at(child));
    }

    assert(child <= m_max.size());
    if (child == m_max.size()) {
        return;
    }
    if (m_max.at(parent) < m_max.at(child)) {
        std::swap(m_max.at(parent), m_max.at(child));
    }

    if (m_min.at(child) > m_max.at(child)) {
        std::swap(m_min.at(child), m_max.at(child));
    }
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

void
MinMaxHeap::delete_min()
{

    if (m_min.size() == 1) {
        this->pop_tail();
        return;
    }

    m_min.front() = this->pop_tail();
    this->propagate_down(0);

    this->check_invariants();
}

void
MinMaxHeap::delete_max()
{
    if (m_max.size() == 1 || (m_max.size() == 0 && m_min.size() == 1)) {
        this->pop_tail();
        return;
    }

    m_max.front() = this->pop_tail();
    this->heapify_down(0);

    this->check_invariants();
}

size_t
MinMaxHeap::greater(const std::vector<int>& vec, size_t idx1, size_t idx2)
{
    return vec.at(idx1) > vec.at(idx2) ? idx1 : idx2;
}

void
MinMaxHeap::heapify_down(size_t idx)
{
    auto children = this->get_children_max(idx);
    int left  = children.first;
    int right = children.second;

    if (left == UNDEFINED) {
        // std::cout << "Balancing up\n";
        this->balance_up(idx);
    } else if (right == UNDEFINED) {
        // std::cout << "Balancing up2\n";
        if (m_max.at(left) > m_max.at(idx)) {
            std::swap(m_max.at(left), m_max.at(idx));
        }
        this->balance_up(left);
    } else {
        if (m_max.at(left) < m_max.at(idx) && m_max.at(right) < m_max.at(idx)) {
            // std::cout << "Both worse\n";
            this->balance_up(idx);
        } else if (m_max.at(left) > m_max.at(idx) && m_max.at(right) > m_max.at(idx)) {
            // std::cout << "both better\n";
            size_t greater = this->greater(m_max, left, right);
            std::swap(m_max.at(greater), m_max.at(idx));
            this->heapify_down(greater);
        } else if (m_max.at(left) > m_max.at(idx)) {
            // std::cout << "left = " << m_max.at(left) << " root = " << m_max.at(idx) << std::endl;
            std::swap(m_max.at(left), m_max.at(idx));
            this->heapify_down(left);
        } else {
            // std::cout << "right better\n";
            std::swap(m_max.at(right), m_max.at(idx));
            this->heapify_down(right);
        }
    }
}

size_t
MinMaxHeap::smaller(const std::vector<int>& vec, size_t idx1, size_t idx2)
{
    return vec.at(idx1) < vec.at(idx2) ? idx1 : idx2;
}

void
MinMaxHeap::propagate_down(size_t idx)
{
    auto children = this->get_children_min(idx);
    int left  = children.first;
    int right = children.second;

    if (left == UNDEFINED) {
        // std::cout << "Balancing up\n";
        this->balance_up(idx);
    } else if (right == UNDEFINED) {
        // std::cout << "Balancing up2\n";
        this->balance_up(left);
    } else {
        if (m_min.at(left) > m_min.at(idx) && m_min.at(right) > m_min.at(idx)) {
            // std::cout << "Both worse\n";
            this->balance_up(idx);
        } else if (m_min.at(left) < m_min.at(idx) && m_min.at(right) < m_min.at(idx)) {
            // std::cout << "both better\n";
            size_t smaller = this->smaller(m_min, left, right);
            std::swap(m_min.at(smaller), m_min.at(idx));
            this->propagate_down(smaller);
        } else if (m_min.at(left) < m_min.at(idx)) {
            // std::cout << "left = " << m_min.at(left) << " root = " << m_min.at(idx) << std::endl;
            std::swap(m_min.at(left), m_min.at(idx));
            this->propagate_down(left);
        } else {
            // std::cout << "right better\n";
            std::swap(m_min.at(right), m_min.at(idx));
            this->propagate_down(right);
        }
    }
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    std::vector<int> test_permutation;
    for (int i = 0; i < 1010; ++i) {
        test_permutation.push_back(i);
    }

    for (int test = 0; test < 1000; ++test) {
        // if (test % 10 == 0) {
        //     std::cout << "Test NO " << test << std::endl;
        // }
        std::random_shuffle(test_permutation.begin(), test_permutation.end());
        MinMaxHeap heap;
        for (const auto n : test_permutation) {
            // std::cout << "inserting: " << n << "\n";
            heap.insert(n);
        }
        for (const auto n : test_permutation) {
            // std::cout << "Deleting max.. " << "\n";
            heap.delete_max();
        }
    }
    return 0;
}
