#ifndef DLINKEDLIST_HPP
#define DLINKEDLIST_HPP

#include "src/NodeIterator.hpp"
#include <stdexcept>
#include <initializer_list>
#include <utility>
#include <algorithm>
#include <src/DLinkedList.hpp>


namespace bork_lib
{

template<typename T>
using NodeType = typename DLinkedList<T>::DNode;

template<typename T>
class DLinkedList
{
private:
    struct DNode
    {
        T data;
        DNode* next;
        DNode* prev;
        explicit DNode(T data = {}, DNode* next = nullptr, DNode* prev = nullptr)
                     : data{std::move(data)}, next{next}, prev{prev} {}
    };

    DNode* head;
    DNode* tail;
    bool srtd;         // the list is guaranteed to be sorted if true
    std::size_t sz;         // size of the list
    void insert_empty(T&& data);
    void mergesort(DNode*& head_ref, std::size_t size);
    DNode* merge(DNode* head_left, DNode* head_right, std::size_t right_size);
    DNode* search_front(const T& data) noexcept;
    DNode* search_back(const T& data) noexcept;

public:
    explicit DLinkedList(std::size_t num_elems = 0, const T& data = {});
    DLinkedList(const DLinkedList<T>& other);   // copy constructor
    DLinkedList(DLinkedList<T>&& other) noexcept;        // move constructor
    template<typename InputIterator>
    DLinkedList(InputIterator begin, InputIterator end);  // construct from iterator range
    DLinkedList(std::initializer_list<T> li);   // initializer list constructor
    ~DLinkedList() { delete_list(); }
    DLinkedList<T>& operator=(const DLinkedList<T>& other);  // copy assignment
    DLinkedList<T>& operator=(DLinkedList<T>&& other) noexcept;       // move assignment
    inline bool empty() const noexcept { return !head; }    // is the list empty?
    inline bool sorted() const noexcept { return srtd; }    // is the list sorted?
    NodeIterator<NodeType<T>, T>& insert_before(NodeIterator<NodeType<T>, T>& iter, T data);
    NodeIterator<NodeType<T>, T>& insert_after(NodeIterator<NodeType<T>, T>& iter, T data);
    void insert_sorted(T data);
    NodeIterator<NodeType<T>, T>& delete_node(NodeIterator<NodeType<T>, T>& iter);
    void delete_list() noexcept;
    inline void push_front(const T& data) { ListIterator<T> iter{head}; insert_before(iter, data); }  // add node to front
    inline void pop_front() { ListIterator<T> iter{head}; delete_node(iter); }   // remove node from front
    inline void push_back(const T& data) { ListIterator<T> iter{tail}; insert_after(iter, data); }    // add node to back
    inline void pop_back() { ListIterator<T> iter{tail}; delete_node(iter); } // remove node from back
    ListIterator<T> find_front(const T& data) noexcept { return std::move(ListIterator<T>{search_front(data)}); } // start search from the head
    ReverseListIterator<T> find_back(const T& data) noexcept { return std::move(ReverseListIterator<T>{search_back(data)}); } // start search from the tail
    ConstListIterator<T> find_front(const T& data) const noexcept { return std::move(ConstListIterator<T>{search_front(data)}); }
    ConstReverseListIterator<T> find_back(const T& data) const noexcept { return std::move(ConstReverseListIterator<T>{search_front(data)}); }
    std::size_t count(const T& data) const noexcept;
    inline std::size_t size() const noexcept { return sz; }    // how many nodes in the list?
    void sort();

