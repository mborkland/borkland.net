#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include <algorithm>
#include <memory>
#include <type_traits>
#include "NodeIterator.hpp"

namespace detail
{

}    // end namespace

namespace bork_lib
{

class SingleLinkage {};
class DoubleLinkage {};

template<typename LinkageType, typename ValueType>
class LinkedList {
protected:
    template<typename ShadowedLinkageType, typename = void> struct ListNode;

    template<typename ShadowedLinkageType>
    struct ListNode<ShadowedLinkageType, std::enable_if_t<std::is_same<ShadowedLinkageType, SingleLinkage>::value>>
    {
        ValueType data;
        std::unique_ptr<ListNode> next;
        explicit ListNode(ValueType&& data, std::unique_ptr<ListNode>&& next = std::unique_ptr<ListNode>(nullptr))
                : data{std::forward<ValueType>(data)}, next{std::move(next)} { }
    };

    template<typename ShadowedLinkageType>
    struct ListNode<ShadowedLinkageType, std::enable_if_t<!std::is_same<ShadowedLinkageType, SingleLinkage>::value>>
    {
        ValueType data;
        std::unique_ptr<ListNode> next;
        ListNode* prev;
        explicit ListNode(ValueType&& data, std::unique_ptr<ListNode>&& next = std::unique_ptr<ListNode>(nullptr),
                ListNode* prev = nullptr) : data{std::forward<ValueType>(data)}, next{std::move(next)}, prev{prev} { }
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
    using NodeType = ListNode<LinkageType>;
    std::unique_ptr<NodeType> head = std::unique_ptr<NodeType>(nullptr);
    NodeType* tail = nullptr;
    bool srtd = false;         // the list is guaranteed to be sorted if true
    size_type sz = 0;         // size of the list
    void list_swap(LinkedList<LinkageType, ValueType>& list1, LinkedList<LinkageType, ValueType>& list2);
    template<typename... Args> void emplace_empty(Args&&... args);
    NodeType* find_sorted_position(const value_type& val);
    virtual void emplace_before_node(NodeType* node, value_type&& val) = 0;
    virtual void emplace_after_node(NodeType* node, value_type&& val) = 0;
    virtual void mergesort(std::unique_ptr<NodeType>& left_owner, size_type size) = 0;
    virtual void merge(std::unique_ptr<NodeType>& left_owner, NodeType* right_raw, size_type right_size) = 0;
    virtual void delete_node(NodeType* node) = 0;
    NodeType* search_front(const value_type& val) const noexcept;

public:
    // construction, assignment, and destruction
    LinkedList() = default;
    explicit LinkedList(size_type num_elements, const value_type& val = {});
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
    node_iterator& insert_before(node_iterator& iter, value_type&& val) { return emplace_before(iter, val); }
    node_iterator& insert_after(node_iterator& iter, const value_type& val) { return emplace_after(iter, val); }
    node_iterator& insert_after(node_iterator& iter, value_type&& val) { return emplace_after(iter, val); }
    void insert_sorted(const value_type& val) { return emplace_sorted(val); }
    void insert_sorted(value_type&& val) { return emplace_sorted(val); }

    template<typename... Args> node_iterator& emplace_before(node_iterator& iter, Args&&... args);
    template<typename... Args> node_iterator& emplace_after(node_iterator& iter, Args&&... args);
    template<typename... Args> void emplace_sorted(Args&&... args);

    void push_front(const value_type& val) { iterator iter{head.get()}; insert_before(iter, val); }
    void push_front(value_type&& val) { iterator iter{head.get()}, insert_before(iter, std::forward<value_type>(val)); }
    void push_back(const value_type& val) { iterator iter{tail}; insert_after(iter, val); }
    void push_back(value_type&& val) { iterator iter{tail}; insert_after(iter, std::forward<value_type>(val)); }
    virtual void pop_front() { delete_node(head.get()); }
    virtual void pop_back() { delete_node(tail); }

    iterator find(const value_type& val) const;
    size_type count(const value_type& val) const;
    void sort();
    node_iterator& erase(node_iterator& iter);
    void clear() noexcept;

    // iterator functions
    iterator begin() noexcept { return iterator{head.get()}; }
    const_iterator begin() const noexcept { return const_iterator{head.get()}; }
    iterator end() noexcept { return iterator{nullptr}; }
    const_iterator end() const noexcept { return const_iterator{nullptr}; };
    const_iterator cbegin() const noexcept { return const_iterator{head.get()}; }
    const_iterator cend()  const noexcept { return const_iterator{nullptr}; }


