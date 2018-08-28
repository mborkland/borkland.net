#ifndef NODEITERATOR_HPP
#define NODEITERATOR_HPP

#include <stdexcept>
#include <utility>

namespace bork_lib
{

class SingleLinkage;
class DoubleLinkage;
template<typename L, typename V> class LinkedList;
template<typename V> class DLinkedList;
template<typename V> class SLinkedList;
template<typename B, typename K, typename V> class BinarySearchTree;
template<typename K, typename V> class AVLTree;
template<typename K, typename V> class RedBlackTree;

template<typename NodeType, typename ValueType>
class NodeIterator
{
protected:
    NodeType* node;

public:
    using value_type = ValueType;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit NodeIterator(NodeType* node = nullptr) : node{node} {}
    NodeIterator(const NodeIterator<NodeType, value_type>& other) : node(other.node) {}
    NodeIterator<NodeType, value_type>& operator=(const NodeIterator<NodeType, value_type>& other)
    {
        node = other.node;
        return *this;
    }
    virtual NodeIterator<NodeType, value_type>& operator++() = 0;
    virtual NodeIterator<NodeType, value_type>& operator--() = 0;
    reference operator*() { return node->data; }
    pointer operator->() { return &(node->data); }
    bool operator==(const NodeIterator<NodeType, value_type>& other) const noexcept
    {
        return node == other.node;
    }

    bool operator!=(const NodeIterator<NodeType, value_type>& other) const noexcept
    {
        return !operator==(other);
    }
    virtual ~NodeIterator() = default;

    template<typename L, typename V> friend class LinkedList;
    friend class DLinkedList<value_type>;
    friend class SLinkedList<value_type>;
    template<typename B, typename K, typename V> friend class BinarySearchTree;
    template<typename K, typename V> friend class AVLTree;
    template<typename K, typename V> friend class RedBlackTree;
};

template<typename NodeType, typename ValueType>
class ConstNodeIterator
{
protected:
    NodeType* node;

public:
    using value_type = ValueType;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ConstNodeIterator(NodeType* node = nullptr) : node{node} {}
    ConstNodeIterator(const ConstNodeIterator<NodeType, value_type>& other) : node(other.node) {}
    ConstNodeIterator<NodeType, value_type>& operator=(const ConstNodeIterator<NodeType, value_type>& other)
    {
        node = other.node;
        return *this;
    }
    virtual ConstNodeIterator<NodeType, value_type>& operator++() = 0;
    virtual ConstNodeIterator<NodeType, value_type>& operator--() = 0;
    const_reference operator*() { return node->data; }
    const_pointer operator->() { return &(node->data); }
    bool operator==(const ConstNodeIterator<NodeType, value_type>& other) const noexcept
    {
        return node == other.node;
    }
    bool operator!=(const ConstNodeIterator<NodeType, value_type>& other) const noexcept
    {
        return !operator==(other);
    }
    virtual ~ConstNodeIterator() = default;

    template<typename L, typename V> friend class LinkedList;
    template<typename V> friend class DLinkedList;
    template<typename V> friend class SLinkedList;
    template<typename B, typename K, typename V> friend class BinarySearchTree;
    template<typename K, typename V> friend class AVLTree;
    template<typename K, typename V> friend class RedBlackTree;
};

template<typename ValueType>
class ListIterator : public NodeIterator<typename LinkedList<DoubleLinkage, ValueType>::NodeType, ValueType>
{
    using NodeType = typename LinkedList<DoubleLinkage, ValueType>::NodeType;
    using NodeIterator<NodeType, ValueType>::NodeIterator;
    using NodeIterator<NodeType, ValueType>::node;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename NodeIterator<NodeType, ValueType>::value_type;
    using reference = typename NodeIterator<NodeType, value_type>::reference;
    using const_reference = typename NodeIterator<NodeType, value_type>::const_reference;
    using pointer = typename NodeIterator<NodeType, value_type>::pointer;
    using const_pointer = typename NodeIterator<NodeType, value_type>::const_pointer;
    using size_type = typename NodeIterator<NodeType, value_type>::size_type;
    using difference_type = typename NodeIterator<NodeType, value_type>::difference_type;

