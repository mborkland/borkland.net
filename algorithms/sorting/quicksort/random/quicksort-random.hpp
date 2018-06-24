#ifndef QUICKSORT_RANDOM_HPP
#define QUICKSORT_RANDOM_HPP

#include <iterator>
#include <random>

namespace bork_lib
{
    template<typename InputIterator>
    InputIterator partition(InputIterator low, InputIterator high)
    {
        auto pivot = low;
        auto leftIndex = low - 1;
        auto rightIndex = high;

        while (true)
        {
            do {
                ++leftIndex;
            } while (*leftIndex < *pivot);
            do {
                --rightIndex;
            } while (*rightIndex > *pivot);
                if (leftIndex >= rightIndex)
                    return rightIndex;
            std::iter_swap(leftIndex, rightIndex);
        }
    }

    template<typename InputIterator>
    InputIterator randomized_partition(InputIterator low, InputIterator high)
    {
        std::default_random_engine re {};
        std::uniform_int_distribution<> dist{0, high - low - 1};
        auto pivot = low + dist(re);
        std::iter_swap(low, pivot);
        return partition(low, high);
    }

    template<typename InputIterator>
    void quicksort_random(InputIterator low, InputIterator high)
    {
        if (high - 1 <= low)
            return;

        auto p = randomized_partition(low, high);
        quicksort_random(low, p + 1);
        quicksort_random(p + 1, high);
    }
}

#endif
