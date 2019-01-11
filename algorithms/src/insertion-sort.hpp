#ifndef INSERTIONSORT_H
#define INSERTIONSORT_H

#include <iterator>

template<typename InputIterator>
void insertion_sort(InputIterator low, InputIterator high)
{
    using T = typename std::iterator_traits<InputIterator>::value_type;
    for (auto i = low + 1; i != high; ++i) {
        T key = *i;
        auto j = i - 1;
        for (; (j != low - 1) && (*j > key); --j) {
            *(j + 1) = *j;
        }

        *(j + 1) = key;
    }
}

#endif
