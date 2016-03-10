#include "printer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
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

    std::pair<size_t, size_t> get_children(size_t idx)
    {
        int left  = 2 * idx + 1;
        int right = 2 * idx + 2;
        auto filter = [this] (int idx) {
            return idx < m_min.size() ? idx : UNDEFINED;
        };
        return std::make_pair(filter(left), filter(right));
    }

    void balance_up(size_t idx);
    void propagate_up(size_t child);
    void propagate_down(size_t idx);

    static size_t smaller(const std::vector<int>& vec, size_t idx1, size_t idx2);
    int pop_tail();

    void check_invariants() const;

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
    assert(m_min.size() == m_max.size() || m_min.size() - m_max.size() == 1);

    assert(this->linear_max() == this->max());
    assert(this->linear_min() == this->min());

    for (size_t i = 0; i < m_max.size(); ++i) {
        assert(m_max.at(i) > m_min.at(i));
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
    } else {
        idx = m_max.size();
        m_max.push_back(x);
    }
    this->balance_up(idx);

    assert(m_min.size() - m_max.size() <= 1);
    show();
    std::cout << "\n";
    this->check_invariants();
}

void
MinMaxHeap::balance_up(size_t idx)
{
    while (idx != UNDEFINED) {
        if (idx < m_max.size() && m_max.at(idx) < m_min.at(idx)) {
            std::swap(m_max.at(idx), m_min.at(idx));
        }

        this->propagate_up(idx);
        idx = this->get_parent(idx);
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
    assert(m_min.size() >= 3);
    m_min.front() = this->pop_tail();
    if (m_min.at(1) < m_min.at(2)) {
        std::swap(m_min.front(), m_min.at(1));
        this->propagate_down(1);
    } else {
        std::swap(m_min.front(), m_min.at(2));
        this->propagate_down(2);
    }


    show();
    this->check_invariants();
}

size_t
MinMaxHeap::smaller(const std::vector<int>& vec, size_t idx1, size_t idx2)
{
    return vec.at(idx1) < vec.at(idx2) ? idx1 : idx2;
}

void
MinMaxHeap::propagate_down(size_t idx)
{
    auto children = this->get_children(idx);
    int left  = children.first;
    int right = children.second;

    if (left == UNDEFINED) {
        std::cout << "Balancing up\n";
        this->balance_up(idx);
    } else if (right == UNDEFINED) {
        std::cout << "Balancing up2\n";
        this->balance_up(left);
    } else {
        if (m_min.at(left) > m_min.at(idx) && m_min.at(right) > m_min.at(idx)) {
            std::cout << "Both worse\n";
            this->balance_up(idx);
        } else if (m_min.at(left) < m_min.at(idx)) {
            std::cout << "left better\n";
            std::swap(m_min.at(left), m_min.at(idx));
            this->propagate_down(left);
        } else if (m_min.at(right) > m_min.at(idx)) {
            std::cout << "right better\n";
            std::swap(m_min.at(right), m_min.at(idx));
            this->propagate_down(right);
        } else {
            std::cout << "both better\n";
            size_t smaller = this->smaller(m_min, left, right);
            std::swap(m_min.at(smaller), m_min.at(idx));
            this->propagate_down(smaller);
        }
    }
}

int main(int argc, char const *argv[])
{
    std::vector<int> test_permutation;
    for (int i = 0; i < 30; ++i) {
        test_permutation.push_back(i);
    }

    for (int test = 0; test < 1; ++test) {
        // if (test % 100 == 0) {
        //     std::cout << "Test NO " << test << std::endl;
        // }
        std::random_shuffle(test_permutation.begin(), test_permutation.end());
        MinMaxHeap heap;
        for (const auto n : test_permutation) {
            std::cout << "inserting: " << n << "\n";
            heap.insert(n);
        }
        for (const auto n : test_permutation) {
            std::cout << "Deleting min.. " << "\n";
            heap.delete_min();
        }
    }
    return 0;
}