    friend class ListIterator<value_type>;
    friend class ConstListIterator<value_type>;
    friend class ForwardListIterator<value_type>;
    friend class ConstForwardListIterator<value_type>;
    friend class NodeIterator<ListNode<LinkageType>, value_type>;
};

template<typename LinkageType, typename ValueType>
LinkedList<LinkageType, ValueType>::LinkedList(size_type num_elements, const value_type& val)
{
    for (size_type i = 0; i < num_elements; ++i) {
        push_back(val);
    }

    srtd = true;
}

template<typename LinkageType, typename ValueType>
LinkedList<LinkageType, ValueType>& LinkedList<LinkageType, ValueType>::operator=(const LinkedList<LinkageType, ValueType>& other)
{
    clear();
    for (const auto& x : other) {
        push_back(x);
    }

    return *this;
}

template<typename LinkageType, typename ValueType>
LinkedList<LinkageType, ValueType>& LinkedList<LinkageType, ValueType>::operator=(LinkedList<LinkageType, ValueType>&& other) noexcept
{
    list_swap(*this, other);
    return *this;
}

template<typename LinkageType, typename ValueType>
void LinkedList<LinkageType, ValueType>::list_swap(LinkedList<LinkageType, ValueType>& list1, LinkedList<LinkageType, ValueType>& list2)
{
    auto temp_unique = std::move(list1.head);
    list1.head = std::move(list2.head);
    list2.head = std::move(temp_unique);

    using std::swap;
    swap(list1.tail, list2.tail);
    swap(list1.sz, list2.sz);
    swap(list1.srtd, list2.srtd);
}

template<typename LinkageType, typename ValueType>
template<typename... Args>
void LinkedList<LinkageType, ValueType>::emplace_empty(Args&&... args)
{
    head = std::make_unique<NodeType>(NodeType{value_type{args...}});
    tail = head.get();
    ++sz;
}

template<typename LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::NodeType* LinkedList<LinkageType, ValueType>::find_sorted_position(const value_type &val)
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

template<typename LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::NodeType* LinkedList<LinkageType, ValueType>::search_front(const value_type& val) const noexcept
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
template<typename... Args>
typename LinkedList<LinkageType, ValueType>::node_iterator& LinkedList<LinkageType, ValueType>::emplace_before(node_iterator& iter, Args&&... args)
{
    if (empty()) {
        emplace_empty(std::forward<Args>(args)...);
        iter.node = head.get();
    } else {
        emplace_before_node(iter.node, value_type{args...});
    }

    return iter;
}

template<typename LinkageType, typename ValueType>
template<typename... Args>
typename LinkedList<LinkageType, ValueType>::node_iterator& LinkedList<LinkageType, ValueType>::emplace_after(node_iterator& iter, Args&&... args)
{
    if (empty()) {
        emplace_empty(std::forward<Args>(args)...);
        iter.node = head.get();
        return iter;
    }

    emplace_after_node(iter.node, value_type{args...});
    return ++iter;
}

template<typename LinkageType, typename ValueType>
template<typename... Args>
void LinkedList<LinkageType, ValueType>::emplace_sorted(Args&&... args)
{
    if (empty()) {
        emplace_empty(std::forward<Args>(args)...);
    }

    sort();    // won't sort if already sorted
    auto position = find_sorted_position(value_type{args...});
    position ? emplace_before_node(position, value_type{args...}) : emplace_after_node(tail, value_type{args...});
}

template<typename LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::iterator LinkedList<LinkageType, ValueType>::find(const value_type &val) const
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

template<typename LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::size_type LinkedList<LinkageType, ValueType>::count(const value_type& val) const
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

template<typename LinkageType, typename ValueType>
void LinkedList<LinkageType, ValueType>::sort()
{
    if (srtd) {
        return;
    }

    mergesort(head, sz);   // sort the entire list
    srtd = true;
}

template<typename LinkageType, typename ValueType>
typename LinkedList<LinkageType, ValueType>::node_iterator& LinkedList<LinkageType, ValueType>::erase(node_iterator& iter)
{
    ++iter;
    delete_node(iter.node);
    return iter;
}

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