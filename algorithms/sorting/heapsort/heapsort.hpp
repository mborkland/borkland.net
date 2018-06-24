#ifndef HEAPSORT_HPP
#define HEAPSORT_HPP

#include <vector>
#include <iterator>

template<typename InputIterator>
void swap(InputIterator A, InputIterator B)
{
  using T = typename std::iterator_traits<InputIterator>::value_type;
  T temp = *A;
  *A = *B;
  *B = temp;
}

template<typename InputIterator>
void maxHeapify(InputIterator begin, size_t heapSize, InputIterator index)
{
  auto left = begin, right = begin, end = begin, largest = index;
  auto dist = std::distance(begin, index);
  std::advance(left, 2*dist + 1);
  std::advance(right, 2*dist + 2);
  std::advance(end, heapSize);

  if (left < end && *left > *index)
  {
    largest = left;
  }

  if (right < end && *right > *largest)
  {
    largest = right;
  }

  if (largest != index)
  {
    ::swap(index, largest);
    maxHeapify(begin, heapSize, largest);
  }
}

template<typename InputIterator>
void buildMaxHeap(InputIterator begin, InputIterator end)
{
  auto heapSize = std::distance(begin, end);
  for (auto it = begin + heapSize / 2; it != begin - 1; --it)
  {
    maxHeapify(begin, heapSize, it);
  }
}

template<typename InputIterator>
void heapsort(InputIterator begin, InputIterator end)
{
  buildMaxHeap(begin, end);
  auto heapSize = std::distance(begin, end);
  for (auto it = end - 1; it != begin; --it)
  {
    ::swap(begin, it);
    heapSize -= 1;
    maxHeapify(begin, heapSize, begin);
  }
}

#endif