    ListIterator<value_type>& operator=(const NodeIterator<NodeType, value_type>& other)
    {
        return *this = dynamic_cast<ListIterator<value_type>&>(NodeIterator<NodeType, value_type>::operator=(other));
    }
    ListIterator<value_type>& operator++() override { node = node->next.get(); return *this; }
    ListIterator<value_type>& operator--() override { node = node->prev; return *this; }
    virtual ListIterator<value_type> operator++(int)
    {
        ListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    virtual ListIterator<value_type> operator--(int)
    {
        ListIterator<value_type> temp{*this};
        operator--();
        return temp;
    }
    friend class DLinkedList<value_type>;
};

template<typename ValueType>
class ConstListIterator : public ConstNodeIterator<typename LinkedList<DoubleLinkage, ValueType>::NodeType, ValueType>
{
    using NodeType = typename LinkedList<DoubleLinkage, ValueType>::NodeType;
    using ConstNodeIterator<NodeType, ValueType>::ConstNodeIterator;
    using ConstNodeIterator<NodeType, ValueType>::node;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename ConstNodeIterator<NodeType, ValueType>::value_type;
    using reference = typename ConstNodeIterator<NodeType, value_type>::reference;
    using const_reference = typename ConstNodeIterator<NodeType, value_type>::const_reference;
    using pointer = typename ConstNodeIterator<NodeType, value_type>::pointer;
    using const_pointer = typename ConstNodeIterator<NodeType, value_type>::const_pointer;
    using size_type = typename ConstNodeIterator<NodeType, value_type>::size_type;
    using difference_type = typename ConstNodeIterator<NodeType, value_type>::difference_type;

    ConstListIterator<value_type>& operator=(const ConstNodeIterator<NodeType, value_type>& other)
    {
        return *this = dynamic_cast<ConstListIterator<value_type>&>(ConstNodeIterator<NodeType, value_type>::operator=(other));
    }
    virtual ConstListIterator<value_type>& operator++() { node = node->next.get(); return *this; }
    virtual ConstListIterator<value_type>& operator--() { node = node->prev; return *this; }
    virtual ConstListIterator<value_type> operator++(int)
    {
        ConstListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    virtual ConstListIterator<value_type> operator--(int)
    {
        ConstListIterator<value_type> temp{*this};
        operator--();
        return temp;
    }
    friend class DLinkedList<value_type>;
};

template<typename ValueType>
class ReverseListIterator : public NodeIterator<typename LinkedList<DoubleLinkage, ValueType>::NodeType, ValueType>
{
    using NodeType = typename LinkedList<DoubleLinkage, ValueType>::NodeType;
    using NodeIterator<NodeType, ValueType>::NodeIterator;
    using NodeIterator<NodeType, ValueType>::node;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename NodeIterator<NodeType, ValueType>::value_type;
    using reference = typename NodeIterator<NodeType, value_type>::reference;
    using const_reference = typename NodeIterator<NodeType, value_type>::const_reference;
    using pointer = typename NodeIterator<NodeType, value_type>::pointer;
    using const_pointer = typename NodeIterator<NodeType, value_type>::const_pointer;
    using size_type = typename NodeIterator<NodeType, value_type>::size_type;
    using difference_type = typename NodeIterator<NodeType, value_type>::difference_type;

    ReverseListIterator<value_type>& operator=(const NodeIterator<NodeType, value_type>& other)
    {
        return *this = dynamic_cast<ReverseListIterator<value_type>&>(NodeIterator<NodeType, value_type>::operator=(other));
    }
    virtual ReverseListIterator<value_type>& operator++() { node = node->prev; return *this; }
    virtual ReverseListIterator<value_type>& operator--() { node = node->next.get(); return *this; }
    virtual ReverseListIterator<value_type> operator++(int)
    {
        ReverseListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    virtual ReverseListIterator<value_type> operator--(int)
    {
        ReverseListIterator<value_type> temp{*this};
        operator--();
        return temp;
    }
    friend class DLinkedList<value_type>;
};

template<typename ValueType>
class ConstReverseListIterator : public ConstNodeIterator<typename LinkedList<DoubleLinkage, ValueType>::NodeType, ValueType>
{
    using NodeType = typename LinkedList<DoubleLinkage, ValueType>::NodeType;
    using ConstNodeIterator<NodeType, ValueType>::ConstNodeIterator;
    using ConstNodeIterator<NodeType, ValueType>::node;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename ConstNodeIterator<NodeType, ValueType>::value_type;
    using reference = typename ConstNodeIterator<NodeType, value_type>::reference;
    using const_reference = typename ConstNodeIterator<NodeType, value_type>::const_reference;
    using pointer = typename ConstNodeIterator<NodeType, value_type>::pointer;
    using const_pointer = typename ConstNodeIterator<NodeType, value_type>::const_pointer;
    using size_type = typename ConstNodeIterator<NodeType, value_type>::size_type;
    using difference_type = typename ConstNodeIterator<NodeType, value_type>::difference_type;