    ListIterator<T> begin() noexcept { return ListIterator<T>{head}; }
    ListIterator<T> end() noexcept { return ListIterator<T>{nullptr}; }
    ConstListIterator<T> cbegin() const noexcept { return ConstListIterator<T>{head}; }
    ConstListIterator<T> cend()  const noexcept { return ConstListIterator<T>{nullptr}; }
    ReverseListIterator<T> rbegin() noexcept { return ReverseListIterator<T>{tail}; }
    ReverseListIterator<T> rend() noexcept { return ReverseListIterator<T>{nullptr}; }
    ConstReverseListIterator<T> crbegin() const noexcept { return ConstReverseListIterator<T>{tail}; }
    ConstReverseListIterator<T> crend() const noexcept { return ConstReverseListIterator<T>{nullptr}; }
    friend class ListIterator<T>;
    friend class ConstListIterator<T>;
    friend class ReverseListIterator<T>;
    friend class ConstReverseListIterator<T>;
};

/* General constructor: can be used to create a list with
   any number of nodes, all with the same value. */
template<typename T>
DLinkedList<T>::DLinkedList(std::size_t num_elems, const T& data)
 : head{nullptr}, tail{nullptr}, srtd{num_elems <= 1}, sz{0}
{
    for (std::size_t i = 0; i < num_elems; ++i)
        push_back(data);
}

/* Copy constructor: create a list as a copy of another. */
template<typename T>
DLinkedList<T>::DLinkedList(const DLinkedList<T>& other)
 : head{nullptr}, tail{nullptr}, sz{0}
{
    auto node = other.head;
    while (node)
    {
        push_back(node->data);
        node = node->next;
    }

    srtd = other.srtd;
}

/* Move constructor: create a list from an rvalue. */
template<typename T>
DLinkedList<T>::DLinkedList(DLinkedList<T>&& other) noexcept
 : head{other.head}, tail{other.tail}, srtd{other.srtd}, sz{other.sz}
{
    other.head = nullptr;
    other.tail = nullptr;
}

/* Construct a list from an iterator range. */
template<typename T>
template<typename InputIterator>
DLinkedList<T>::DLinkedList(InputIterator begin, InputIterator end)
 : DLinkedList()
{
    while (begin != end)
        push_back(*begin++);
}

/* Initializer list constructor: create a list from an initializer list. */
template<typename T>
DLinkedList<T>::DLinkedList(std::initializer_list<T> li)
 : DLinkedList(li.begin(), li.end())
{
    sz = li.size();
}

/* Copy assignment: assign a list to another list by copying it. */
template<typename T>
DLinkedList<T>& DLinkedList<T>::operator=(const DLinkedList<T>& other)
{
    if (&other == this)
        return *this;

    std::size_t this_size = sz, new_size = other.sz;
    auto node = head;
    auto node_other = other.head;

    while (this_size <= new_size ? node : node_other) // copy the values up to the smaller size
    {
        node->data = node_other->data;
        node = node->next;
        node_other = node_other->next;
    }
    
    if (this_size < new_size)  // current list size is less than or equal to new list size
    {
        while (node_other)
        {
            push_back(node_other->data);
            node_other = node_other->next;
        }
    }

    if (this_size > new_size)  // current list is larger than the new list
    {
        std::size_t size_diff = this_size - new_size;
        for (std::size_t i = 0; i < size_diff; ++i)
            pop_back();
    }

    srtd = other.srtd;
    return *this;
}

/* Move assignment: assign to a list from an rvalue. */
template<typename T>
DLinkedList<T>& DLinkedList<T>::operator=(DLinkedList<T>&& other) noexcept
{
    std::swap(head, other.head);
    std::swap(tail, other.tail);
    srtd = other.srtd;
    sz = other.sz;
    return *this;
}

