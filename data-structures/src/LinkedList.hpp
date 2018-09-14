#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include <algorithm>
#include <memory>
#include <type_traits>
#include <utility>
#include "Linkage.hpp"
#include "NodeIterator.hpp"

namespace bork_lib
{

template<typename, typename = void> struct can_be_sorted : std::false_type { };
template<typename T> struct can_be_sorted<T, decltype(void(std::declval<T&>() <= std::declval<T&>()))> : std::true_type { };

template<Linkage LinkageType, typename ValueType> struct ListNode;

template<Linkage LinkageType, typename ValueType>
class LinkedList {
protected:
    using node_type = ListNode<LinkageType, ValueType>;

public:
    using value_type = ValueType;
    using reference = ValueType&;
    using const_reference = const ValueType&;
    using pointer = ValueType*;
    using const_pointer = const ValueType*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = std::conditional_t<LinkageType == Linkage::SingleLinkage,
                     ForwardListIterator<ValueType>, ListIterator<ValueType>>;
    using const_iterator = std::conditional_t<LinkageType == Linkage::SingleLinkage,
                           ConstForwardListIterator<ValueType>, ConstListIterator<ValueType>>;

    using node_iterator = NodeIterator<node_type, value_type>;

protected:
    std::unique_ptr<node_type> head = std::unique_ptr<node_type>(nullptr);
    node_type* tail = nullptr;
    bool srtd = true;         // the list is guaranteed to be sorted if true
    size_type sz = 0;         // size of the list

    void list_swap(LinkedList<LinkageType, ValueType>& other);
    void swap(value_type& a, value_type& b);

    template<typename... Args> void emplace_empty(Args&&... args);
    node_type* find_sorted_position(const value_type& val);
    virtual node_type* insert_node_before(node_type *node, std::unique_ptr<node_type> &new_node, bool is_reverse) = 0;
    virtual node_type* insert_node_after(node_type *node, std::unique_ptr<node_type> &new_node, bool is_reverse) = 0;

    virtual node_type* delete_node(node_type* node, bool is_reverse) = 0;
    void delete_error_check();
    node_type* search_front(const value_type& val) const noexcept;
    template<typename InputIterator> void construct_from_iterator_range(InputIterator begin, InputIterator end);

public:
    // construction, assignment, and destruction
    LinkedList() = default;
    LinkedList(const LinkedList<LinkageType, value_type>& other) = delete;
    LinkedList(LinkedList<LinkageType, value_type>&& other) noexcept
     : head{std::move(other.head)}, tail{other.tail}, sz{other.sz}, srtd{other.srtd} { }
    LinkedList<LinkageType, ValueType>& operator=(const LinkedList<LinkageType, ValueType>& other);
    LinkedList<LinkageType, ValueType>& operator=(LinkedList<LinkageType, ValueType>&& other) noexcept;
    virtual ~LinkedList() noexcept { clear(); };

    bool empty() const noexcept { return !head; }
    size_type size() const noexcept { return sz; }
    bool sorted() const noexcept { return srtd; }
    reference front() const noexcept { return head->data; }
    reference back() const noexcept { return tail->data; }

    iterator insert_before(iterator iter, const value_type& val) { return emplace_before(iter, val); }
    const_iterator insert_before(const_iterator iter, const value_type& val) { return emplace_before(iter, val); }
    iterator insert_before(iterator iter, value_type&& val) { return emplace_before(iter, std::forward<value_type>(val)); }
    const_iterator insert_before(const_iterator iter, value_type&& val) { return emplace_before(iter, std::forward<value_type>(val)); }
    iterator insert_after(iterator iter, const value_type& val) { return emplace_after(iter, val); }
    const_iterator insert_after(const_iterator iter, const value_type& val) { return emplace_after(iter, val); }
    iterator insert_after(iterator iter, value_type&& val) { return emplace_after(iter, std::forward<value_type>(val)); }
    const_iterator insert_after(const_iterator iter, value_type&& val) { return emplace_after(iter, std::forward<value_type>(val)); }
    void insert_sorted(const value_type& val) { return emplace_sorted(val); }
    void insert_sorted(value_type&& val) { return emplace_sorted(std::forward<value_type>(val)); }

    template<typename... Args> iterator emplace_before(iterator iter, Args&&... args);
    template<typename... Args> const_iterator emplace_before(const_iterator iter, Args&&... args);
    template<typename... Args> iterator emplace_after(iterator iter, Args&&... args);
    template<typename... Args> const_iterator emplace_after(const_iterator iter, Args&&... args);
    template<typename... Args> void emplace_sorted(Args&&... args);

