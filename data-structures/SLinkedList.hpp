#ifndef SLINKEDLIST_HPP
#define SLINKEDLIST_HPP

#include "../NodeIterator/NodeIterator.hpp"
#include <stdexcept>
#include <initializer_list>
#include <utility>
#include <algorithm>
#include <stack>
#include <iostream>

namespace bork_lib
{

template<typename T>
using NodeType = typename SLinkedList<T>::SNode;

template<typename T>
class SLinkedList
{
private:
    struct SNode
    {
        T val;
        SNode* next;
        explicit SNode(T val = {}, SNode* next = nullptr)
                     : val{std::move(val)}, next{next} {}
    };
    
    SNode* head;
    bool srtd;         // the list is guaranteed to be sorted if true
    std::size_t sz;         // size of the list
    void insert_empty(T&& val);
    void mergesort(SNode*& head_ref, std::size_t size);
    SNode* merge(SNode*& head_left, SNode*& head_right, std::size_t right_size);
    SNode* search_front(const T& val) const noexcept;
    void swap(T& a, T& b) noexcept;

public:
    explicit SLinkedList(std::size_t num_elems = 0, const T& val = {});
    SLinkedList(const SLinkedList<T>& other);   // copy constructor
    SLinkedList(SLinkedList<T>&& other) noexcept;  // move constructor
    template<typename InputIterator>
    SLinkedList(InputIterator begin, InputIterator end);  // construct from iterator range
    SLinkedList(std::initializer_list<T> li);   // initializer list constructor
    ~SLinkedList() { delete_list(); }
    SLinkedList<T>& operator=(const SLinkedList<T>& other);  // copy assignment
    SLinkedList<T>& operator=(SLinkedList<T>&& other) noexcept; // move assignment
    inline bool empty() const noexcept { return !head; }    // is the list empty?
    inline bool sorted() const noexcept { return srtd; }    // is the list sorted?
    NodeIterator<NodeType<T>, T>& insert_before(NodeIterator<NodeType<T>, T>& iter, T val);
    NodeIterator<NodeType<T>, T>& insert_after(NodeIterator<NodeType<T>, T>& iter, T val);
    void insert_sorted(T val);
    NodeIterator<NodeType<T>, T>& delete_node(NodeIterator<NodeType<T>, T>& iter);
    void delete_list() noexcept;
    void push_front(T val);
    inline void pop_front() { ForwardListIterator<T> iter{head}; delete_node(iter); }   // remove node from front
    void push_back(T val);
    void pop_back();
    ForwardListIterator<T> find_front(const T& val) noexcept { return std::move(ForwardListIterator<T>{search_front(val)}); } // start search from the head
    ConstForwardListIterator<T> find_front(const T& val) const noexcept { return std::move(ConstForwardListIterator<T>{search_front(val)}); }
    std::size_t count(const T& val) const noexcept;
    inline std::size_t size() const noexcept { return sz; }    // how many nodes in the list?
    void sort();

