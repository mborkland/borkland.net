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
class SLinkedList : public LinkedList<Linkage::SingleLinkage, ValueType>
{
public:
    using value_type = typename LinkedList<Linkage::SingleLinkage, ValueType>::value_type;
    using reference = typename LinkedList<Linkage::SingleLinkage, value_type>::reference;
    using const_reference = typename LinkedList<Linkage::SingleLinkage, value_type>::const_reference;
    using pointer = typename LinkedList<Linkage::SingleLinkage, value_type>::pointer;
    using const_pointer = typename LinkedList<Linkage::SingleLinkage, value_type>::const_pointer;
    using size_type = typename LinkedList<Linkage::SingleLinkage, value_type>::size_type;
    using difference_type = typename LinkedList<Linkage::SingleLinkage, value_type>::difference_type;
    using iterator = typename LinkedList<Linkage::SingleLinkage, value_type>::iterator;
    using const_iterator = typename LinkedList<Linkage::SingleLinkage, value_type>::const_iterator;
    using LinkedList<Linkage::SingleLinkage, value_type>::push_back;
    
private:
    using node_iterator = typename LinkedList<Linkage::SingleLinkage, value_type>::node_iterator;
    using node_type = typename LinkedList<Linkage::SingleLinkage, value_type>::node_type;
    using LinkedList<Linkage::SingleLinkage, value_type>::head;
    using LinkedList<Linkage::SingleLinkage, value_type>::tail;
    using LinkedList<Linkage::SingleLinkage, value_type>::srtd;
    using LinkedList<Linkage::SingleLinkage, value_type>::sz;
    using LinkedList<Linkage::SingleLinkage, value_type>::LinkedList;
    using LinkedList<Linkage::SingleLinkage, value_type>::construct_from_iterator_range;
    using LinkedList<Linkage::SingleLinkage, value_type>::swap;

    node_type* insert_node_before(node_type *node, std::unique_ptr<node_type> &new_node, bool is_reverse) override;
    node_type* insert_node_after(node_type *node, std::unique_ptr<node_type> &new_node, bool is_reverse) override;

    node_type* delete_node(node_type* node, bool is_reverse) override;

public:
    SLinkedList() : LinkedList<Linkage::SingleLinkage, ValueType>{} { }
    SLinkedList(const SLinkedList<value_type>& other) : SLinkedList{other.cbegin(), other.cend()} { srtd = other.srtd; }
    SLinkedList(SLinkedList<value_type>&& other) noexcept : LinkedList<Linkage::SingleLinkage, value_type>{std::forward<SLinkedList<value_type>>(other)} { }
    template<typename InputIterator> SLinkedList(InputIterator begin, InputIterator end) { construct_from_iterator_range(begin, end); }
    SLinkedList(std::initializer_list<value_type> li) : SLinkedList<value_type>{li.begin(), li.end()} { }
    ~SLinkedList() = default;
    SLinkedList& operator=(const SLinkedList<value_type>& other) = default;
    SLinkedList& operator=(SLinkedList<value_type>&& other) noexcept = default;

    friend class ForwardListIterator<value_type>;
    friend class ConstForwardListIterator<value_type>;
    friend class NodeIterator<node_type, value_type>;
};

/* Helper function that takes a new node and inserts it before an existing node in the list. */
template<typename ValueType>
typename SLinkedList<ValueType>::node_type* SLinkedList<ValueType>::insert_node_before(node_type *node, std::unique_ptr<node_type> &new_node, bool is_reverse)
{
    if (!node) {
        throw std::invalid_argument{"Non-empty list pointer can't be null."};
    }

    using std::swap;
    swap(node->data, new_node->data);
    return insert_node_after(node, new_node, is_reverse);
}

/* Helper function that takes a new node and inserts it before an existing node in the list. */
template<typename ValueType>
typename SLinkedList<ValueType>::node_type* SLinkedList<ValueType>::insert_node_after(node_type *node, std::unique_ptr<node_type> &new_node, bool is_reverse)
{
    if (!node) {
        throw std::invalid_argument{"Non-empty list pointer can't be null."};
    }

    ++sz;
    srtd = false;
    new_node->next = std::move(node->next);
    if (node == tail) {
        tail = new_node.get();
    }
    node->next = std::move(new_node);
    return node->next.get();
}

/* Helper function that removes a node from the list. */
template<typename ValueType>
typename SLinkedList<ValueType>::node_type* SLinkedList<ValueType>::delete_node(node_type* node, bool is_reverse)
{
    if (!node) {
        throw std::invalid_argument{"Can't delete null pointer."};
    }

    node_type* return_node = nullptr;
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