    ConstReverseListIterator<value_type>& operator=(const ConstNodeIterator<NodeType, value_type>& other)
    {
        return *this = dynamic_cast<ConstReverseListIterator<value_type>&>(ConstNodeIterator<NodeType, value_type>::operator=(other));
    }
    virtual ConstReverseListIterator<value_type>& operator++() { node = node->prev; return *this; }
    virtual ConstReverseListIterator<value_type>& operator--() { node = node->next.get(); return *this; }
    virtual ConstReverseListIterator<value_type> operator++(int)
    {
        ConstReverseListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    virtual ConstReverseListIterator<value_type> operator--(int)
    {
        ConstReverseListIterator<value_type> temp{*this};
        operator--();
        return temp;
    }
    friend class DLinkedList<value_type>;
};

template<typename ValueType>
class ForwardListIterator : public NodeIterator<typename LinkedList<SingleLinkage, ValueType>::NodeType, ValueType>
{
    using NodeType = typename LinkedList<SingleLinkage, ValueType>::NodeType;
    using NodeIterator<NodeType, ValueType>::NodeIterator;
    using NodeIterator<NodeType, ValueType>::node;
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename NodeIterator<NodeType, ValueType>::value_type;
    using reference = typename NodeIterator<NodeType, value_type>::reference;
    using const_reference = typename NodeIterator<NodeType, value_type>::const_reference;
    using pointer = typename NodeIterator<NodeType, value_type>::pointer;
    using const_pointer = typename NodeIterator<NodeType, value_type>::const_pointer;
    using size_type = typename NodeIterator<NodeType, value_type>::size_type;
    using difference_type = typename NodeIterator<NodeType, value_type>::difference_type;

    ForwardListIterator<value_type>& operator=(const NodeIterator<NodeType, value_type>& other)
    {
        return *this = dynamic_cast<ForwardListIterator<value_type>&>(NodeIterator<NodeType, value_type>::operator=(other));
    }
    virtual ForwardListIterator<value_type>& operator++() { node = node->next.get(); return *this; }
    virtual ForwardListIterator<value_type>& operator--() { throw std::logic_error("Cannot decrement forward iterator."); }
    virtual ForwardListIterator<value_type> operator++(int)
    { 
        ForwardListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    virtual ForwardListIterator<value_type> operator--(int) { throw std::logic_error("Cannot decrement forward iterator."); }
    friend class SLinkedList<value_type>;
};

template<typename ValueType>
class ConstForwardListIterator : public ConstNodeIterator<typename LinkedList<SingleLinkage, ValueType>::NodeType, ValueType>
{
    using NodeType = typename LinkedList<SingleLinkage, ValueType>::NodeType;
    using ConstNodeIterator<NodeType, ValueType>::ConstNodeIterator;
    using ConstNodeIterator<NodeType, ValueType>::node;
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename ConstNodeIterator<NodeType, ValueType>::value_type;
    using reference = typename ConstNodeIterator<NodeType, value_type>::reference;
    using const_reference = typename ConstNodeIterator<NodeType, value_type>::const_reference;
    using pointer = typename ConstNodeIterator<NodeType, value_type>::pointer;
    using const_pointer = typename ConstNodeIterator<NodeType, value_type>::const_pointer;
    using size_type = typename ConstNodeIterator<NodeType, value_type>::size_type;
    using difference_type = typename ConstNodeIterator<NodeType, value_type>::difference_type;

    ConstForwardListIterator<value_type>& operator=(const ConstNodeIterator<NodeType, value_type>& other)
    {
        return *this = dynamic_cast<ConstForwardListIterator<value_type>&>(ConstNodeIterator<NodeType, value_type>::operator=(other));
    }
    virtual ConstForwardListIterator<value_type>& operator++() { node = node->next.get(); return *this; }
    virtual ConstForwardListIterator<value_type>& operator--() { throw std::logic_error("Cannot decrement forward iterator."); }
    virtual ConstForwardListIterator<value_type> operator++(int)
    {
        ConstForwardListIterator<value_type> temp{*this};
        operator++();
        return temp;
    }
    virtual ConstForwardListIterator<value_type> operator--(int) { throw std::logic_error("Cannot decrement forward iterator."); }
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
class TreeIterator : public NodeIterator<NodeType, std::pair<const KeyType, ValueType>>
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using NodeIterator<NodeType, std::pair<const KeyType, ValueType>>::value_type;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::reference;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::const_reference;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::pointer;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::const_pointer;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::size_type;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::difference_type;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::NodeIterator;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::node;

    TreeIterator<NodeType, KeyType, value_type>& operator=(const NodeIterator<NodeType, std::pair<const KeyType, value_type>>& other)
    {
        return *this = dynamic_cast<TreeIterator<NodeType, KeyType, value_type>&>(
               NodeIterator<NodeType, std::pair<const KeyType, value_type>>::operator=(other));
    }
    virtual TreeIterator<NodeType, KeyType, value_type>& operator++()
    {
        node = succ<NodeType>(node);
        return *this;
    }
    virtual TreeIterator<NodeType, KeyType, value_type>& operator--()
    {
        node = pred<NodeType>(node);
        return *this;
    }
    virtual TreeIterator<NodeType, KeyType, value_type> operator++(int)
    {
        TreeIterator<NodeType, KeyType, value_type> temp{*this};
        operator++();
        return temp;
    }
    virtual TreeIterator<NodeType, KeyType, value_type> operator--(int)
    {
        TreeIterator<NodeType, KeyType, value_type> temp{*this};
        operator--();
        return temp;
    }
    friend class BinarySearchTree<NodeType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

template<typename NodeType, typename KeyType, typename ValueType>
class ConstTreeIterator : public ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>::value_type;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::reference;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::const_reference;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::pointer;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::const_pointer;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::size_type;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::difference_type;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::ConstNodeIterator;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::node;

