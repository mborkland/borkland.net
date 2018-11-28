#ifndef HEAPSORT_HPP
#define HEAPSORT_HPP

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace bork_lib
{

template<typename Iter>
void max_heapify(Iter begin, std::size_t heap_size, Iter index)
{
    auto left = begin, right = begin, end = begin;
    auto largest = index;
    auto dist = std::distance(begin, index);
    std::advance(left, 2*dist + 1);
    std::advance(right, 2*dist + 2);
    std::advance(end, heap_size);

    if (left < end && *left > *index) {
        largest = left;
    }
    if (right < end && *right > *largest) {
        largest = right;
    }
    if (largest != index) {
        std::iter_swap(index, largest);
        max_heapify(begin, heap_size, largest);
    }
}

template<typename Iter>
void build_max_heap(Iter begin, Iter end)
{
    auto heap_size = static_cast<std::size_t>(std::distance(begin, end));
    auto rit = begin;
    std::advance(rit, heap_size / 2);
    for (;; --rit) {
        max_heapify(begin, heap_size, rit);
        if (rit == begin) {
            break;
        }
    }
}

template<typename Iter>
void heapsort(Iter begin, Iter end)
{
    build_max_heap(begin, end);
    auto heap_size = static_cast<std::size_t>(std::distance(begin, end));
    auto rit = end;
    --rit;
    for (; rit != begin; --rit) {
        std::iter_swap(begin, rit);
        --heap_size;
        max_heapify(begin, heap_size, begin);
    }
}

} // end namespace

#endif
