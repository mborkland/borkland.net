#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include <algorithm>
#include <memory>
#include <type_traits>
#include <utility>
#include "NodeIterator.hpp"

namespace bork_lib
{

class SingleLinkage {};
class DoubleLinkage {};

template<typename, typename = void>
struct supports_less_than : std::false_type { };

template<typename T>
struct supports_less_than<T, std::void_t<decltype(std::declval<T&>() <= std::declval<T&>())>> : std::true_type { };

template<typename LinkageType, typename ValueType>
class LinkedList {
protected:
    template<typename ShadowedLinkageType, typename = void> struct ListNode;

    template<typename ShadowedLinkageType>
    struct ListNode<ShadowedLinkageType, std::enable_if_t<std::is_same<ShadowedLinkageType, SingleLinkage>::value>>
    {
        ValueType data;
        std::unique_ptr<ListNode> next = std::unique_ptr<ListNode>(nullptr);

        template<typename... Args, typename = std::enable_if_t<std::is_constructible_v<ValueType, Args&&...>>>
        explicit ListNode(Args&&... args) : data{std::forward<Args>(args)...} { }
        explicit ListNode(const ValueType& data) : data{data} { }
        explicit ListNode(ValueType&& data) : data{std::forward<ValueType>(data)} { }
    };

    template<typename ShadowedLinkageType>
    struct ListNode<ShadowedLinkageType, std::enable_if_t<!std::is_same<ShadowedLinkageType, SingleLinkage>::value>>
    {
        ValueType data;
        std::unique_ptr<ListNode> next = std::unique_ptr<ListNode>(nullptr);
        ListNode* prev = nullptr;

        template<typename... Args, typename = std::enable_if_t<std::is_constructible_v<ValueType, Args&&...>>>
        explicit ListNode(Args&&... args) : data{std::forward<Args>(args)...} { }
        explicit ListNode(const ValueType& data) : data{data} { }
        explicit ListNode(ValueType&& data) : data{std::forward<ValueType>(data)} { }
    };

public:
    using value_type = ValueType;
    using reference = ValueType&;
    using const_reference = const ValueType&;
    using pointer = ValueType*;
    using const_pointer = const ValueType*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = std::conditional_t<std::is_same<LinkageType, SingleLinkage>::value,
                     ForwardListIterator<ValueType>, ListIterator<ValueType>>;
    using const_iterator = std::conditional_t<std::is_same<LinkageType, SingleLinkage>::value,
                           ConstForwardListIterator<ValueType>, ConstListIterator<ValueType>>;

    using node_iterator = NodeIterator<ListNode<LinkageType>, value_type>;

protected:
    using node_type = ListNode<LinkageType>;
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

    void mergesort(std::unique_ptr<node_type>& left_owner, size_type size);
    virtual void merge(std::unique_ptr<node_type>& left_owner, node_type* right_raw, size_type right_size) = 0;

    virtual node_type* delete_node(node_type* node, bool is_reverse) = 0;
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

    node_iterator& insert_before(node_iterator& iter, const value_type& val) { return emplace_before(iter, val); }
    node_iterator& insert_before(node_iterator& iter, value_type&& val) { return emplace_before(iter, std::forward<value_type>(val)); }
    node_iterator& insert_after(node_iterator& iter, const value_type& val) { return emplace_after(iter, val); }
    node_iterator& insert_after(node_iterator& iter, value_type&& val) { return emplace_after(iter, std::forward<value_type>(val)); }
    void insert_sorted(const value_type& val) { return emplace_sorted(val); }
    void insert_sorted(value_type&& val) { return emplace_sorted(std::forward<value_type>(val)); }

    template<typename... Args> node_iterator& emplace_before(node_iterator& iter, Args&&... args);
    template<typename... Args> node_iterator& emplace_after(node_iterator& iter, Args&&... args);
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
    node_iterator& erase(node_iterator& iter);
    void clear() noexcept;

