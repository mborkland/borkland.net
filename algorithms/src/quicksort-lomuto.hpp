#ifndef QUICKSORT_LOMUTO_HPP
#define QUICKSORT_LOMUTO_HPP

#include <iterator>
#include <iostream>

namespace bork_lib
{
    template<typename RandAccIter>
    RandAccIter partition(RandAccIter low, RandAccIter high)
    {
        auto pivot = std::prev(high);
        auto i = std::prev(low);

        for (auto j = low; j != high - 1; ++j) {
            if (*j <= *pivot) {
                ++i;
                std::iter_swap(i, j);
            }
        }

        std::iter_swap(i + 1, high - 1);
        return i + 1;
    }

    template<typename RandAccIter>
    void quicksort_lomuto(RandAccIter low, RandAccIter high)
    {
        if (high - 1 <= low)
            return;

        auto p = partition(low, high);
        quicksort_lomuto(low, p);
        quicksort_lomuto(p + 1, high);
    }
}

#endif
