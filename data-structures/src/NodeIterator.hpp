#ifndef NODEITERATOR_HPP
#define NODEITERATOR_HPP

#include <stdexcept>
#include <utility>
#include "Linkage.hpp"

namespace bork_lib
{

template<Linkage L, typename V> class LinkedList;
template<typename V> class DLinkedList;
template<typename V> class SLinkedList;
template<typename B, typename K, typename V> class BinarySearchTree;
template<typename K, typename V> class AVLTree;
template<typename K, typename V> class RedBlackTree;

template<typename V> class ConstListIterator;
template<typename V> class ReverseListIterator;
template<typename V> class ConstReverseListIterator;
template<typename V> class ConstForwardListIterator;
template<typename N, typename K, typename V> class ConstTreeIterator;
template<typename N, typename K, typename V> class ReverseTreeIterator;
template<typename N, typename K, typename V> class ConstReverseTreeIterator;

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

template<typename NodeType>
NodeType* succ(NodeType* node)
{
    NodeType* succ_node;
    if (node->right) {  // find the leftmost node in node's right subtree
        succ_node = node->right.get();
        while (succ_node->left)
            succ_node = succ_node->left.get();
    } else {  // find the lowest ancestor of node whose left child is also an ancestor of node
        succ_node = node->parent;
        while (succ_node && node == succ_node->right.get()) {
            node = succ_node;
            succ_node = succ_node->parent;
        }
    }

    return succ_node;
}

template<typename NodeType>
NodeType* pred(NodeType* node)
{
    NodeType* pred_node;
    if (node->left) {  // find the rightmost node in node's left subtree
        pred_node = node->left.get();
        while (pred_node->right)
            pred_node = pred_node->right.get();
    } else {  // find the lowest ancestor of node whose right child is also an ancestor of node
        pred_node = node->parent;
        while (pred_node && node == pred_node->left.get()) {
            node = pred_node;
            pred_node = pred_node->parent;
        }
    }

    return pred_node;
}

template<typename NodeType, typename KeyType, typename ValueType>
class TreeIterator
{
private:
    NodeType* node;
    using tree_iterator = TreeIterator<NodeType, KeyType, ValueType>;
    using const_tree_iterator = ConstTreeIterator<NodeType, KeyType, ValueType>;
    using reverse_tree_iterator = ReverseTreeIterator<NodeType, KeyType, ValueType>;
    using const_reverse_tree_iterator = ConstReverseTreeIterator<NodeType, KeyType, ValueType>;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair<const KeyType, ValueType>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    TreeIterator(NodeType* node) : node{node} { }
    TreeIterator(const tree_iterator& other) : node{other.node} { }
    TreeIterator(const const_tree_iterator& other) : node{other.node} { }
    TreeIterator(const reverse_tree_iterator& other) : node{other.node} { }
    TreeIterator(const const_reverse_tree_iterator& other) : node{other.node} { }

    tree_iterator& operator=(const tree_iterator& other) { node = other.node; return *this; }
    tree_iterator& operator=(const const_tree_iterator& other) { node = other.node; return *this; }
    tree_iterator& operator=(const reverse_tree_iterator& other) { node = other.node; return *this; }
    tree_iterator& operator=(const const_reverse_tree_iterator& other) { node = other.node; return *this; }
    ~TreeIterator() = default;

    reference operator*() const { return node->data; }
    pointer operator->() { return &(node->data); }
    tree_iterator& operator++() { node = succ<NodeType>(node); return *this; }
    tree_iterator& operator--() { node = pred<NodeType>(node); return *this; }
    const tree_iterator operator++(int)
    {
        tree_iterator temp{*this};
        operator++();
        return temp;
    }
    const tree_iterator operator--(int)
    {
        tree_iterator temp{*this};
        operator--();
        return temp;
    }
    bool operator==(const tree_iterator& other) const noexcept { return node == other.node; }
    bool operator!=(const tree_iterator& other) const noexcept { return !operator==(other); }

    friend class BinarySearchTree<NodeType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

template<typename NodeType, typename KeyType, typename ValueType>
class ConstTreeIterator
{
private:
    NodeType* node;
    using tree_iterator = TreeIterator<NodeType, KeyType, ValueType>;
    using const_tree_iterator = ConstTreeIterator<NodeType, KeyType, ValueType>;
    using reverse_tree_iterator = ReverseTreeIterator<NodeType, KeyType, ValueType>;
    using const_reverse_tree_iterator = ConstReverseTreeIterator<NodeType, KeyType, ValueType>;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair<const KeyType, ValueType>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    ConstTreeIterator(NodeType* node) : node{node} { }
    ConstTreeIterator(const tree_iterator& other) : node{other.node} { }
    ConstTreeIterator(const const_tree_iterator& other) : node{other.node} { }
    ConstTreeIterator(const reverse_tree_iterator& other) : node{other.node} { }
    ConstTreeIterator(const const_reverse_tree_iterator& other) : node{other.node} { }

    const_tree_iterator& operator=(const tree_iterator& other) { node = other.node; return *this; }
    const_tree_iterator& operator=(const const_tree_iterator& other) { node = other.node; return *this; }
    const_tree_iterator& operator=(const reverse_tree_iterator& other) { node = other.node; return *this; }
    const_tree_iterator& operator=(const const_reverse_tree_iterator& other) { node = other.node; return *this; }
    ~ConstTreeIterator() = default;