    template<typename T = value_type, std::enable_if_t<supports_less_than<T>::value, int> = 0>
    void sort();
    template<typename T = value_type, std::enable_if_t<!supports_less_than<T>::value, int> = 0>
    void sort() { throw std::logic_error("List cannot be sorted, as value_type does not support comparison."); }

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
    friend class NodeIterator<ListNode<LinkageType>, value_type>;
};

template<typename LinkageType, typename ValueType>
LinkedList<LinkageType, ValueType>& LinkedList<LinkageType, ValueType>::operator=(const LinkedList<LinkageType, ValueType>& other)
{
    clear();
    for (const auto& x : other) {
        push_back(x);
    }

    srtd = other.srtd;
    return *this;
}

template<typename LinkageType, typename ValueType>
LinkedList<LinkageType, ValueType>& LinkedList<LinkageType, ValueType>::operator=(LinkedList<LinkageType, ValueType>&& other) noexcept
{
    list_swap(other);
    return *this;
}

template<typename LinkageType, typename ValueType>
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

template<typename LinkageType, typename ValueType>
void LinkedList<LinkageType, ValueType>::swap(value_type& a, value_type& b)
{
    value_type tmp{std::move(a)};
    a = std::move(b);
    b = std::move(tmp);
}

/* Helper function to insert an element in-place into an empty list. */
template<typename LinkageType, typename ValueType>
template<typename... Args>
void LinkedList<LinkageType, ValueType>::emplace_empty(Args&&... args)
{
    head = std::make_unique<node_type>(std::forward<Args>(args)...);
    tail = head.get();
    ++sz;
}

/* Helper function to find the correct position for inserting an element into a sorted list. */
template<typename LinkageType, typename ValueType>
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

/* Helper function used to recursively sort and merge sublists. */
template<typename LinkageType, typename ValueType>
void LinkedList<LinkageType, ValueType>::mergesort(std::unique_ptr<node_type>& left_owner, size_type size)
{
    if (size <= 1)  // already sorted
        return;

    size_type split = size / 2;
    mergesort(left_owner, split);                // sort left half
    auto node = left_owner.get();
    for (size_type i = 0; i < split - 1; ++i) {   // split the list
        node = node->next.get();
    }
    auto& right_owner = node->next;
    mergesort(right_owner, size - split);        // sort right half
    merge(left_owner, right_owner.get(), size - split);  // merge the two halves
}

/* Helper function that returns a pointer to the first node with the value specified. */
template<typename LinkageType, typename ValueType>
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


template<typename LinkageType, typename ValueType>
template<typename InputIterator>
void LinkedList<LinkageType, ValueType>::construct_from_iterator_range(InputIterator begin, InputIterator end)
{
    while (begin != end) {
        push_back(*begin++);
    }
    srtd = std::is_sorted(begin, end);
}

/* Public function that inserts a value in-place before a node in the list. */
template<typename LinkageType, typename ValueType>
template<typename... Args>
typename LinkedList<LinkageType, ValueType>::node_iterator& LinkedList<LinkageType, ValueType>::emplace_before(node_iterator& iter, Args&&... args)
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

/* Public function that inserts a value in-place after a node in the list. */
template<typename LinkageType, typename ValueType>
template<typename... Args>
typename LinkedList<LinkageType, ValueType>::node_iterator& LinkedList<LinkageType, ValueType>::emplace_after(node_iterator& iter, Args&&... args)
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

/* Public function that inserts a value in-place into its correct position in a sorted list. */
template<typename LinkageType, typename ValueType>
template<typename... Args>
void LinkedList<LinkageType, ValueType>::emplace_sorted(Args&&... args)
{
    if (empty()) {
        emplace_empty(std::forward<Args>(args)...);
        return;
    }

    sort();    // won't sort if already sorted
    auto new_node = std::make_unique<node_type>(std::forward<Args>(args)...);
    auto position = find_sorted_position(new_node->data);
    position ? insert_node_before(position, new_node, false) : insert_node_after(tail, new_node, false);
    srtd = true;
}

/* Public function that attempts to find a value within the list. */
template<typename LinkageType, typename ValueType>
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
template<typename LinkageType, typename ValueType>
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

/* Public sort function. Calls the mergesort helper function. */
template<typename LinkageType, typename ValueType>
template<typename T, std::enable_if_t<supports_less_than<T>::value, int>>
void LinkedList<LinkageType, ValueType>::sort()
{
    if (srtd) {
        return;
    }

    mergesort(head, sz);   // sort the entire list
    srtd = true;
}

/* Public function that erases a node pointed to by an iterator. */
template<typename LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::node_iterator& LinkedList<LinkageType, ValueType>::erase(node_iterator& iter)
{
    if (empty()) {
        throw std::out_of_range{"Can't delete from empty list."};
    }

    iter.node = delete_node(iter.node, iter.is_reverse());
    return iter;
}

/* Public function that clears a list. */
template<typename LinkageType, typename ValueType>
void LinkedList<LinkageType, ValueType>::clear() noexcept
{
    while (head) {
        head = std::move(head->next);
    }

    tail = nullptr;
    sz = 0;
}

}   // end namespace

#endif