    template<typename ValueType>
    void DLinkedList<ValueType>::mergesort(NodeType *&head_ref, size_type size) {

    }

/* Insert the first node in the list. */
template<typename T>
void DLinkedList<T>::insert_empty(T&& data)
{
    head = tail = new DNode{std::forward<T>(data)};
    sz = 1;
}

/* Insert before a node in the list. Returns an iterator
   pointing to the node after the inserted node (AKA 
   the original node passed to the function). */
template<typename T>
NodeIterator<NodeType<T>, T>& DLinkedList<T>::insert_before(NodeIterator<NodeType<T>, T>& iter, T data)
{    
    if (empty())  // empty list
    {
        insert_empty(std::move(data));
        iter.node = head;
        return iter;
    }

    auto node = iter.node;
    if (!node)
        throw std::invalid_argument{"Non-empty list pointer can't be null."};

    ++sz;
    srtd = false;
    auto new_node = new DLinkedList<T>::DNode{std::move(data), node, node->prev};
    node->prev = new_node;

    if (node == head)   // insert at front of list
        head = new_node;
    else
        new_node->prev->next = new_node;

    return iter;
}

/* Insert after a node in the list. Returns an iterator pointing to
   the inserted node. */
template<typename T>
NodeIterator<NodeType<T>, T>& DLinkedList<T>::insert_after(NodeIterator<NodeType<T>, T>& iter, T data)
{
    if (empty())  // empty list
    {
        insert_empty(std::move(data));
        iter.node = head;
        return iter;
    }

    auto node = iter.node;
    if (!node)
        throw std::invalid_argument{"Non-empty list pointer can't be null."};

    ++sz;
    srtd = false;
    auto new_node = new DLinkedList<T>::DNode{std::move(data), node->next, node};
    node->next = new_node;

    if (node == tail)   // insert at back of list
        tail = new_node;
    else
        new_node->next->prev = new_node;

    return ++iter;
}

template<typename T>
void DLinkedList<T>::insert_sorted(T data)
{
    sort();   // won't sort if already sorted
    if (empty())
    {
        insert_empty(std::move(data));
        return;
    }

    auto node = head;
    while (node)
    {
        if (node->data > data)
            break;
        node = node->next;
    }

    if (node)
    {
        ListIterator<T> iter{node};
        insert_before(iter, std::move(data));
    }
    else
    {
        ListIterator<T> iter{tail};
        insert_after(iter, std::move(data));
    }
    srtd = true;
}

/* Delete a node from the list. Returns an iterator pointing
   to the element after the deleted node. */
template<typename T>
NodeIterator<NodeType<T>, T>& DLinkedList<T>::delete_node(NodeIterator<NodeType<T>, T>& iter)
{
    if (empty())
        throw std::out_of_range{"Can't delete from empty list."};
    
    auto node = iter.node;
    if (!node)
        throw std::invalid_argument{"Can't delete null pointer."};

    --sz;
    ++iter;
    if (node == head && node == tail)  // list has one element
    {
        head = tail = nullptr;
    }
    else if (node == head)   // delete first element
    {
        head = node->next;
        node->next->prev = nullptr;
    }
    else if (node == tail)  // delete last element
    {
        tail = node->prev;
        node->prev->next = nullptr;
    }
    else     // multi-element list where p is not the head or tail
    {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }
    
    delete node;
    node = nullptr;
    return iter;
}

/* Delete every node in the list. */
template<typename T>
void DLinkedList<T>::delete_list() noexcept
{
    auto it = begin();
    while (it != end())
        it = delete_node(it);

    head = nullptr;
    tail = nullptr;
    srtd = true;  // an empty list is sorted
}

/* Search for a particular value in the list, starting
   from the front. */
template<typename T>
typename DLinkedList<T>::DNode* DLinkedList<T>::search_front(const T& data) noexcept
{
    auto node = head;
    while (node)
    {
        if (node->data == data)
            break;
        node = node->next;
    }

    return node;
}

/* Search for a particular value in the list, starting
   from the back. */
template<typename T>
typename DLinkedList<T>::DNode* DLinkedList<T>::search_back(const T& data) noexcept
{
    auto node = tail;
    while (node)
    {
        if (node->data == data)
            break;
        node = node->prev;
    }

    return node;
}

/* Count the number of occurrences of a particular
   item in the list. */
template<typename T>
std::size_t DLinkedList<T>::count(const T& data) const noexcept
{
    std::size_t num = 0;
    auto node = head;
    while (node)
    {
        if (node->data == data)
            ++num;
        node = node->next;
    }

    return num;
}

/* Sorts a list in ascending order. */
template<typename T>
void DLinkedList<T>::sort()
{
    if (srtd)  // don't sort a sorted list
        return;

    mergesort(head, sz);   // sort the entire list
    srtd = true;
}

/* Mergesort implementation for a linked list. Splits the
   list in half by declaring pointers to the head node and
   to a node halfway down the list. Recursively sorts
   the two halves, then merges the two halves using the
   merge function. */
template<typename T>
void DLinkedList<T>::mergesort(DNode*& head_ref, std::size_t size)
{
    if (size <= 1)  // already sorted
        return;

    auto head_left = head_ref;
    auto head_right = head_ref;
    std::size_t split = size / 2;
    for (std::size_t i = 0; i < split; ++i)   // split the list
        head_right = head_right->next;

    mergesort(head_left, split);                // sort left half
    mergesort(head_right, size - split);        // sort right half
    head_ref = merge(head_left, head_right, size - split);  // merge the two halves
}

/* The merge function. Merges two sorted lists
   in place by rearranging the next and prev pointers
   of the nodes. */
template<typename T>
typename DLinkedList<T>::DNode* DLinkedList<T>::merge(DLinkedList<T>::DNode* head_left,
                         DLinkedList<T>::DNode* head_right, std::size_t right_size)
{
    if (!head_left || !head_right)
        return head_left;

    auto new_left = head_left;  // keep track of the leftmost node

    /* Explanation of the following while loop conditions:
       1. right_size keeps track of the number of unmerged nodes
          in the right sublist. When right_size == 0, the last node
          to be merged was in the right sublist and the sublists
          have been merged.
       2. If head_left == head_right, then all the nodes in the
          left sublist have been merged. Since the right sublist is
          already sorted, the merging is now complete. */
    while(right_size && (head_left != head_right))
    {
        /* When the next node to be merged is from the
        left sublist, simply move the head_left pointer
        to the next node. */
        if (head_left->data <= head_right->data)
        {
            head_left = head_left->next;
        }

        /* When the next node to be merged is from the
        right sublist, put that node in front of the
        node pointed to by head_left. */
        else
        {
            --right_size;
            auto current = head_right;          // the node currently being moved
            head_right = head_right->next;      // point to the next node to be merged

            // remove the node
            current->prev->next = current->next;
            if (current->next)
            {
                current->next->prev = current->prev;
            }
            else    // last node in list
            {
                tail = current->prev;
            }

            // insert the node
            current->prev = head_left->prev;
            current->next = head_left;
            if (head_left == new_left) // move in front of first node in left sublist
            {
                new_left = current;
            }
            if (head_left->prev)
            {
                head_left->prev->next = current;
            }
            head_left->prev = current;
        }
    }

    return new_left;    // update leftmost node pointer
}

}  // end namespace

#endif