    const_reference operator*() const { return node->data; }
    const_pointer operator->() { return &(node->data); }
    const_tree_iterator& operator++() { node = succ<NodeType>(node); return *this; }
    const_tree_iterator& operator--() { node = pred<NodeType>(node); return *this; }
    const const_tree_iterator operator++(int)
    {
        const_tree_iterator temp{*this};
        operator++();
        return temp;
    }
    const const_tree_iterator operator--(int)
    {
        const_tree_iterator temp{*this};
        operator--();
        return temp;
    }
    bool operator==(const const_tree_iterator& other) const noexcept { return node == other.node; }
    bool operator!=(const const_tree_iterator& other) const noexcept { return !operator==(other); }

    friend class BinarySearchTree<NodeType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

template<typename NodeType, typename KeyType, typename ValueType>
class ReverseTreeIterator
{
private:
    NodeType* node;
    using tree_iterator = TreeIterator<NodeType, KeyType, ValueType>;
    using const_tree_iterator = ConstTreeIterator<NodeType, KeyType, ValueType>;
    using reverse_tree_iterator = ReverseTreeIterator<NodeType, KeyType, ValueType>;
    using const_reverse_tree_iterator = ConstReverseTreeIterator<NodeType, KeyType, ValueType>;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair<const KeyType, ValueType>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    ReverseTreeIterator(NodeType* node) : node{node} { }
    ReverseTreeIterator(const tree_iterator& other) : node{other.node} { }
    ReverseTreeIterator(const const_tree_iterator& other) : node{other.node} { }
    ReverseTreeIterator(const reverse_tree_iterator& other) : node{other.node} { }
    ReverseTreeIterator(const const_reverse_tree_iterator& other) : node{other.node} { }

    reverse_tree_iterator& operator=(const tree_iterator& other) { node = other.node; return *this; }
    reverse_tree_iterator& operator=(const const_tree_iterator& other) { node = other.node; return *this; }
    reverse_tree_iterator& operator=(const reverse_tree_iterator& other) { node = other.node; return *this; }
    reverse_tree_iterator& operator=(const const_reverse_tree_iterator& other) { node = other.node; return *this; }
    ~ReverseTreeIterator() = default;

    reference operator*() const { return node->data; }
    pointer operator->() { return &(node->data); }
    reverse_tree_iterator& operator++() { node = pred<NodeType>(node); return *this; }
    reverse_tree_iterator& operator--() { node = succ<NodeType>(node); return *this; }
    const reverse_tree_iterator operator++(int)
    {
        reverse_tree_iterator temp{*this};
        operator++();
        return temp;
    }
    const reverse_tree_iterator operator--(int)
    {
        reverse_tree_iterator temp{*this};
        operator--();
        return temp;
    }
    bool operator==(const reverse_tree_iterator& other) const noexcept { return node == other.node; }
    bool operator!=(const reverse_tree_iterator& other) const noexcept { return !operator==(other); }

    friend class BinarySearchTree<NodeType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

template<typename NodeType, typename KeyType, typename ValueType>
class ConstReverseTreeIterator
{
private:
    NodeType* node;
    using tree_iterator = TreeIterator<NodeType, KeyType, ValueType>;
    using const_tree_iterator = ConstTreeIterator<NodeType, KeyType, ValueType>;
    using reverse_tree_iterator = ReverseTreeIterator<NodeType, KeyType, ValueType>;
    using const_reverse_tree_iterator = ConstReverseTreeIterator<NodeType, KeyType, ValueType>;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair<const KeyType, ValueType>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    ConstReverseTreeIterator(NodeType* node) : node{node} { }
    ConstReverseTreeIterator(const tree_iterator& other) : node{other.node} { }
    ConstReverseTreeIterator(const const_tree_iterator& other) : node{other.node} { }
    ConstReverseTreeIterator(const reverse_tree_iterator& other) : node{other.node} { }
    ConstReverseTreeIterator(const const_reverse_tree_iterator& other) : node{other.node} { }

    const_reverse_tree_iterator& operator=(const tree_iterator& other) { node = other.node; return *this; }
    const_reverse_tree_iterator& operator=(const const_tree_iterator& other) { node = other.node; return *this; }
    const_reverse_tree_iterator& operator=(const reverse_tree_iterator& other) { node = other.node; return *this; }
    const_reverse_tree_iterator& operator=(const const_reverse_tree_iterator& other) { node = other.node; return *this; }
    ~ConstReverseTreeIterator() = default;

    reference operator*() { return node->data; }
    pointer operator->() { return &(node->data); }
    const_reverse_tree_iterator& operator++() { node = pred<NodeType>(node); return *this; }
    const_reverse_tree_iterator& operator--() { node = succ<NodeType>(node); return *this; }
    const const_reverse_tree_iterator operator++(int)
    {
        const_reverse_tree_iterator temp{*this};
        operator++();
        return temp;
    }
    const const_reverse_tree_iterator operator--(int)
    {
        const_reverse_tree_iterator temp{*this};
        operator--();
        return temp;
    }
    bool operator==(const const_reverse_tree_iterator& other) const noexcept { return node == other.node; }
    bool operator!=(const const_reverse_tree_iterator& other) const noexcept { return !operator==(other); }

    friend class BinarySearchTree<NodeType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

}  // end namespace

#endif