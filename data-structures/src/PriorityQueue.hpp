#ifndef PRIORITYQUEUE_HPP
#define PRIORITYQUEUE_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace bork_lib
{

enum class HeapType
{
    min, max
};

template <typename T>
class PriorityQueue
{
private:
    struct PQObject
    {
        T object;
        int priority;
        PQObject(T object, int priority) : object{std::move(object)}, priority{priority} {}
        friend bool operator< (const PQObject& lhs, const PQObject& rhs) { return lhs.priority < rhs.priority; }
        friend bool operator> (const PQObject& lhs, const PQObject& rhs) { return  operator< (rhs,lhs); }
    };

    static constexpr std::size_t default_length = 8;
    std::size_t length;   // the amount of memory allocated for the heap
    std::size_t sz;       // the number of objects in the heap
    HeapType heap_type;
    PQObject* heap;
    void* raw_memory;
    std::size_t parent(std::size_t index) { return (index - 1) / 2; }
    std::size_t left(std::size_t index) { return 2 * index + 1; }
    std::size_t right(std::size_t index) { return 2 * index + 2; }
    template<typename Compare> void heapify(std::size_t index, Compare comp);
    template<typename Compare> void restore_heap(Compare comp);
    void increase_length();
    void decrease_length();
    void reallocate_heap_memory();
    void allocate_heap_memory(std::size_t new_length, void*& raw_mem, PQObject*& typed_mem);
    void swap_heap(PriorityQueue<T>& temp_heap);

public:
    explicit PriorityQueue(std::size_t length = default_length, HeapType type = HeapType::max);
    PriorityQueue(const PriorityQueue<T>& other);
    PriorityQueue(PriorityQueue<T>&& other) noexcept;
    PriorityQueue<T>& operator=(const PriorityQueue<T>& other);
    PriorityQueue<T>& operator=(PriorityQueue<T>&& other) noexcept;
    ~PriorityQueue() { clear(); }
    void insert(T obj, int priority);
    std::pair<T, int> extreme() const;
    std::pair<T, int> extract();
    std::size_t size() const noexcept { return sz; }
    bool empty() const noexcept { return sz == 0; }
    HeapType type() const noexcept { return heap_type; }
    void clear();
};

/* Maintains the heap property after an element has been extracted from the heap. */
template<typename T>
template<typename Compare>
void PriorityQueue<T>::heapify(std::size_t index, Compare comp)
{
    auto left_child = left(index);
    auto right_child = right(index);
    auto extreme = index;
    if (left_child < sz && comp(heap[left_child], heap[index])) {
        extreme = left_child;
    }

    if (right_child < sz && comp(heap[right_child], heap[extreme])) {
        extreme = right_child;
    }

    if (extreme != index) {
        using std::swap;
        swap(heap[index], heap[extreme]);
        heapify(extreme, comp);
    }
}

/* Maintains the heap property after an element has been inserted into the heap. */
template<typename T>
template<typename Compare>
void PriorityQueue<T>::restore_heap(Compare comp)
{
    auto index = sz - 1;
    while (index > 0 && comp(heap[parent(index)],heap[index])) {
        using std::swap;
        swap(heap[index], heap[parent(index)]);
        index = parent(index);
    }
}

/* Doubles the length when more memory needs to be allocated for the heap. */
template<typename T>
void PriorityQueue<T>::increase_length()
{
    length ? length *= 2 : length += default_length;
    reallocate_heap_memory();
}

/* Reduces the length when the size of the heap is equal to the length divided by four. */
template<typename T>
void PriorityQueue<T>::decrease_length()
{
    length /= 2;
    reallocate_heap_memory();
}

/* Reallocates memory for the heap for a new value of length. */
template<typename T>
void PriorityQueue<T>::reallocate_heap_memory()
{
    void* new_raw_memory = nullptr;
    PQObject* new_heap = nullptr;
    allocate_heap_memory(length, new_raw_memory, new_heap);
    for (int i = 0; i < sz; ++i) {
        new(&new_heap[i]) PQObject(std::move(heap[i].object), heap[i].priority);
    }

    auto new_sz = sz;
    clear();
    sz = new_sz;
    raw_memory = new_raw_memory;
    heap = new_heap;
}

/* Allocates memory for the heap. */
template<typename T>
void PriorityQueue<T>::allocate_heap_memory(std::size_t new_length, void*& raw_mem, PQObject*& typed_mem)
{
    raw_mem = operator new[](new_length * sizeof(PQObject));
    typed_mem = static_cast<PQObject*>(raw_mem);
}

/* Swaps two heaps. */
template<typename T>
void PriorityQueue<T>::swap_heap(PriorityQueue<T>& temp_heap)
{
    using std::swap;
    swap(length, temp_heap.length);
    swap(sz, temp_heap.sz);
    swap(heap_type, temp_heap.heap_type);
    swap(heap, temp_heap.heap);
    swap(raw_memory, temp_heap.raw_memory);
}

/* General constructor. Takes in an optional length and the type of the heap. */
template<typename T>
PriorityQueue<T>::PriorityQueue(std::size_t length, HeapType type)
  : length{std::max(length, default_length)}, sz{0}, heap_type{type}
{
    allocate_heap_memory(this->length, raw_memory, heap);
}

/* Copy constructor. */
template<typename T>
PriorityQueue<T>::PriorityQueue(const PriorityQueue<T>& other) : length{other.length}, sz{other.sz}, heap_type{other.heap_type}
{
    allocate_heap_memory(length, raw_memory, heap);
    for (int i = 0; i < sz; ++i) {
        new(&heap[i]) PQObject(other.heap[i].object, other.heap[i].priority);
    }
}

/* Move constructor. */
template<typename T>
PriorityQueue<T>::PriorityQueue(PriorityQueue<T>&& other) noexcept : length{0}, sz{0}, heap_type{other.heap_type},
    heap{nullptr}, raw_memory{nullptr}
{
    swap_heap(other);
}

/* Copy assignment. Uses the copy-and-swap idiom. */
template<typename T>
PriorityQueue<T>& PriorityQueue<T>::operator=(const PriorityQueue<T>& other)
{
    if (this != &other) {
        auto temp = other;
        swap_heap(temp);
    }

    return *this;
}

/* Move assignment. */
template<typename T>
PriorityQueue<T>& PriorityQueue<T>::operator=(PriorityQueue<T>&& other) noexcept
{
    if (this != &other) {
        swap_heap(other);
    }

    return *this;
}

/* Inserts an object into the heap with a specified priority. */
template<typename T>
void PriorityQueue<T>::insert(T obj, int priority)
{
    if (sz == length) {
        increase_length();
    }

    ++sz;
    new(&heap[sz - 1]) PQObject(std::move(obj), priority);
    heap_type == HeapType::max ? restore_heap(std::less<PQObject>()) : restore_heap(std::greater<PQObject>());
}

/* Returns a copy of the max (for a max heap) or min (for a min heap) element along with its priority without removing it. */
template<typename T>
std::pair<T, int> PriorityQueue<T>::extreme() const
{
    if (sz) {
        return {heap[0].object, heap[0].priority};
    }

    throw std::out_of_range("No max or min when heap is empty.");
}

/* Returns a copy of the max (for a max heap) or min (for a min heap) element along with its priority and removes it. */
template<typename T>
std::pair<T, int> PriorityQueue<T>::extract()
{
    if (!sz) {
        throw std::out_of_range("No max or min when heap is empty.");
    }

    auto extreme = std::make_pair(heap[0].object, heap[0].priority);
    heap[0] = heap[sz - 1];
    --sz;
    if (sz == length / 4) {
        decrease_length();
    }

    heap_type == HeapType::max ? heapify(0, std::greater<PQObject>()) : heapify(0, std::less<PQObject>());
    return extreme;
}

/* Clears the heap. */
template<typename T>
void PriorityQueue<T>::clear()
{
    if (!raw_memory) {
        return;
    }

    for (int i = 0; i < sz; ++i) {
        heap[i].~PQObject();
    }

    operator delete[] (raw_memory);
    heap = nullptr;
    raw_memory = nullptr;
    sz = 0;
    length = 0;
}

}  // end namespace

#endif