#ifndef DLINKEDLIST_HPP
#define DLINKEDLIST_HPP

#include "LinkedList.hpp"
#include "NodeIterator.hpp"
#include <memory>
#include <stdexcept>
#include <utility>

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

private:
    using node_iterator = typename LinkedList<DoubleLinkage, value_type>::node_iterator;
    using LinkedList<DoubleLinkage, value_type>::head;
    using LinkedList<DoubleLinkage, value_type>::tail;
    using LinkedList<DoubleLinkage, value_type>::srtd;
    using LinkedList<DoubleLinkage, value_type>::sz;
    using LinkedList<DoubleLinkage, value_type>::LinkedList;
    using LinkedList<DoubleLinkage, value_type>::empty;
    using NodeType = typename LinkedList<DoubleLinkage, value_type>::NodeType;

private:
    void emplace_before_node(NodeType* node, value_type&& val) override;
    void emplace_after_node(NodeType* node, value_type&& val) override;
    void mergesort(std::unique_ptr<NodeType>& left_owner, size_type size) override;
    void merge(std::unique_ptr<NodeType>& left_owner, NodeType* right_raw, size_type right_size) override;
    void delete_node(NodeType* node) override;

public:
    DLinkedList() : LinkedList<DoubleLinkage, ValueType>{} { }
    explicit DLinkedList(size_type num_elements, const value_type& val = {}) : LinkedList<DoubleLinkage, ValueType>{num_elements, val} { }
    DLinkedList(const DLinkedList<value_type>& other) : LinkedList<DoubleLinkage, ValueType>{other} { }
    DLinkedList(DLinkedList<value_type>&& other) noexcept : LinkedList<DoubleLinkage, ValueType>{std::forward<DLinkedList<value_type>>(other)} { }
    template<typename InputIterator>
    DLinkedList(InputIterator begin, InputIterator end) : LinkedList<DoubleLinkage, ValueType>{begin, end} { }
    DLinkedList(std::initializer_list<value_type> li) : LinkedList<DoubleLinkage, ValueType>{li} { }
    ~DLinkedList() = default;
    DLinkedList<value_type>& operator=(const DLinkedList<value_type>& other) = default;
    DLinkedList<value_type>& operator=(DLinkedList<value_type>&& other) noexcept = default;

    reverse_iterator rbegin() noexcept { return reverse_iterator{tail}; }
    reverse_iterator rend() noexcept { return reverse_iterator{nullptr}; }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{tail}; }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator{nullptr}; }

    friend class ListIterator<value_type>;
    friend class ConstListIterator<value_type>;
    friend class ReverseListIterator<value_type>;
    friend class ConstReverseListIterator<value_type>;
    friend class NodeIterator<NodeType, value_type>;
};

template<typename ValueType>
void DLinkedList<ValueType>::emplace_before_node(NodeType* node, value_type&& val)
{
    if (!node) {
        throw std::invalid_argument{"Non-empty list pointer can't be null."};
    }

    ++sz;
    srtd = false;
    auto new_node = std::make_unique<NodeType>(std::forward<value_type>(val));
    new_node->prev = node->prev;
    node->prev = new_node.get();

    if (node == head.get()) {    // insert at front of list
        new_node->next = std::move(head);
        head = std::move(new_node);
    } else {
        new_node->next = std::move(new_node->prev->next);
        new_node->prev->next = std::move(new_node);
    }
}

template<typename ValueType>
void DLinkedList<ValueType>::emplace_after_node(NodeType* node, value_type&& val)
{
    if (!node) {
        throw std::invalid_argument{"Non-empty list pointer can't be null."};
    }

    ++sz;
    srtd = false;
    auto new_node = std::make_unique<NodeType>(std::forward<value_type>(val), std::move(node->next), node);

    if (node == tail) {   // insert at back of list
        tail = new_node.get();
    } else {
        new_node->next->prev = new_node.get();
    }
    node->next = std::move(new_node);
}

template<typename ValueType>
void DLinkedList<ValueType>::mergesort(std::unique_ptr<NodeType>& left_owner, size_type size)
{
    if (size <= 1)  // already sorted
        return;

    auto node = left_owner.get();
    size_type split = size / 2;
    for (size_type i = 0; i < split - 1; ++i) {   // split the list
        node = node->next.get();
    }
    
    auto& right_owner = node->next;
    mergesort(left_owner, split);                // sort left half
    mergesort(right_owner, size - split);        // sort right half
    merge(left_owner, right_owner.get(), size - split);  // merge the two halves
}

template<typename ValueType>
void DLinkedList<ValueType>::merge(std::unique_ptr<NodeType>& left_owner, NodeType* right_raw, size_type right_size)
{
    if (!left_owner || !right_raw) {
        return;
    }

    auto left_raw = left_owner.get();

    /* Explanation of the following while loop conditions:
       1. right_size keeps track of the number of unmerged nodes
          in the right sublist. When right_size == 0, the last node
          to be merged was in the right sublist and the sublists
          have been merged.
       2. If left_owner == right_owner, then all the nodes in the
          left sublist have been merged. Since the right sublist is
          already sorted, the merging is now complete. */
    while(right_size && left_raw != right_raw)
    {
        /* When the next node to be merged is from the
        left sublist, simply move the left_owner pointer
        to the next node. */
        if (left_raw->data <= right_raw->data) {
            left_raw = left_raw->next.get();
        }

            /* When the next node to be merged is from the
            right sublist, put that node in front of the
            node pointed to by left_owner. */
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
            } else if (left_raw->prev) {
                current->next = std::move(left_raw->prev->next);
                left_raw->prev->next = std::move(current);
            }

            left_raw->prev = current.get();
        }
    }
}

template<typename ValueType>
void DLinkedList<ValueType>::delete_node(NodeType* node)
{
    if (empty()) {
        throw std::out_of_range{"Can't delete from empty list."};
    }

    if (!node) {
        throw std::invalid_argument{"Can't delete null pointer."};
    }

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
}

}  // end namespace

#endif
