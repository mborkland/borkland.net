#ifndef QUICKSORT_HOARE_HPP
#define QUICKSORT_HOARE_HPP

#include <iterator>
#include <iostream>

namespace bork_lib
{
    template<typename InputIterator>
    InputIterator partition(InputIterator low, InputIterator high)
    {
        auto pivot = low;
        auto i = low - 1;
        auto j = high;

        while (true)
        {
            do {
                ++i;
            } while (*i < *pivot);
            do {
                --j;
            } while (*j > *pivot);
            if (i >= j)
                return j;
            std::iter_swap(i, j);
        }
    }

    template<typename InputIterator>
    void quicksort_hoare(InputIterator low, InputIterator high)
    {
        if (high - 1 <= low)
            return;

        auto p = partition(low, high);
        quicksort_hoare(low, p + 1);
        quicksort_hoare(p + 1, high);
    }
}

#endif