    ConstTreeIterator<NodeType, KeyType, value_type>& operator=(const ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>& other)
    {
        return *this = dynamic_cast<ConstTreeIterator<NodeType, KeyType, value_type>&>(
               ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::operator=(other));
    }
    virtual ConstTreeIterator<NodeType, KeyType, value_type>& operator++()
    {
        node = succ<NodeType>(node);
        return *this;
    }
    virtual ConstTreeIterator<NodeType, KeyType, value_type>& operator--()
    {
        node = pred<NodeType>(node);
        return *this;
    }
    virtual ConstTreeIterator<NodeType, KeyType, value_type> operator++(int)
    {
        ConstTreeIterator<NodeType, KeyType, value_type> temp{*this};
        operator++();
        return temp;
    }
    virtual ConstTreeIterator<NodeType, KeyType, value_type> operator--(int)
    {
        ConstTreeIterator<NodeType, KeyType, value_type> temp{*this};
        operator--();
        return temp;
    }
    friend class BinarySearchTree<NodeType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

template<typename NodeType, typename KeyType, typename ValueType>
class ReverseTreeIterator : public NodeIterator<NodeType, std::pair<const KeyType, ValueType>>
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using NodeIterator<NodeType, std::pair<const KeyType, ValueType>>::value_type;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::reference;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::const_reference;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::pointer;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::const_pointer;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::size_type;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::difference_type;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::NodeIterator;
    using NodeIterator<NodeType, std::pair<const KeyType, value_type>>::node;

    ReverseTreeIterator<NodeType, KeyType, value_type>& operator=(const NodeIterator<NodeType, std::pair<const KeyType, value_type>>& other)
    {
        return *this = dynamic_cast<ReverseTreeIterator<NodeType, KeyType, value_type>&>(
               NodeIterator<NodeType, std::pair<const KeyType, value_type>>::operator=(other));
    }
    virtual ReverseTreeIterator<NodeType, KeyType, value_type>& operator++()
    {
        node = pred<NodeType>(node);
        return *this;
    }
    virtual ReverseTreeIterator<NodeType, KeyType, value_type>& operator--()
    {
        node = succ<NodeType>(node);
        return *this;
    }
    virtual ReverseTreeIterator<NodeType, KeyType, value_type> operator++(int)
    {
        ReverseTreeIterator<NodeType, KeyType, value_type> temp{*this};
        operator++();
        return temp;
    }
    virtual ReverseTreeIterator<NodeType, KeyType, value_type> operator--(int)
    {
        ReverseTreeIterator<NodeType, KeyType, value_type> temp{*this};
        operator--();
        return temp;
    }
    friend class BinarySearchTree<NodeType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

template<typename NodeType, typename KeyType, typename ValueType>
class ConstReverseTreeIterator : public ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>
{
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>::value_type;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::reference;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::const_reference;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::pointer;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::const_pointer;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::size_type;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::difference_type;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::ConstNodeIterator;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, value_type>>::node;

    ConstReverseTreeIterator<NodeType, KeyType, value_type>& operator=(const NodeIterator<NodeType, std::pair<const KeyType, value_type>>& other)
    {
        return *this = dynamic_cast<ConstReverseTreeIterator<NodeType, KeyType, value_type>&>(
               NodeIterator<NodeType, std::pair<const KeyType, value_type>>::operator=(other));
    }
    virtual ConstReverseTreeIterator<NodeType, KeyType, value_type>& operator++()
    {
        node = pred<NodeType>(node);
        return *this;
    }
    virtual ConstReverseTreeIterator<NodeType, KeyType, value_type>& operator--()
    {
        node = succ<NodeType>(node);
        return *this;
    }
    virtual ConstReverseTreeIterator<NodeType, KeyType, value_type> operator++(int)
    {
        ConstReverseTreeIterator<NodeType, KeyType, value_type> temp{*this};
        operator++();
        return temp;
    }
    virtual ConstReverseTreeIterator<NodeType, KeyType, value_type> operator--(int)
    {
        ConstReverseTreeIterator<NodeType, KeyType, value_type> temp{*this};
        operator--();
        return temp;
    }
    friend class BinarySearchTree<NodeType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

}  // end namespace

#endif