
template <typename T>
T
MinMaxHeap<T>::min() const
{
    assert(m_min.size() + m_max.size() != 0);
    assert(m_min.size() != 0);
    return m_min.front();
}

template <typename T>
T
MinMaxHeap<T>::max() const
{
    assert(m_min.size() + m_max.size() != 0);
    if (m_max.size() != 0) {
        return m_max.front();
    } else {
        return m_min.front();
    }
}

template <typename T>
void
MinMaxHeap<T>::insert(T x)
{
    assert(m_min.size() - m_max.size() <= 1);

    if (m_max.size() != 0 && this->max() < x) {
        T old_max = this->max();
        m_max.front() = x;
        this->insert(old_max);
        return;
    }// append

    size_t idx;
    if (m_min.size() == m_max.size()) {
        idx = m_min.size();
        m_min.push_back(x);
        this->heapify_up<Min<T>>(idx);
    } else {
        idx = m_max.size();
        m_max.push_back(x);

        this->heapify_up<Max<T>>(idx);
        if (m_max.at(idx) < m_min.at(idx)) {
            std::swap(m_max.at(idx), m_min.at(idx));
            this->heapify_up<Min<T>>(idx);
            this->heapify_up<Max<T>>(idx);
        }

    }

    assert(m_min.size() - m_max.size() <= 1);
#ifndef DEBUG
    this->check_invariants();
#endif
}

template <typename T>
T
MinMaxHeap<T>::deleteMin()
{

    if (m_min.size() == 0) {
        return std::numeric_limits<T>::min();
    }
    if (m_min.size() == 1 && m_max.size() == 0) {
        return this->pop_tail();
    }

    T min = this->pop_tail();
    m_min.front() = min;
    this->heapify_down<Min<T>>(0);

#ifndef DEBUG
    this->check_invariants();
#endif
    return min;
}

template <typename T>
T
MinMaxHeap<T>::deleteMax()
{
    if (m_min.size() == 0) {
        return std::numeric_limits<T>::max();
    }
    if (m_min.size() == 1) {
        return this->pop_tail();
    }

    T max = this->pop_tail();
    m_max.front() = max;
    this->heapify_down<Max<T>>(0);

#ifndef DEBUG
    this->check_invariants();
#endif
    return max;
}

template <typename T>
size_t
MinMaxHeap<T>::get_parent(size_t idx)
{
    if (idx == 0) {
        return UNDEFINED;
    }
    return std::floor((idx - 1) / 2.);
}

template <typename T>
template <class Tail>
std::pair<size_t, size_t>
MinMaxHeap<T>::get_children(size_t idx) const
{
    const auto& heap = this->get_heap(Tail());
    size_t left  = 2 * idx + 1;
    size_t right = 2 * idx + 2;
    auto filter = [&] (size_t idx) {
        return idx < heap.size() ? idx : UNDEFINED;
    };
    return std::make_pair(filter(left), filter(right));
}


template <typename T>
std::vector<T>&
MinMaxHeap<T>::get_heap(Min<T>)
{
    return m_min;
}

template <typename T>
std::vector<T>&
MinMaxHeap<T>::get_heap(Max<T>)
{
    return m_max;
}

template <typename T>
const std::vector<T>&
MinMaxHeap<T>::get_heap(Min<T>) const
{
    return m_min;
}

template <typename T>
const std::vector<T>&
MinMaxHeap<T>::get_heap(Max<T>) const
{
    return m_max;
}

template <typename T>
T
MinMaxHeap<T>::pop_tail()
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

template <typename T>
template <class Tail>
void
MinMaxHeap<T>::heapify_up(size_t idx)
{
    auto& heap = this->get_heap(Tail());
    size_t parent = get_parent(idx);
    while (parent != UNDEFINED) {
        if (Tail::greater(heap.at(idx), heap.at(parent))) {
            std::swap(heap.at(parent), heap.at(idx));
        }
        idx    = parent;
        parent = get_parent(idx);
    }
}

template <typename T>
template <class Tail>
void
MinMaxHeap<T>::heapify_down(size_t idx)
{
    auto& heap    = this->get_heap(Tail());
    auto children = this->get_children<Tail>(idx);
    size_t left  = children.first;
    size_t right = children.second;

    if (left == UNDEFINED) {
        this->balance_up(idx);
    } else if (right == UNDEFINED) {
        if (Tail::greater(heap.at(left), heap.at(idx))) {
            std::swap(heap.at(left), heap.at(idx));
        }
        this->balance_up(left);
    } else {
        auto dir = this->get_direction<Tail>(idx);
        if (dir) {
            std::swap(heap.at(*dir), heap.at(idx));
            this->heapify_down<Tail>(*dir);
        }
    }
}

template <typename T>
template <class Tail>
boost::optional<size_t>
MinMaxHeap<T>::get_direction(size_t parent) const
{
    const auto& heap = this->get_heap(Tail());
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

template <typename T>
void
MinMaxHeap<T>::balance_up(size_t idx)
{
    if (idx >= m_max.size()) {
        return;
    }
    if (m_min.at(idx) > m_max.at(idx)) {
        std::swap(m_min.at(idx), m_max.at(idx));
        this->heapify_up<Min<T>>(idx);
        this->heapify_up<Max<T>>(idx);
    }
}

template <typename T>
T
MinMaxHeap<T>::linear_max() const
{
    T int_min = std::numeric_limits<T>::min();
    auto bin = [] (T a, T b) {
        return std::max(a, b);
    };
    return std::accumulate(begin(m_min), end(m_min),
        std::accumulate(begin(m_max), end(m_max), int_min, bin),
        bin);
}

template <typename T>
T
MinMaxHeap<T>::linear_min() const
{
    int int_min = std::numeric_limits<T>::max();
    auto bin = [] (T a, T b) {
        return std::min(a, b);
    };
    return std::accumulate(begin(m_min), end(m_min),
        std::accumulate(begin(m_max), end(m_max), int_min, bin),
        bin);
}

template <typename T>
void
MinMaxHeap<T>::check_invariants() const
{
    assert(m_min.size() == m_max.size() || m_min.size() - m_max.size() == 1);

    assert_heap_invariant();

    assert(this->linear_max() == this->max());
    assert(this->linear_min() == this->min());

    for (size_t i = 0; i < m_max.size(); ++i) {
        assert(m_max.at(i) >= m_min.at(i));
    }
}

template <typename T>
void
MinMaxHeap<T>::assert_heap_invariant() const
{
    for (size_t idx = 0; idx < m_min.size(); ++idx) {
        auto children = this->get_children<Min<T>>(idx);
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
        auto children = this->get_children<Max<T>>(idx);
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

template <typename T>
void
MinMaxHeap<T>::show() const
{
    // std::cout << "==================MIN==================\n";
    // print_tree(m_min);
    // std::cout << "==================MAX==================\n";
    // print_tree(m_max);
}
