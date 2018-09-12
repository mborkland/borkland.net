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
class DLinkedList : public LinkedList<Linkage::DoubleLinkage, ValueType>
{
public:
    using value_type = typename LinkedList<Linkage::DoubleLinkage, ValueType>::value_type;
    using reference = typename LinkedList<Linkage::DoubleLinkage, value_type>::reference;
    using const_reference = typename LinkedList<Linkage::DoubleLinkage, value_type>::const_reference;
    using pointer = typename LinkedList<Linkage::DoubleLinkage, value_type>::pointer;
    using const_pointer = typename LinkedList<Linkage::DoubleLinkage, value_type>::const_pointer;
    using size_type = typename LinkedList<Linkage::DoubleLinkage, value_type>::size_type;
    using difference_type = typename LinkedList<Linkage::DoubleLinkage, value_type>::difference_type;
    using iterator = typename LinkedList<Linkage::DoubleLinkage, value_type>::iterator;
    using const_iterator = typename LinkedList<Linkage::DoubleLinkage, value_type>::const_iterator;
    using reverse_iterator = ReverseListIterator<value_type>;
    using const_reverse_iterator = ConstReverseListIterator<value_type>;
    using LinkedList<Linkage::DoubleLinkage, value_type>::push_back;

private:
    using node_iterator = typename LinkedList<Linkage::DoubleLinkage, value_type>::node_iterator;
    using node_type = typename LinkedList<Linkage::DoubleLinkage, value_type>::node_type;
    using LinkedList<Linkage::DoubleLinkage, value_type>::head;
    using LinkedList<Linkage::DoubleLinkage, value_type>::tail;
    using LinkedList<Linkage::DoubleLinkage, value_type>::srtd;
    using LinkedList<Linkage::DoubleLinkage, value_type>::sz;
    using LinkedList<Linkage::DoubleLinkage, value_type>::LinkedList;
    using LinkedList<Linkage::DoubleLinkage, value_type>::construct_from_iterator_range;

    node_type* insert_node_before(node_type *node, std::unique_ptr<node_type> &new_node, bool is_reverse) override;
    node_type* insert_node_after(node_type *node, std::unique_ptr<node_type> &new_node, bool is_reverse) override;

    node_type* delete_node(node_type* node, bool is_reverse) override;

public:
    DLinkedList() : LinkedList<Linkage::DoubleLinkage, value_type>{} { }
    DLinkedList(const DLinkedList<value_type>& other) : DLinkedList{other.cbegin(), other.cend()} { srtd = other.srtd; }
    DLinkedList(DLinkedList<value_type>&& other) noexcept : LinkedList<Linkage::DoubleLinkage, value_type>{std::forward<DLinkedList<value_type>>(other)} { }
    template<typename InputIterator> DLinkedList(InputIterator begin, InputIterator end) { construct_from_iterator_range(begin, end); }
    DLinkedList(std::initializer_list<value_type> li) : DLinkedList<value_type>{li.begin(), li.end()} { }
    ~DLinkedList() = default;
    DLinkedList& operator=(const DLinkedList<value_type>& other) = default;
    DLinkedList& operator=(DLinkedList<value_type>&& other) noexcept = default;

    friend void list_sort(LinkedList<Linkage::DoubleLinkage, ValueType>& linked_list);
    friend void list_mergesort(std::unique_ptr<node_type>& left_owner, size_type size);
    friend void list_merge(std::unique_ptr<node_type>& left_owner, node_type* right_raw, size_type right_size, node_type* tail);

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

/* Helper function that takes a new node and inserts it before an existing node in the list. */
template<typename ValueType>
typename DLinkedList<ValueType>::node_type* DLinkedList<ValueType>::insert_node_before(node_type *node, std::unique_ptr<node_type> &new_node, bool is_reverse)
{
    if (!node) {
        throw std::invalid_argument{"Non-empty list pointer can't be null."};
    }

    if (is_reverse) {
        return insert_node_after(node, new_node, false)->prev;
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

/* Helper function that takes a new node and inserts it after an existing node in the list. */
template<typename ValueType>
typename DLinkedList<ValueType>::node_type* DLinkedList<ValueType>::insert_node_after(node_type *node, std::unique_ptr<node_type> &new_node, bool is_reverse)
{
    if (!node) {
        throw std::invalid_argument{"Non-empty list pointer can't be null."};
    }

    if (is_reverse) {
        return insert_node_before(node, new_node, false)->prev;
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

/* Helper function that removes a node from the list. */
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
