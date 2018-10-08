#ifndef NODEITERATOR_HPP
#define NODEITERATOR_HPP

#include <iterator>
#include <stdexcept>
#include "Linkage.hpp"

namespace bork_lib
{

template<Linkage L, typename V> class LinkedList;
template<typename V> class DLinkedList;
template<typename V> class SLinkedList;

template<typename V> class ConstListIterator;
template<typename V> class ReverseListIterator;
template<typename V> class ConstReverseListIterator;
template<typename V> class ConstForwardListIterator;

template<typename ValueType>
class ListIterator
{
private:
    using node_type = typename LinkedList<Linkage::DoubleLinkage, ValueType>::node_type;
    node_type* node;
    bool is_reverse() { return false; }
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = ValueType;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ListIterator(node_type* node = nullptr) : node{node} { }
    ListIterator(const ListIterator<value_type>& other) : node{other.node} { }
    ListIterator(const ConstListIterator<value_type>& other) : node{other.node} { }
    ListIterator(const ReverseListIterator<value_type>& other) : node{other.node} { }
    ListIterator(const ConstReverseListIterator<value_type>& other) : node{other.node} { }

    ListIterator<value_type>& operator=(const ListIterator<value_type>& other) { node = other.node; return *this; }
    ListIterator<value_type>& operator=(const ConstListIterator<value_type>& other) { node = other.node; return *this; }
    ListIterator<value_type>& operator=(const ReverseListIterator<value_type>& other) { node = other.node; return *this; }
    ListIterator<value_type>& operator=(const ConstReverseListIterator<value_type>& other) { node = other.node; return *this; }
    ~ListIterator() = default;

    reference operator*() const { return node->data; }
    pointer operator->() { return &(node->data); }
    ListIterator<value_type>& operator++() { node = node->next.get(); return *this; }
    ListIterator<value_type>& operator--() { node = node->prev; return *this; }
    const ListIterator<value_type> operator++(int)
    {
        ListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    const ListIterator<value_type> operator--(int)
    {
        ListIterator<value_type> temp{*this};
        operator--();
        return temp;
    }
    bool operator==(const ListIterator<value_type>& other) const noexcept { return node == other.node; }
    bool operator!=(const ListIterator<value_type>& other) const noexcept { return !operator==(other); }

    friend class LinkedList<Linkage::DoubleLinkage, value_type>;
    friend class DLinkedList<value_type>;
};

template<typename ValueType>
class ConstListIterator
{
private:
    using node_type = typename LinkedList<Linkage::DoubleLinkage, ValueType>::node_type;
    node_type* node;
    bool is_reverse() { return false; }
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = ValueType;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ConstListIterator(node_type* node = nullptr) : node{node} { }
    ConstListIterator(const ListIterator<value_type>& other) : node{other.node} { }
    ConstListIterator(const ConstListIterator<value_type>& other) : node{other.node} { }
    ConstListIterator(const ReverseListIterator<value_type>& other) : node{other.node} { }
    ConstListIterator(const ConstReverseListIterator<value_type>& other) : node{other.node} { }

    ConstListIterator<value_type>& operator=(const ListIterator<value_type>& other) { node = other.node; return *this; }
    ConstListIterator<value_type>& operator=(const ConstListIterator<value_type>& other) { node = other.node; return *this; }
    ConstListIterator<value_type>& operator=(const ReverseListIterator<value_type>& other) { node = other.node; return *this; }
    ConstListIterator<value_type>& operator=(const ConstReverseListIterator<value_type>& other) { node = other.node; return *this; }
    ~ConstListIterator() = default;

