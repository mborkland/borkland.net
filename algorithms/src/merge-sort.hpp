#ifndef MERGESORT_HPP
#define MERGESORT_HPP

#include <iterator>
#include <utility>
#include <vector>

namespace bork_lib
{
    template<typename ForwardIter>
    void merge(ForwardIter low, ForwardIter mid, ForwardIter high)
    {
        using T = typename std::iterator_traits<ForwardIter>::value_type;
        std::vector<T> left{low, mid};
        std::vector<T> right{mid, high};
        auto left_iter = left.begin();
        auto right_iter = right.begin();

        auto output_iter = low;
        for (; left_iter != left.end() && right_iter != right.end(); ++output_iter) {
            if (*left_iter <= *right_iter) {
                *output_iter = *left_iter;
                ++left_iter;
            } else {
                *output_iter = *right_iter;
                ++right_iter;
            }
        }

        std::move(left_iter, left.end(), output_iter);
        std::move(right_iter, right.end(), output_iter);
    }

    template<typename ForwardIter>
    void merge_sort(ForwardIter low, ForwardIter high)
    {
        auto dist = std::distance(low, high);
        if (dist == typename ForwardIter::difference_type{1}) {
            return;
        }

        auto mid = std::next(low, dist / 2);
        merge_sort(low, mid);
        merge_sort(mid, high);
        merge(low, mid, high);
    }
}

#endif
