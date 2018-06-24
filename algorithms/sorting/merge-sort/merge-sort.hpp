#ifndef MERGESORT_HPP
#define MERGESORT_HPP

#include <iterator>
#include <vector>
#include <array>

namespace bork_lib
{
    template<typename InputIterator>
    void merge(InputIterator low, InputIterator mid, InputIterator high)
    {
        using T = typename std::iterator_traits<InputIterator>::value_type;
        std::vector<T> left{low, mid + 1};
        std::vector<T> right{mid + 1, high};
        auto left_iter = left.begin();
        auto right_iter = right.begin();

        for (auto it = low; it != high; ++it)
        {
            if (left_iter == left.end())
            {
                *it = *right_iter;
                ++right_iter;
            }
            else if (right_iter == right.end())
            {
                *it = *left_iter;
                ++left_iter;
            }
            else if (*left_iter <= *right_iter)
            {
                *it = *left_iter;
                ++left_iter;
            }
            else
            {
                *it = *right_iter;
                ++right_iter;
            }
        }
    }

    template<typename InputIterator>
    void merge_sort(InputIterator low, InputIterator high)
    {
        if (low >= high - 1)
            return;

        auto dist = std::distance(low, high) - 1;
        auto mid = low + (dist / 2);
        merge_sort(low, mid + 1);
        merge_sort(mid + 1, high);
        merge(low, mid, high);
    }
}

#endif