    ForwardListIterator<T> begin() noexcept { return ForwardListIterator<T>{head}; }
    ForwardListIterator<T> end() noexcept { return ForwardListIterator<T>{nullptr}; }
    ConstForwardListIterator<T> cbegin() const noexcept { return ConstForwardListIterator<T>{head}; }
    ConstForwardListIterator<T> cend()  const noexcept { return ConstForwardListIterator<T>{nullptr}; }
    friend class ForwardListIterator<T>;
    friend class ConstForwardListIterator<T>;
};

/* General constructor: can be used to create a list with
   any number of elements, all with the same value. */
template<typename T>
SLinkedList<T>::SLinkedList(std::size_t num_elems, const T& val)
 : head{nullptr}, srtd{num_elems <= 1}, sz{num_elems}
{
    for (std::size_t i = 0; i < num_elems; ++i)
        push_front(val);
}

/* Copy constructor: create a list as a copy of another. */
template<typename T>
SLinkedList<T>::SLinkedList(const SLinkedList<T>& other)
 : head{nullptr}, sz{0}
{
    std::stack<T> st;
    auto node = other.head;
    while (node)
    {
        st.push(node->val);
        node = node->next;
    }

    while (!st.empty())
    {
        push_front(st.top());
        st.pop();
    }

    srtd = other.srtd;
}

/* Move constructor: create a list from an rvalue. */
template<typename T>
SLinkedList<T>::SLinkedList(SLinkedList<T>&& other) noexcept
 : head{other.head}, srtd{other.srtd}, sz{other.sz}
{
    other.head = nullptr;
}

/* Construct a list from an iterator range. */
template<typename T>
template<typename InputIterator>
SLinkedList<T>::SLinkedList(InputIterator begin, InputIterator end)
 : SLinkedList()
{
    while (begin != end)
        push_front(*begin++);
}

/* Initializer list constructor: create a list from an initializer list. */
template<typename T>
SLinkedList<T>::SLinkedList(std::initializer_list<T> li)
 : SLinkedList(li.rbegin(), li.rend())
{
    sz = li.size();
}

/* Copy assignment: assign a list to another list by copying it. */
template<typename T>
SLinkedList<T>& SLinkedList<T>::operator=(const SLinkedList<T>& other)
{
    if (&other == this)
        return *this;

    SLinkedList<T> new_list{other};
    return *this = std::move(new_list);
}

/* Move assignment: assign to a list from an rvalue. */
template<typename T>
SLinkedList<T>& SLinkedList<T>::operator=(SLinkedList<T>&& other) noexcept
{
    std::swap(head, other.head);
    srtd = other.srtd;
    sz = other.sz;
    return *this;
}

/* Insert the first node in the list. */
template<typename T>
void SLinkedList<T>::insert_empty(T&& val)
{
    head = new SNode{std::forward<T>(val)};
    sz = 1;
}

/* Insert before a node in the list. Returns an iterator
   pointing to the node after the inserted node (AKA 
   the original node passed to the function). */
template<typename T>
NodeIterator<NodeType<T>, T>& SLinkedList<T>::insert_before(NodeIterator<NodeType<T>, T>& iter, T val)
{
    if (empty())  // empty list
    {
        insert_empty(std::move(val));
        iter.node = head;
        return iter;
    }

    auto node = iter.node;
    if (!node)
        throw std::invalid_argument("Non-empty list pointer can't be null.");

    ++sz;
    srtd = false;
    auto new_node = new SLinkedList<T>::SNode(std::move(node->val), node->next);
    node->next = new_node;
    node->val = std::move(val);
    return ++iter;
}

/* Insert after a node in the list. Returns an iterator pointing to
   the inserted node. */
template<typename T>
NodeIterator<NodeType<T>, T>& SLinkedList<T>::insert_after(NodeIterator<NodeType<T>, T>& iter, T val)
{
    if (empty())  // empty list
    {
        insert_empty(std::move(val));
        iter.node = head;
        return iter;
    }

    auto node = iter.node;
    if (!node)
        throw std::invalid_argument("Non-empty list pointer can't be null.");

    ++sz;
    srtd = false;
    auto new_node = new SLinkedList<T>::SNode(std::move(val), node->next);
    node->next = new_node;
    return ++iter;
}

template<typename T>
void SLinkedList<T>::insert_sorted(T val)
{
    sort();   // won't sort if already sorted
    if (empty())
    {
        insert_empty(std::move(val));
        return;
    }
    
    ForwardListIterator<T> iter{head};
    if (val <= head->val)
    {
        push_front(std::move(val));
        srtd = true;
        return;
    }

    ForwardListIterator<T> iter_next{head->next};
    while (iter_next != end())
    {
        if (*iter_next > val)
            break;
        iter = iter_next;
        ++iter_next;
    }

    insert_after(iter, std::move(val));
    srtd = true;
}

/* Delete a node from the list. Returns an iterator pointing
   to the element after the deleted node. */
template<typename T>
NodeIterator<NodeType<T>, T>& SLinkedList<T>::delete_node(NodeIterator<NodeType<T>, T>& iter)
{
    if (empty())
        throw std::out_of_range("Can't delete from empty list.");
    
    auto node = iter.node;
    if (!node)
        throw std::invalid_argument("Can't delete null pointer.");

    --sz;
    if (node == head && sz == 1)  // list has one element
    {
        head = nullptr;
    }
    else if (node == head)     // delete first element
    {
        head = node->next;
    }
    else if (!node->next)   // last element in list
    {
        pop_back();
        return ++iter;
    }
    else
    {
        node->val = std::move(node->next->val);
        auto hold = node->next;
        node->next = node->next->next;
        node = hold;
    }

    delete node;
    return ++iter;
}

/* Delete every node in the list. */
template<typename T>
void SLinkedList<T>::delete_list() noexcept
{
    auto node = head;
    while (node)
    {
        auto current = node;
        node = node->next;
        delete current;
    }
    
    head = nullptr;
    srtd = true;   // an empty list is sorted
    sz = 0;
}

/* Insert a node at the front of the list. */
template<typename T>
void SLinkedList<T>::push_front(T val)
{
    if (empty())
    {
        insert_empty(std::move(val));
        return;
    }
    
    ++sz;
    if (srtd && val > head->val)
        srtd = false;
    auto new_node = new SLinkedList<T>::SNode{std::move(val), head};
    head = new_node;
}

/* Insert a node at the end of the list. Linear
   time operation. */
template<typename T>
void SLinkedList<T>::push_back(T val)
{
    if (empty())
    {
        insert_empty(std::move(val));
        return;
    }

    auto node = head;
    while (node)
    {
        if (!node->next)
        {
            ForwardListIterator<T> iter{node};
            insert_after(iter, val);
            return;
        }
        node = node->next;
    }
}

/* Remove a node from the end of the list. Linear
   time operation. */
template<typename T>
void SLinkedList<T>::pop_back()
{
    if (empty())
        throw std::out_of_range("Can't delete from empty list!");

    --sz;
    auto node = head;
    auto next_node = head->next;
    if (!next_node)   // list has 1 node
    {
        delete node;
        head = nullptr;
        return;
    }

    while (true)
    {
        if (!next_node->next)
        {
            node->next = nullptr;
            delete next_node;
            --sz;
            return;
        }

        node = next_node;
        next_node = next_node->next;
    }
}

/* Search for a particular value in the list, starting
   from the front. */
template<typename T>
typename SLinkedList<T>::SNode* SLinkedList<T>::search_front(const T& val) const noexcept
{
    auto node = head;
    while (node)
    {
        if (node->val == val)
            break;
        node = node->next;
    }

    return node;
}

/* Count the number of occurrences of a particular
   item in the list. */
template<typename T>
std::size_t SLinkedList<T>::count(const T& val) const noexcept
{
    std::size_t count = 0;
    auto node = head;
    while (node)
    {
        if (node->val == val)
            ++count;
        node = node->next;
    }

    return count;
}

/* Utility swap function using std::move. */
template<typename T>
void SLinkedList<T>::swap(T& a, T& b) noexcept
{ 
    T temp{std::move(a)};
    a = std::move(b); 
    b = std::move(temp);
} 

/* Sorts a list in ascending order. */
template<typename T>
void SLinkedList<T>::sort()
{
    if (srtd)  // don't sort a sorted list
        return;

    mergesort(head, sz);
    srtd = true;
}

/* Mergesort implementation for a linked list. Splits the
   list in half by declaring pointers to the head node and
   to a node halfway down the list. Recursively calls itself
   on the two halves, then merges the two halves using the
   merge function. */
template<typename T>
void SLinkedList<T>::mergesort(SNode*& head_ref, std::size_t size)
{
    if (size <= 1)  // already sorted
        return;

    auto head_left = head_ref;
    auto head_right = head_ref;
    std::size_t split = size / 2;
    for (std::size_t i = 0; i < split; ++i)   // split the list
        head_right = head_right->next;

    mergesort(head_left, split);                // split left half
    mergesort(head_right, size - split);        // split right half
    head_ref = merge(head_left, head_right, size - split);  // merge the two halves
}

/* The merge function. Merges two sorted lists
   in place by rearranging the next and prev pointers
   of the nodes. */
template<typename T>
typename SLinkedList<T>::SNode* SLinkedList<T>::merge(SLinkedList<T>::SNode*& head_left,
                         SLinkedList<T>::SNode*& head_right, std::size_t right_size)
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
        if (head_left->val <= head_right->val)
        {
            head_left = head_left->next;
        }
        /* When the next node to be merged is from the
        right sublist, put that node in front of the
        node pointed to by head_left. */
        else if (right_size == 1) // only one element in right partition left
        {                   // requires sequence of swaps
            --right_size;
            while (head_left != head_right)
            {
                using std::swap;
                swap(head_left->val, head_right->val);
                head_left = head_left->next;
            }
        }
        else
        {
            --right_size;
            auto hold = head_right->next;

            // put the node from the right sublist in the correct place
            using std::swap;
            swap(head_left->val, head_right->val);

            // move the value that was just swapped out back to where it should be
            swap(head_right->val, hold->val);
            head_right->next = head_right->next->next;
            hold->next = head_left->next;
            head_left->next = hold;
            head_left = head_left->next;
        }
    }

    return new_left;    // update leftmost node pointer
}

}  // end namespace

#endif