    const_reference operator*() const { return node->data; }
    const_pointer operator->() { return &(node->data); }
    ConstListIterator<value_type>& operator++() { node = node->next.get(); return *this; }
    ConstListIterator<value_type>& operator--() { node = node->prev; return *this; }
    const ConstListIterator<value_type> operator++(int)
    {
        ConstListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    const ConstListIterator<value_type> operator--(int)
    {
        ConstListIterator<value_type> temp{*this};
        operator--();
        return temp;
    }
    bool operator==(const ConstListIterator<value_type>& other) const noexcept { return node == other.node; }
    bool operator!=(const ConstListIterator<value_type>& other) const noexcept { return !operator==(other); }

    friend class LinkedList<Linkage::DoubleLinkage, value_type>;
    friend class DLinkedList<value_type>;
};

template<typename ValueType>
class ReverseListIterator
{
private:
    using node_type = typename LinkedList<Linkage::DoubleLinkage, ValueType>::node_type;
    node_type* node;
    bool is_reverse() { return true; }
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = ValueType;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ReverseListIterator(node_type* node = nullptr) : node{node} { }
    ReverseListIterator(const ListIterator<value_type>& other) : node{other.node} { }
    ReverseListIterator(const ConstListIterator<value_type>& other) : node{other.node} { }
    ReverseListIterator(const ReverseListIterator<value_type>& other) : node{other.node} { }
    ReverseListIterator(const ConstReverseListIterator<value_type>& other) : node{other.node} { }

    ReverseListIterator<value_type>& operator=(const ListIterator<value_type>& other) { node = other.node; return *this; }
    ReverseListIterator<value_type>& operator=(const ConstListIterator<value_type>& other) { node = other.node; return *this; }
    ReverseListIterator<value_type>& operator=(const ReverseListIterator<value_type>& other) { node = other.node; return *this; }
    ReverseListIterator<value_type>& operator=(const ConstReverseListIterator<value_type>& other) { node = other.node; return *this; }
    ~ReverseListIterator() = default;

    reference operator*() const { return node->data; }
    pointer operator->() { return &(node->data); }
    ReverseListIterator<value_type>& operator++() { node = node->prev; return *this; }
    ReverseListIterator<value_type>& operator--() { node = node->next.get(); return *this; }
    const ReverseListIterator<value_type> operator++(int)
    {
        ReverseListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    const ReverseListIterator<value_type> operator--(int)
    {
        ReverseListIterator<value_type> temp{*this};
        operator--();
        return temp;
    }
    bool operator==(const ReverseListIterator<value_type>& other) const noexcept { return node == other.node; }
    bool operator!=(const ReverseListIterator<value_type>& other) const noexcept { return !operator==(other); }

    friend class LinkedList<Linkage::DoubleLinkage, value_type>;
    friend class DLinkedList<value_type>;
};

template<typename ValueType>
class ConstReverseListIterator
{
private:
    using node_type = typename LinkedList<Linkage::DoubleLinkage, ValueType>::node_type;
    node_type* node;
    bool is_reverse() { return true; }
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = ValueType;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ConstReverseListIterator(node_type* node = nullptr) : node{node} { }
    ConstReverseListIterator(const ListIterator<value_type>& other) : node{other.node} { }
    ConstReverseListIterator(const ConstListIterator<value_type>& other) : node{other.node} { }
    ConstReverseListIterator(const ReverseListIterator<value_type>& other) : node{other.node} { }
    ConstReverseListIterator(const ConstReverseListIterator<value_type>& other) : node{other.node} { }

    ConstReverseListIterator<value_type>& operator=(const ListIterator<value_type>& other) { node = other.node; return *this; }
    ConstReverseListIterator<value_type>& operator=(const ConstListIterator<value_type>& other) { node = other.node; return *this; }
    ConstReverseListIterator<value_type>& operator=(const ReverseListIterator<value_type>& other) { node = other.node; return *this; }
    ConstReverseListIterator<value_type>& operator=(const ConstReverseListIterator<value_type>& other) { node = other.node; return *this; }
    ~ConstReverseListIterator() = default;

