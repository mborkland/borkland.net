#ifndef PRIORITYQUEUE_HPP
#define PRIORITYQUEUE_HPP

#include <iostream>
#include <stdexcept>

enum class HeapType
{
  min, max
};

template<typename T>
struct PQObject
{
    T object;
    int priority;
    PQObject(T object = {}, int priority = {})
             : object(object), priority(priority) {}
    
    inline friend bool operator<(const PQObject &lhs, const PQObject &rhs)
                         { return lhs.priority < rhs.priority; }
    inline friend bool operator>(const PQObject &lhs, const PQObject &rhs)
                         { return rhs < lhs; }
    friend std::ostream &operator<<(std::ostream &os, const PQObject &pq)
    {
        os << pq.priority << ":" << pq.object;
    }
};

namespace PQUtils
{
  template<typename T>
  void swap(T &A, T &B)
  {
    T temp = A;
    A = B;
    B = temp;
  }

  size_t max(size_t A, size_t B)
  {
    if (A > B)
      return A;
    else
      return B;
  }
}

template <typename T>
class PriorityQueue
{
private:
  size_t heapLength;
  size_t heapSize;
  HeapType type;
  PQObject<T> *heap;
  void *raw_memory;
  inline size_t parent(size_t i) { return (i-1)/2; }
  inline size_t left(size_t i) { return 2*i+1; }
  inline size_t right(size_t i) { return 2*i+2; }
  void heapify(size_t i);
  void max_heapify(size_t i);
  void min_heapify(size_t i);
  void restore_max_heap();
  void restore_min_heap();
  void increase_length();

public:
  PriorityQueue(size_t heapLength = 8, HeapType type = HeapType::max);
  ~PriorityQueue();
  void insert(T obj, int priority);
  T extreme();
  T extract();
  void print(const char *delim = " ");
};

template<typename T>
PriorityQueue<T>::PriorityQueue(size_t heapLength, HeapType type)
    : heapLength(PQUtils::max(heapLength, 1)), heapSize(0),
      type(type)
{
    raw_memory = operator new[](heapLength * sizeof(PQObject<T>));
    heap = static_cast<PQObject<T>*>(raw_memory);
}

template<typename T>
PriorityQueue<T>::~PriorityQueue()
{
    for (int i = 0; i < heapSize; ++i)
    {
        heap[i].~PQObject<T>();
    }

    operator delete[] (raw_memory);
}

template<typename T>
void PriorityQueue<T>::insert(T obj, int priority)
{
  if (heapSize == heapLength)
  {
    increase_length();
  }

  ++heapSize;
  new(&heap[heapSize - 1]) PQObject<T>(obj, priority);

  if (type == HeapType::max)
  {
    restore_max_heap();
  }
  else
  {
    restore_min_heap();
  }
}

template<typename T>
void PriorityQueue<T>::restore_max_heap()
{
  size_t i = heapSize - 1;
  while (i > 0 && heap[parent(i)] < heap[i])
  {
    PQUtils::swap(heap[i], heap[parent(i)]);
    i = parent(i);
  }
}

template<typename T>
void PriorityQueue<T>::restore_min_heap()
{
  size_t i = heapSize - 1;
  while (i > 0 && heap[parent(i)] > heap[i])
  {
    PQUtils::swap(heap[i], heap[parent(i)]);
    i = parent(i);
  }
}

template<typename T>
T PriorityQueue<T>::extreme()
{
  if (heapSize)
    return heap[0];
  
  throw std::out_of_range("No max or min when heap is empty.");
}

template<typename T>
T PriorityQueue<T>::extract()
{
  if (!heapSize)
    throw std::out_of_range("No max or min when heap is empty.");
  T extreme = heap[0];
  heap[0] = heap[heapSize - 1];
  --heapSize;
  heapify(0);
  return extreme;
}

template<typename T>
void PriorityQueue<T>::heapify(size_t i)
{
  if (type == HeapType::max)
    max_heapify(i);
  else
    min_heapify(i);
}

template<typename T>
void PriorityQueue<T>::max_heapify(size_t i)
{
  size_t L = left(i), R = right(i), largest = i;
  if (L < heapSize && heap[L] > heap[i])
  {
    largest = L;
  }
  if (R < heapSize && heap[R] > heap[largest])
  {
    largest = R;
  }
  if (largest != i)
  {
    PQUtils::swap(heap[i], heap[largest]);
    max_heapify(largest);
  }
}

template<typename T>
void PriorityQueue<T>::min_heapify(size_t i)
{
  size_t L = left(i), R = right(i), smallest = i;
  if (L < heapSize && heap[L] < heap[i])
  {
    smallest = L;
  }
  if (R < heapSize && heap[R] < heap[smallest])
  {
    smallest = R;
  }
  if (smallest != i)
  {
    PQUtils::swap(heap[i], heap[smallest]);
    min_heapify(smallest);
  }
}

template<typename T>
void PriorityQueue<T>::increase_length()
{
    // TODO: fix this function
    heapLength *= 2;
    void *new_raw_memory = operator new[](heapLength * sizeof(PQObject<T>));
    PQObject<T> *new_heap = static_cast<PQObject<T>*>(new_raw_memory);
    for (int i = 0; i < heapSize; ++i)
    {
        std::cout << heap[i] << std::endl;
        new_heap[i] = heap[i];
    }

    for (int i = 0; i < heapSize; ++i)
    {
        heap[i].~PQObject<T>();
    }
    operator delete[] (raw_memory);
    heap = new_heap;
    raw_memory = new_raw_memory;
}

template<typename T>
void PriorityQueue<T>::print(const char *delim)
{
  for (int i = 0; i < heapSize; ++i)
  {
    std::cout << heap[i] << " ";
  }

  std::cout << std::endl;
}

#endif