    void push_front(const value_type& val) { emplace_front(val); }
    void push_front(value_type&& val) { emplace_front(std::forward<value_type>(val)); }
    void push_back(const value_type& val) { emplace_back(val); }
    void push_back(value_type&& val) { emplace_back(std::forward<value_type>(val)); }

    template<typename... Args> void emplace_front(Args&&... args) { iterator iter{head.get()}; emplace_before(iter, std::forward<Args>(args)...); }
    template<typename... Args> void emplace_back(Args&&... args) { iterator iter{tail}; emplace_after(iter, std::forward<Args>(args)...); }

    void pop_front() { delete_node(head.get(), false); }
    void pop_back() { delete_node(tail, false); }

    iterator find(const value_type& val) const noexcept;
    size_type count(const value_type& val) const noexcept;
    iterator erase(iterator iter);
    const_iterator erase(const_iterator iter) { return static_cast<const_iterator>(erase(static_cast<iterator>(iter))); }
    void clear() noexcept;

    template<Linkage L, typename V>
    friend void list_sort(LinkedList<L, V>& linked_list);
    template<Linkage L, typename V>
    friend void list_mergesort(std::unique_ptr<ListNode<L,V>>& left_owner, ListNode<L, V>* tail, size_type size);
    template<typename V>
    friend void list_merge(std::unique_ptr<ListNode<Linkage::SingleLinkage, V>>& left_owner,
            ListNode<Linkage::SingleLinkage, V>* right_raw, size_type right_size, ListNode<Linkage::SingleLinkage, V>* tail);
    template<typename V>
    friend void list_merge(std::unique_ptr<ListNode<Linkage::DoubleLinkage, V>>& left_owner,
            ListNode<Linkage::DoubleLinkage, V>* right_raw, size_type right_size, ListNode<Linkage::DoubleLinkage, V>* tail);

    // iterator functions
    iterator begin() noexcept { return iterator{head.get()}; }
    const_iterator begin() const noexcept { return const_iterator{head.get()}; }
    iterator end() noexcept { return iterator{nullptr}; }
    const_iterator end() const noexcept { return const_iterator{nullptr}; };
    const_iterator cbegin() const noexcept { return const_iterator{head.get()}; }
    const_iterator cend()  const noexcept { return const_iterator{nullptr}; }

    friend class ListIterator<value_type>;
    friend class ConstListIterator<value_type>;
    friend class ReverseListIterator<value_type>;
    friend class ConstReverseListIterator<value_type>;
    friend class ForwardListIterator<value_type>;
    friend class ConstForwardListIterator<value_type>;
    friend class NodeIterator<node_type, value_type>;
};

template<typename ValueType>
struct ListNode<Linkage::SingleLinkage, ValueType>
{
private:
    ValueType data;
    std::unique_ptr<ListNode> next = std::unique_ptr<ListNode>(nullptr);

public:
    template<typename... Args, typename = std::enable_if_t<std::is_constructible_v<ValueType, Args&&...>>>
    explicit ListNode(Args&&... args) : data{std::forward<Args>(args)...} { }
    explicit ListNode(const ValueType& data) : data{data} { }
    explicit ListNode(ValueType&& data) : data{std::move(data)} { }

    template<Linkage L, typename V>
    friend void list_mergesort(std::unique_ptr<ListNode<L,V>>& left_owner, ListNode<L, V>* tail, std::size_t size);
    template<typename V>
    friend void list_merge(std::unique_ptr<ListNode<Linkage::SingleLinkage, V>>& left_owner,
            ListNode<Linkage::SingleLinkage, V>* right_raw, std::size_t right_size, ListNode<Linkage::SingleLinkage, V>* tail);

    friend class LinkedList<Linkage::SingleLinkage, ValueType>;
    friend class SLinkedList<ValueType>;
    friend class ForwardListIterator<ValueType>;
    friend class ConstForwardListIterator<ValueType>;
    friend class NodeIterator<ListNode<Linkage::SingleLinkage, ValueType>, ValueType>;
    friend class ConstNodeIterator<ListNode<Linkage::SingleLinkage, ValueType>, ValueType>;
};

template<typename ValueType>
struct ListNode<Linkage::DoubleLinkage, ValueType>
{
private:
    ValueType data;
    std::unique_ptr<ListNode> next = std::unique_ptr<ListNode>(nullptr);
    ListNode* prev = nullptr;

public:
    template<typename... Args, typename = std::enable_if_t<std::is_constructible_v<ValueType, Args&&...>>>
    explicit ListNode(Args&&... args) : data{std::forward<Args>(args)...} { }
    explicit ListNode(const ValueType& data) : data{data} { }
    explicit ListNode(ValueType&& data) : data{std::move(data)} { }

