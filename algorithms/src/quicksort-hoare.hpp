#ifndef QUICKSORT_HOARE_HPP
#define QUICKSORT_HOARE_HPP

#include <iterator>

namespace bork_lib
{
    template<typename RandAccIter>
    RandAccIter partition(RandAccIter low, RandAccIter high)
    {
        auto pivot = low;
        auto i = std::prev(low);
        auto j = high;

        while (true) {
            do {
                ++i;
            } while (*i < *pivot);
            do {
                --j;
            } while (*j > *pivot);
            
            if (i >= j) {
                return j;
            }
            std::iter_swap(i, j);
        }
    }

    template<typename RandAccIter>
    void quicksort_hoare(RandAccIter low, RandAccIter high)
    {
        if (high - 1 <= low)
            return;

        auto p = partition(low, high);
        quicksort_hoare(low, p + 1);
        quicksort_hoare(p + 1, high);
    }
}

#endif