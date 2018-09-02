#ifndef DLINKEDLIST_HPP
#define DLINKEDLIST_HPP

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <utility>
#include "LinkedList.hpp"
#include "NodeIterator.hpp"

namespace bork_lib
{

template<typename ValueType>
class DLinkedList : public LinkedList<DoubleLinkage, ValueType>
{
public:
    using value_type = typename LinkedList<DoubleLinkage, ValueType>::value_type;
    using reference = typename LinkedList<DoubleLinkage, value_type>::reference;
    using const_reference = typename LinkedList<DoubleLinkage, value_type>::const_reference;
    using pointer = typename LinkedList<DoubleLinkage, value_type>::pointer;
    using const_pointer = typename LinkedList<DoubleLinkage, value_type>::const_pointer;
    using size_type = typename LinkedList<DoubleLinkage, value_type>::size_type;
    using difference_type = typename LinkedList<DoubleLinkage, value_type>::difference_type;
    using iterator = typename LinkedList<DoubleLinkage, value_type>::iterator;
    using const_iterator = typename LinkedList<DoubleLinkage, value_type>::const_iterator;
    using reverse_iterator = ReverseListIterator<value_type>;
    using const_reverse_iterator = ConstReverseListIterator<value_type>;
    using LinkedList<DoubleLinkage, value_type>::push_back;

private:
    using node_iterator = typename LinkedList<DoubleLinkage, value_type>::node_iterator;
    using node_type = typename LinkedList<DoubleLinkage, value_type>::node_type;
    using LinkedList<DoubleLinkage, value_type>::head;
    using LinkedList<DoubleLinkage, value_type>::tail;
    using LinkedList<DoubleLinkage, value_type>::srtd;
    using LinkedList<DoubleLinkage, value_type>::sz;
    using LinkedList<DoubleLinkage, value_type>::LinkedList;
    using LinkedList<DoubleLinkage, value_type>::construct_from_iterator_range;

    node_type* emplace_before_node(node_type* node, std::unique_ptr<node_type>& new_node, bool is_reverse) override;
    node_type* emplace_after_node(node_type* node, std::unique_ptr<node_type>& new_node, bool is_reverse) override;

    void merge(std::unique_ptr<node_type>& left_owner, node_type* right_raw, size_type right_size) override;
    template<typename T = value_type, std::enable_if_t<supports_less_than<T>::value, int> = 0>
    void merge_helper(std::unique_ptr<node_type>& left_owner, node_type* right_raw, size_type right_size);
    template<typename T = value_type, std::enable_if_t<!supports_less_than<T>::value, int> = 0>
    void merge_helper(std::unique_ptr<node_type>& left_owner, node_type* right_raw, size_type right_size) { }

    node_type* delete_node(node_type* node, bool is_reverse) override;

public:
    DLinkedList() : LinkedList<DoubleLinkage, value_type>{} { }
    DLinkedList(const DLinkedList<value_type>& other) : DLinkedList{other.cbegin(), other.cend()} { srtd = other.srtd; }
    DLinkedList(DLinkedList<value_type>&& other) noexcept : LinkedList<DoubleLinkage, value_type>{std::forward<DLinkedList<value_type>>(other)} { }
    template<typename InputIterator> DLinkedList(InputIterator begin, InputIterator end);
    DLinkedList(std::initializer_list<value_type> li) : DLinkedList<value_type>{li.begin(), li.end()} { }
    ~DLinkedList() = default;
    DLinkedList& operator=(const DLinkedList<value_type>& other) = default;
    DLinkedList& operator=(DLinkedList<value_type>&& other) noexcept = default;

    reverse_iterator rbegin() noexcept { return reverse_iterator{tail}; }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{tail}; };
    reverse_iterator rend() noexcept { return reverse_iterator{nullptr}; }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator{nullptr}; };
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{tail}; }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator{nullptr}; }

    friend class ListIterator<value_type>;
    friend class ConstListIterator<value_type>;
    friend class ReverseListIterator<value_type>;
    friend class ConstReverseListIterator<value_type>;
    friend class NodeIterator<node_type, value_type>;
};

template<typename ValueType>
template<typename InputIterator>
DLinkedList<ValueType>::DLinkedList(InputIterator begin, InputIterator end)
{
    construct_from_iterator_range(begin, end);
}

template<typename ValueType>
typename DLinkedList<ValueType>::node_type* DLinkedList<ValueType>::emplace_before_node(node_type* node, std::unique_ptr<node_type>& new_node, bool is_reverse)
{
    if (!node) {
        throw std::invalid_argument{"Non-empty list pointer can't be null."};
    }

    if (is_reverse) {
        return emplace_after_node(node, new_node, false)->prev;
    }

    ++sz;
    srtd = false;
    new_node->prev = node->prev;
    node->prev = new_node.get();
    if (node == head.get()) {    // insert at front of list
        new_node->next = std::move(head);
        head = std::move(new_node);
    } else {
        new_node->next = std::move(new_node->prev->next);
        new_node->prev->next = std::move(new_node);
    }

    return node;
}

template<typename ValueType>
typename DLinkedList<ValueType>::node_type* DLinkedList<ValueType>::emplace_after_node(node_type* node, std::unique_ptr<node_type>& new_node, bool is_reverse)
{
    if (!node) {
        throw std::invalid_argument{"Non-empty list pointer can't be null."};
    }

    if (is_reverse) {
        return emplace_before_node(node, new_node, false)->prev;
    }

    ++sz;
    srtd = false;
    new_node->next = std::move(node->next);
    new_node->prev = node;
    if (node == tail) {   // insert at back of list
        tail = new_node.get();
    } else {
        new_node->next->prev = new_node.get();
    }
    node->next = std::move(new_node);
    return node->next.get();
}

template<typename ValueType>
void DLinkedList<ValueType>::merge(std::unique_ptr<node_type>& left_owner, node_type* right_raw, size_type right_size)
{
    merge_helper(left_owner, right_raw, right_size);
}

template<typename ValueType>
template<typename T, std::enable_if_t<supports_less_than<T>::value, int>>
void DLinkedList<ValueType>::merge_helper(std::unique_ptr<node_type>& left_owner, node_type* right_raw, size_type right_size)
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

template<typename ValueType>
typename DLinkedList<ValueType>::node_type* DLinkedList<ValueType>::delete_node(node_type* node, bool is_reverse)
{
    if (!node) {
        throw std::invalid_argument{"Can't delete null pointer."};
    }

    auto return_node = is_reverse ? node->prev : node->next.get();
    if (sz == 1) {
        head.release();
        tail = nullptr;
    } else if (node == head.get()) {
        head = std::move(node->next);
    } else if (node == tail) {
        tail = node->prev;
        node->prev->next.release();
    } else {
        node->next->prev = node->prev;
        node->prev->next = std::move(node->next);
    }

    --sz;
    return return_node;
}

}  // end namespace

#endif