    template<Linkage L, typename V>
    friend void list_mergesort(std::unique_ptr<ListNode<L,V>>& left_owner, ListNode<L, V>* tail, std::size_t size);
    template<typename V>
    friend void list_merge(std::unique_ptr<ListNode<Linkage::DoubleLinkage, V>>& left_owner,
            ListNode<Linkage::DoubleLinkage, V>* right_raw, std::size_t right_size, ListNode<Linkage::DoubleLinkage, V>* tail);

    friend class LinkedList<Linkage::DoubleLinkage, ValueType>;
    friend class DLinkedList<ValueType>;
    friend class ListIterator<ValueType>;
    friend class ConstListIterator<ValueType>;
    friend class ReverseListIterator<ValueType>;
    friend class ConstReverseListIterator<ValueType>;
    friend class NodeIterator<ListNode<Linkage::DoubleLinkage, ValueType>, ValueType>;
    friend class ConstNodeIterator<ListNode<Linkage::DoubleLinkage, ValueType>, ValueType>;
};


template<Linkage LinkageType, typename ValueType>
LinkedList<LinkageType, ValueType>& LinkedList<LinkageType, ValueType>::operator=(const LinkedList<LinkageType, ValueType>& other)
{
    clear();
    for (const auto& x : other) {
        push_back(x);
    }

    srtd = other.srtd;
    return *this;
}

template<Linkage LinkageType, typename ValueType>
LinkedList<LinkageType, ValueType>& LinkedList<LinkageType, ValueType>::operator=(LinkedList<LinkageType, ValueType>&& other) noexcept
{
    list_swap(other);
    return *this;
}

template<Linkage LinkageType, typename ValueType>
void LinkedList<LinkageType, ValueType>::list_swap(LinkedList<LinkageType, ValueType>& other)
{
    auto temp_unique = std::move(head);
    head = std::move(other.head);
    other.head = std::move(temp_unique);

    using std::swap;
    swap(tail, other.tail);
    swap(sz, other.sz);
    swap(srtd, other.srtd);
}

template<Linkage LinkageType, typename ValueType>
void LinkedList<LinkageType, ValueType>::swap(value_type& a, value_type& b)
{
    value_type tmp{std::move(a)};
    a = std::move(b);
    b = std::move(tmp);
}

/* Helper function to insert an element in-place into an empty list. */
template<Linkage LinkageType, typename ValueType>
template<typename... Args>
void LinkedList<LinkageType, ValueType>::emplace_empty(Args&&... args)
{
    head = std::make_unique<node_type>(std::forward<Args>(args)...);
    tail = head.get();
    ++sz;
}

/* Helper function to find the correct position for inserting an element into a sorted list. */
template<Linkage LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::node_type* LinkedList<LinkageType, ValueType>::find_sorted_position(const value_type &val)
{
    auto node = head.get();
    while (node) {
        if (node->data >= val) {
            return node;
        }
        node = node->next.get();
    }

    return nullptr;
}

template<Linkage LinkageType, typename ValueType>
void LinkedList<LinkageType, ValueType>::delete_error_check()
{
    if (empty()) {
        throw std::out_of_range{"Can't delete from empty list."};
    }
}

/* Helper function that returns a pointer to the first node with the value specified. */
template<Linkage LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::node_type* LinkedList<LinkageType, ValueType>::search_front(const value_type& val) const noexcept
{
    auto node = head.get();
    while (node) {
        if (node->data == val) {
            return node;
        }
        node = node->next.get();
    }

    return nullptr;
}


template<Linkage LinkageType, typename ValueType>
template<typename InputIterator>
void LinkedList<LinkageType, ValueType>::construct_from_iterator_range(InputIterator begin, InputIterator end)
{
    while (begin != end) {
        push_back(*begin++);
    }
    srtd = std::is_sorted(begin, end);
}

/* Public function that inserts a value in-place before a node in the list. */
template<Linkage LinkageType, typename ValueType>
template<typename... Args>
typename LinkedList<LinkageType, ValueType>::iterator LinkedList<LinkageType, ValueType>::emplace_before(iterator iter, Args&&... args)
{
    if (empty()) {
        emplace_empty(std::forward<Args>(args)...);
        iter.node = head.get();
    } else {
        auto new_node = std::make_unique<node_type>(std::forward<Args>(args)...);
        iter.node = insert_node_before(iter.node, new_node, iter.is_reverse());
    }

    return iter;
}

/* Public function that inserts a value in-place before a node in the list. */
template<Linkage LinkageType, typename ValueType>
template<typename... Args>
typename LinkedList<LinkageType, ValueType>::const_iterator LinkedList<LinkageType, ValueType>::emplace_before(const_iterator iter, Args&&... args)
{
    return static_cast<const_iterator>(emplace_before(static_cast<iterator>(iter), std::forward<Args>(args)...));
}

/* Public function that inserts a value in-place after a node in the list. */
template<Linkage LinkageType, typename ValueType>
template<typename... Args>
typename LinkedList<LinkageType, ValueType>::iterator LinkedList<LinkageType, ValueType>::emplace_after(iterator iter, Args&&... args)
{
    if (empty()) {
        emplace_empty(std::forward<Args>(args)...);
        iter.node = head.get();
    } else {
        auto new_node = std::make_unique<node_type>(std::forward<Args>(args)...);
        iter.node = insert_node_after(iter.node, new_node, iter.is_reverse());
    }

    return iter;
}

/* Public function that inserts a value in-place after a node in the list. */
template<Linkage LinkageType, typename ValueType>
template<typename... Args>
typename LinkedList<LinkageType, ValueType>::const_iterator LinkedList<LinkageType, ValueType>::emplace_after(const_iterator iter, Args&&... args)
{
    return static_cast<const_iterator>(emplace_after(static_cast<iterator>(iter), std::forward<Args>(args)...));
}

/* Public function that inserts a value in-place into its correct position in a sorted list. */
template<Linkage LinkageType, typename ValueType>
template<typename... Args>
void LinkedList<LinkageType, ValueType>::emplace_sorted(Args&&... args)
{
    if (empty()) {
        emplace_empty(std::forward<Args>(args)...);
        return;
    }

    list_sort(*this);    // won't sort if already sorted
    auto new_node = std::make_unique<node_type>(std::forward<Args>(args)...);
    auto position = find_sorted_position(new_node->data);
    position ? insert_node_before(position, new_node, false) : insert_node_after(tail, new_node, false);
    srtd = true;
}

/* Public function that attempts to find a value within the list. */
template<Linkage LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::iterator LinkedList<LinkageType, ValueType>::find(const value_type &val) const noexcept
{
    auto node = head.get();
    while (node) {
        if (node->data == val) {
            break;
        }
        node = node->next.get();
    }

    return iterator{node};
};

/* Public function that counts the occurrences of a value in the list. */
template<Linkage LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::size_type LinkedList<LinkageType, ValueType>::count(const value_type& val) const noexcept
{
    auto node = head.get();
    size_type count = 0;
    while (node) {
        if (node->data == val) {
            ++count;
        }
        node = node->next.get();
    }

    return count;
}

/* Public function that erases a node pointed to by an iterator. */
template<Linkage LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::iterator LinkedList<LinkageType, ValueType>::erase(iterator iter)
{
    delete_error_check();
    iter.node = delete_node(iter.node, false);
    return iter;
}

/* Public function that clears a list. */
template<Linkage LinkageType, typename ValueType>
void LinkedList<LinkageType, ValueType>::clear() noexcept
{
    while (head) {
        head = std::move(head->next);
    }

    tail = nullptr;
    sz = 0;
}

/* Free sort function. Calls the list_mergesort helper function. */
template<Linkage LinkageType, typename ValueType>
void list_sort(LinkedList<LinkageType, ValueType>& linked_list)
{
    static_assert(can_be_sorted<ValueType>::value, "Type that does not support <= operator cannot be sorted.");

    if (linked_list.srtd) {
        return;
    }

    list_mergesort(linked_list.head, linked_list.tail, linked_list.sz);   // sort the entire list
    linked_list.srtd = true;
}

/* Helper function used to recursively sort and merge sublists. */
template<Linkage LinkageType, typename ValueType>
void list_mergesort(std::unique_ptr<ListNode<LinkageType, ValueType>>& left_owner, ListNode<LinkageType, ValueType>* tail, std::size_t size)
{
    if (size <= 1)  // already sorted
        return;

    using size_type = std::size_t;
    size_type split = size / 2;
    list_mergesort(left_owner, tail, split);                // sort left half
    auto node = left_owner.get();
    for (size_type i = 0; i < split - 1; ++i) {   // split the list
        node = node->next.get();
    }
    auto& right_owner = node->next;
    list_mergesort(right_owner, tail, size - split);        // sort right half
    list_merge(left_owner, right_owner.get(), size - split, tail);  // merge the two halves
}

/* Helper function that merges two sublists (mostly) in-place. Does make a few moves because of the lack of prev pointers
   in a singly-linked list. */
template<typename ValueType>
void list_merge(std::unique_ptr<ListNode<Linkage::SingleLinkage, ValueType>>& left_owner, ListNode<Linkage::SingleLinkage, ValueType>* right_raw,
                std::size_t right_size, ListNode<Linkage::SingleLinkage, ValueType>* tail)
{
    auto left_raw = left_owner.get();
    using std::swap;
    /* Explanation of the following while loop conditions:
       1. right_size keeps track of the number of unmerged nodes in the right sublist. When right_size == 0,
          the last node to be merged was in the right sublist and the sublists have been merged.
       2. If left_raw == right_raw, then all the nodes in the left sublist have been merged. Since the right sublist
          is already sorted, the merging is now complete. */
    while(right_size && left_raw != right_raw)
    {
        /* When the next node to be merged is from the left sublist, simply move the left_raw pointer
        to the next node. */
        if (left_raw->data <= right_raw->data) {
            left_raw = left_raw->next.get();
        }
            /* When the next node to be merged is from the right sublist, put that node in front of the
            node pointed to by left_raw. */
        else if (right_size == 1) {  // only one element in right partition left; requires sequence of swaps
            --right_size;
            while (left_raw != right_raw) {
                swap(left_raw->data, right_raw->data);
                left_raw = left_raw->next.get();
            }
        } else {
            --right_size;
            swap(left_raw->data, right_raw->data); // put the value from the right sublist in the correct place
            swap(right_raw->data, right_raw->next->data);  // move the next value in the right sublist ahead
            auto current = std::move(right_raw->next);   // put the value that was moved back where it should be
            right_raw->next = std::move(current->next);
            current->next = std::move(left_raw->next);
            left_raw->next = std::move(current);
            left_raw = left_raw->next.get();
        }
    }
}

/* Helper function that merges two DLinkedList sublists in-place. */
template<typename ValueType>
void list_merge(std::unique_ptr<ListNode<Linkage::DoubleLinkage, ValueType>>& left_owner, ListNode<Linkage::DoubleLinkage, ValueType>* right_raw,
        std::size_t right_size, ListNode<Linkage::DoubleLinkage, ValueType>* tail)
{
    auto left_raw = left_owner.get();

    /* Explanation of the following while loop conditions:
       1. right_size keeps track of the number of unmerged nodes in the right sublist. When right_size == 0,
          the last node to be merged was in the right sublist and the sublists have been merged.
       2. If left_raw == right_raw, then all the nodes in the left sublist have been merged. Since the right sublist
          is already sorted, the merging is now complete. */
    while(right_size && left_raw != right_raw)
    {
        /* When the next node to be merged is from the left sublist, simply move the left_raw pointer
        to the next node. */
        if (left_raw->data <= right_raw->data) {
            left_raw = left_raw->next.get();
        }
            /* When the next node to be merged is from the right sublist, put that node in front of the
            node pointed to by left_raw. */
        else {
            --right_size;
            auto current = std::move(right_raw->prev->next);          // the node currently being moved
            right_raw = right_raw->next.get();      // point to the next node to be merged

            // remove the node
            if (current->next) {
                current->next->prev = current->prev;
            } else {    // last node in list
                tail = current->prev;
            }
            current->prev->next = std::move(current->next);

            // insert the node
            current->prev = left_raw->prev;
            if (left_raw == left_owner.get()) {   // move in front of first node in left sublist
                current->next = std::move(left_owner);
                left_owner = std::move(current);
                left_raw->prev = left_owner.get();
            } else if (left_raw->prev) {
                current->next = std::move(left_raw->prev->next);
                left_raw->prev->next = std::move(current);
                left_raw->prev = left_raw->prev->next.get();
            }
        }
    }
}

}   // end namespace

#endif