#ifndef SELECTIONSORT_H
#define SELECTIONSORT_H

#include <algorithm>
#include <iterator>

template<typename InputIterator>
void selection_sort(InputIterator low, InputIterator high)
{
    for (auto i = low; i != high; ++i) {
        using T = typename std::iterator_traits<InputIterator>::value_type;
        T min = *i;
        auto minIndex = i;

        for (auto j = i; j != high; ++j) {
            if (*j < min) {
                min = *j;
                minIndex = j;
            }
        }

        std::iter_swap(i, minIndex);
    }
}

#endif