    const_reference operator*() const { return node->data; }
    const_pointer operator->() { return &(node->data); }
    ConstReverseListIterator<value_type>& operator++() { node = node->prev; return *this; }
    ConstReverseListIterator<value_type>& operator--() { node = node->next.get(); return *this; }
    const ConstReverseListIterator<value_type> operator++(int)
    {
        ConstReverseListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    const ConstReverseListIterator<value_type> operator--(int)
    {
        ConstReverseListIterator<value_type> temp{*this};
        operator--();
        return temp;
    }
    bool operator==(const ConstReverseListIterator<value_type>& other) const noexcept { return node == other.node; }
    bool operator!=(const ConstReverseListIterator<value_type>& other) const noexcept { return !operator==(other); }

    friend class LinkedList<Linkage::DoubleLinkage, value_type>;
    friend class DLinkedList<value_type>;
};

template<typename ValueType>
class ForwardListIterator
{
private:
    using node_type = typename LinkedList<Linkage::SingleLinkage, ValueType>::node_type;
    node_type* node;
    bool is_reverse() { return false; }
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = ValueType;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ForwardListIterator(node_type* node = nullptr) : node{node} { }
    ForwardListIterator(const ForwardListIterator<value_type>& other) : node{other.node} { }
    ForwardListIterator(const ConstForwardListIterator<value_type>& other) : node{other.node} { }

    ForwardListIterator<value_type>& operator=(const ForwardListIterator<value_type>& other) { node = other.node; return *this; }
    ForwardListIterator<value_type>& operator=(const ConstForwardListIterator<value_type>& other) { node = other.node; return *this; }
    ~ForwardListIterator() = default;

    reference operator*() const { return node->data; }
    pointer operator->() { return &(node->data); }
    ForwardListIterator<value_type>& operator++() { node = node->next.get(); return *this; }
    ForwardListIterator<value_type>& operator--() { throw std::logic_error("Cannot decrement forward iterator."); }
    const ForwardListIterator<value_type> operator++(int)
    { 
        ForwardListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    const ForwardListIterator<value_type> operator--(int) { throw std::logic_error("Cannot decrement forward iterator."); }
    bool operator==(const ForwardListIterator<value_type>& other) const noexcept { return node == other.node; }
    bool operator!=(const ForwardListIterator<value_type>& other) const noexcept { return !operator==(other); }

    friend class LinkedList<Linkage::SingleLinkage, value_type>;
    friend class SLinkedList<value_type>;
};

template<typename ValueType>
class ConstForwardListIterator
{
private:
    using node_type = typename LinkedList<Linkage::SingleLinkage, ValueType>::node_type;
    node_type* node;
    bool is_reverse() { return false; }
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = ValueType;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ConstForwardListIterator(node_type* node = nullptr) : node{node} { }
    ConstForwardListIterator(const ForwardListIterator<value_type>& other) : node{other.node} { }
    ConstForwardListIterator(const ConstForwardListIterator<value_type>& other) : node{other.node} { }

    ConstForwardListIterator<value_type>& operator=(const ForwardListIterator<value_type>& other) { node = other.node; return *this; }
    ConstForwardListIterator<value_type>& operator=(const ConstForwardListIterator<value_type>& other) { node = other.node; return *this; }
    ~ConstForwardListIterator() = default;

    const_reference operator*() const { return node->data; }
    const_pointer operator->() { return &(node->data); }
    ConstForwardListIterator<value_type>& operator++() { node = node->next.get(); return *this; }
    ConstForwardListIterator<value_type>& operator--() { throw std::logic_error("Cannot decrement forward iterator."); }
    const ConstForwardListIterator<value_type> operator++(int)
    {
        ConstForwardListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    const ConstForwardListIterator<value_type> operator--(int) { throw std::logic_error("Cannot decrement forward iterator."); }
    bool operator==(const ConstForwardListIterator<value_type>& other) const noexcept { return node == other.node; }
    bool operator!=(const ConstForwardListIterator<value_type>& other) const noexcept { return !operator==(other); }

    friend class LinkedList<Linkage::SingleLinkage, value_type>;
    friend class SLinkedList<value_type>;
};

}  // end namespace

#endif