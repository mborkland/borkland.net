#ifndef SLINKEDLIST_HPP
#define SLINKEDLIST_HPP

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
class SLinkedList : public LinkedList<SingleLinkage, ValueType>
{
public:
    using value_type = typename LinkedList<SingleLinkage, ValueType>::value_type;
    using reference = typename LinkedList<SingleLinkage, value_type>::reference;
    using const_reference = typename LinkedList<SingleLinkage, value_type>::const_reference;
    using pointer = typename LinkedList<SingleLinkage, value_type>::pointer;
    using const_pointer = typename LinkedList<SingleLinkage, value_type>::const_pointer;
    using size_type = typename LinkedList<SingleLinkage, value_type>::size_type;
    using difference_type = typename LinkedList<SingleLinkage, value_type>::difference_type;
    using iterator = typename LinkedList<SingleLinkage, value_type>::iterator;
    using const_iterator = typename LinkedList<SingleLinkage, value_type>::const_iterator;
    using LinkedList<SingleLinkage, value_type>::push_back;
    
private:
    using node_iterator = typename LinkedList<SingleLinkage, value_type>::node_iterator;
    using NodeType = typename LinkedList<SingleLinkage, value_type>::NodeType;
    using LinkedList<SingleLinkage, value_type>::head;
    using LinkedList<SingleLinkage, value_type>::tail;
    using LinkedList<SingleLinkage, value_type>::srtd;
    using LinkedList<SingleLinkage, value_type>::sz;
    using LinkedList<SingleLinkage, value_type>::LinkedList;
    using LinkedList<SingleLinkage, value_type>::construct_from_iterator_range;
    using LinkedList<SingleLinkage, value_type>::swap;

    NodeType* emplace_before_node(NodeType* node, value_type&& val) override;
    void emplace_after_node(NodeType* node, value_type&& val) override;
    void merge(std::unique_ptr<NodeType>& left_owner, NodeType* right_raw, size_type right_size) override;
    NodeType* delete_node(NodeType* node) override;

public:
    SLinkedList() : LinkedList<SingleLinkage, ValueType>{} { }
    SLinkedList(const SLinkedList<value_type>& other) : SLinkedList{other.cbegin(), other.cend()} { srtd = other.srtd; }
    SLinkedList(SLinkedList<value_type>&& other) noexcept : LinkedList<SingleLinkage, value_type>{std::forward<SLinkedList<value_type>>(other)} { }
    template<typename InputIterator> SLinkedList(InputIterator begin, InputIterator end) { construct_from_iterator_range(begin, end); }
    SLinkedList(std::initializer_list<value_type> li) : SLinkedList<value_type>{li.begin(), li.end()} { }
    ~SLinkedList() = default;
    SLinkedList& operator=(const SLinkedList<value_type>& other) = default;
    SLinkedList& operator=(SLinkedList<value_type>&& other) noexcept = default;

    friend class ForwardListIterator<value_type>;
    friend class ConstForwardListIterator<value_type>;
    friend class NodeIterator<NodeType, value_type>;
};

template<typename ValueType>
typename SLinkedList<ValueType>::NodeType* SLinkedList<ValueType>::emplace_before_node(NodeType* node, value_type&& val)
{
    if (!node) {
        throw std::invalid_argument{"Non-empty list pointer can't be null."};
    }

    value_type current_val = std::move(node->data);
    node->data = std::forward<value_type>(val);
    emplace_after_node(node, std::move(current_val));
    return node->next.get();
}

template<typename ValueType>
void SLinkedList<ValueType>::emplace_after_node(NodeType* node, value_type&& val)
{
    if (!node) {
        throw std::invalid_argument{"Non-empty list pointer can't be null."};
    }

    ++sz;
    srtd = false;
    auto new_node = std::make_unique<NodeType>(std::forward<value_type>(val), std::move(node->next));
    if (node == tail) {
        tail = new_node.get();
    }
    node->next = std::move(new_node);
}

template<typename ValueType>
void SLinkedList<ValueType>::merge(std::unique_ptr<NodeType>& left_owner, NodeType* right_raw, size_type right_size)
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

template<typename ValueType>
typename SLinkedList<ValueType>::NodeType* SLinkedList<ValueType>::delete_node(NodeType* node)
{
    if (!node) {
        throw std::invalid_argument{"Can't delete null pointer."};
    }

    NodeType* return_node = nullptr;
    if (sz == 1) {
        head.release();
        tail = nullptr;
    } else if (node == head.get()) {
        head = std::move(node->next);
        return_node = head.get();
    } else if (node == tail) {    // linear time operation
        auto search = head.get();
        while (search->next.get() != tail) {
            search = search->next.get();
        }
        tail = search;
        search->next.release();
    } else {
        node->data = std::move(node->next->data);
        node->next = std::move(node->next->next);
        return_node = node;
    }

    --sz;
    return return_node;
}

}  // end namespace

#endif
