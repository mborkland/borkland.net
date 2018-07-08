#ifndef NODEITERATOR_HPP
#define NODEITERATOR_HPP

#include <stdexcept>
#include <utility>
#include "BinarySearchTree.hpp"

namespace bork_lib
{

template<typename T> class DLinkedList;
template<typename T> class SLinkedList;
template<typename B, typename K, typename V> class BinarySearchTree;
template<typename K, typename V> class AVLTree;
template<typename K, typename V> class RedBlackTree;

template<typename NodeType, typename ValueType>
class NodeIterator
{
protected:
    NodeType* node;

public:
    NodeIterator(NodeType* node = nullptr) : node{node} {}
    NodeIterator(const NodeIterator<NodeType, ValueType>& other) : node(other.node) {}
    NodeIterator<NodeType, ValueType>& operator=(const NodeIterator<NodeType, ValueType>& other)
    {
        node = other.node;
        return *this;
    }
    virtual NodeIterator<NodeType, ValueType>& operator++() = 0;
    virtual NodeIterator<NodeType, ValueType>& operator--() = 0;
    virtual ValueType& operator*() { return node->data; }
    virtual ValueType* operator->() { return &(node->data); }
    bool operator==(const NodeIterator<NodeType, ValueType>& other) const noexcept
    {
        return node == other.node;
    }

    bool operator!=(const NodeIterator<NodeType, ValueType>& other) const noexcept
    {
        return !operator==(other);
    }
    virtual ~NodeIterator() = default;
    friend class DLinkedList<ValueType>;
    friend class SLinkedList<ValueType>;
    template<typename B, typename K, typename V>
    friend class BinarySearchTree;
    template<typename K, typename V>
    friend class AVLTree;
    template<typename K, typename V>
    friend class RedBlackTree;
};

template<typename NodeType, typename ValueType>
class ConstNodeIterator
{
protected:
    NodeType* node;

public:
    ConstNodeIterator(NodeType* node = nullptr) : node{node} {}
    ConstNodeIterator(const ConstNodeIterator<NodeType, ValueType>& other) : node(other.node) {}
    ConstNodeIterator<NodeType, ValueType>& operator=(const ConstNodeIterator<NodeType, ValueType>& other) = delete;
    virtual ConstNodeIterator<NodeType, ValueType>& operator++() = 0;
    virtual ConstNodeIterator<NodeType, ValueType>& operator--() = 0;
    virtual const ValueType& operator*() { return node->data; }
    virtual const ValueType* operator->() { return &(node->data); }
    bool operator==(const ConstNodeIterator<NodeType, ValueType>& other) const noexcept
    {
        return node == other.node;
    }

    bool operator!=(const ConstNodeIterator<NodeType, ValueType>& other) const noexcept
    {
        return !operator==(other);
    }
    virtual ~ConstNodeIterator() = default;
    friend class DLinkedList<ValueType>;
    friend class SLinkedList<ValueType>;
    template<typename B, typename K, typename V>
    friend class BinarySearchTree;
    template<typename K, typename V>
    friend class AVLTree;
    template<typename K, typename V>
    friend class RedBlackTree;
};

template<typename ValueType>
using DNodeType = typename DLinkedList<ValueType>::DNode;

template<typename ValueType>
class ListIterator : public NodeIterator<DNodeType<ValueType>, ValueType>
{   
    using NodeIterator<DNodeType<ValueType>, ValueType>::NodeIterator;
    using NodeIterator<DNodeType<ValueType>, ValueType>::node;
public:
    ListIterator<ValueType>& operator=(const NodeIterator<DNodeType<ValueType>, ValueType>& other)
    {
        return *this = dynamic_cast<ListIterator<ValueType>&>(NodeIterator<DNodeType<ValueType>, ValueType>::operator=(other));
    }
    virtual ListIterator<ValueType>& operator++() { node = node->next; return *this; }
    virtual ListIterator<ValueType>& operator--() { node = node->prev; return *this; }
    virtual ListIterator<ValueType> operator++(int)
    {
        ListIterator<ValueType> temp{*this};
        operator++();
        return temp;
    }
    virtual ListIterator<ValueType> operator--(int)
    {
        ListIterator<ValueType> temp{*this};
        operator--();
        return temp;
    }
    friend class DLinkedList<ValueType>;
};

template<typename ValueType>
class ConstListIterator : public ConstNodeIterator<DNodeType<ValueType>, ValueType>
{
    using ConstNodeIterator<DNodeType<ValueType>, ValueType>::ConstNodeIterator;
    using ConstNodeIterator<DNodeType<ValueType>, ValueType>::node;
public:
    ConstListIterator<ValueType>& operator=(const ConstNodeIterator<DNodeType<ValueType>, ValueType>& other) = delete;
    virtual ConstListIterator<ValueType>& operator++() { node = node->next; return *this; }
    virtual ConstListIterator<ValueType>& operator--() { node = node->prev; return *this; }
    virtual ConstListIterator<ValueType> operator++(int)
    {
        ConstListIterator<ValueType> temp{*this};
        operator++();
        return temp;
    }
    virtual ConstListIterator<ValueType> operator--(int)
    {
        ConstListIterator<ValueType> temp{*this};
        operator--();
        return temp;
    }
    friend class DLinkedList<ValueType>;
};

template<typename ValueType>
class ReverseListIterator : public NodeIterator<DNodeType<ValueType>, ValueType>
{
    using NodeIterator<DNodeType<ValueType>, ValueType>::NodeIterator;
    using NodeIterator<DNodeType<ValueType>, ValueType>::node;
public:
    ReverseListIterator<ValueType>& operator=(const NodeIterator<DNodeType<ValueType>, ValueType>& other)
    {
        return *this = dynamic_cast<ReverseListIterator<ValueType>&>(NodeIterator<DNodeType<ValueType>, ValueType>::operator=(other));
    }
    virtual ReverseListIterator<ValueType>& operator++() { node = node->prev; return *this; }
    virtual ReverseListIterator<ValueType>& operator--() { node = node->next; return *this; }
    virtual ReverseListIterator<ValueType> operator++(int)
    {
        ReverseListIterator<ValueType> temp{*this};
        operator++();
        return temp;
    }
    virtual ReverseListIterator<ValueType> operator--(int)
    {
        ReverseListIterator<ValueType> temp{*this};
        operator--();
        return temp;
    }
    friend class DLinkedList<ValueType>;
};

template<typename ValueType>
class ConstReverseListIterator : public ConstNodeIterator<DNodeType<ValueType>, ValueType>
{
    using ConstNodeIterator<DNodeType<ValueType>, ValueType>::ConstNodeIterator;
    using ConstNodeIterator<DNodeType<ValueType>, ValueType>::node;
public:
    ConstReverseListIterator<ValueType>& operator=(const ConstNodeIterator<DNodeType<ValueType>, ValueType>& other) = delete;
    virtual ConstReverseListIterator<ValueType>& operator++() { node = node->prev; return *this; }
    virtual ConstReverseListIterator<ValueType>& operator--() { node = node->next; return *this; }
    virtual ConstReverseListIterator<ValueType> operator++(int)
    {
        ConstReverseListIterator<ValueType> temp{*this};
        operator++();
        return temp;
    }
    virtual ConstReverseListIterator<ValueType> operator--(int)
    {
        ConstReverseListIterator<ValueType> temp{*this};
        operator--();
        return temp;
    }
    friend class DLinkedList<ValueType>;
};

template<typename ValueType>
using SNodeType = typename SLinkedList<ValueType>::SNode;

template<typename ValueType>
class ForwardListIterator : public NodeIterator<SNodeType<ValueType>, ValueType>
{
    using NodeIterator<SNodeType<ValueType>, ValueType>::NodeIterator;
    using NodeIterator<SNodeType<ValueType>, ValueType>::node;
public:
    ForwardListIterator<ValueType>& operator=(const NodeIterator<SNodeType<ValueType>, ValueType>& other)
    {
        return *this = dynamic_cast<ForwardListIterator<ValueType>&>(NodeIterator<SNodeType<ValueType>, ValueType>::operator=(other));
    }
    virtual ForwardListIterator<ValueType>& operator++() { node = node->next; return *this; }
    virtual ForwardListIterator<ValueType>& operator--() { throw std::logic_error("Cannot decrement forward iterator."); }
    virtual ForwardListIterator<ValueType> operator++(int)
    { 
        ForwardListIterator<ValueType> temp{*this};
        operator++();
        return temp;
    }
    virtual ForwardListIterator<ValueType> operator--(int) { throw std::logic_error("Cannot decrement forward iterator."); }
    friend class SLinkedList<ValueType>;
};

template<typename ValueType>
class ConstForwardListIterator : public ConstNodeIterator<SNodeType<ValueType>, ValueType>
{
    using ConstNodeIterator<SNodeType<ValueType>, ValueType>::ConstNodeIterator;
    using ConstNodeIterator<SNodeType<ValueType>, ValueType>::node;
public:
    ConstForwardListIterator<ValueType>& operator=(const ConstNodeIterator<SNodeType<ValueType>, ValueType>& other) = delete;
    virtual ConstForwardListIterator<ValueType>& operator++() { node = node->next; return *this; }
    virtual ConstForwardListIterator<ValueType>& operator--() { throw std::logic_error("Cannot decrement forward iterator."); }
    virtual ConstForwardListIterator<ValueType> operator++(int)
    {
        ConstForwardListIterator<ValueType> temp{*this};
        operator++();
        return temp;
    }
    virtual ConstForwardListIterator<ValueType> operator--(int) { throw std::logic_error("Cannot decrement forward iterator."); }
    friend class SLinkedList<ValueType>;
};

template<typename NodeType>
NodeType* succ(NodeType* node)
{
    NodeType* succ_node;
    if (node->right)  // find the leftmost node in node's right subtree
    {
        succ_node = node->right.get();
        while (succ_node->left)
            succ_node = succ_node->left.get();
    }
    else  // find the lowest ancestor of node whose left child is also
    {     // an ancestor of node
        succ_node = node->parent;
        while (succ_node && node == succ_node->right.get())
        {
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
    if (node->left)  // find the rightmost node in node's left subtree
    {
        pred_node = node->left.get();
        while (pred_node->right)
            pred_node = pred_node->right.get();
    }
    else  // find the lowest ancestor of node whose right child is also
    {     // an ancestor of node
        pred_node = node->parent;
        while (pred_node && node == pred_node->left.get())
        {
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
    using NodeIterator<NodeType, std::pair<const KeyType, ValueType>>::NodeIterator;
    using NodeIterator<NodeType, std::pair<const KeyType, ValueType>>::node;
    TreeIterator<NodeType, KeyType, ValueType>& operator=(const NodeIterator<NodeType, std::pair<const KeyType, ValueType>>& other)
    {
        return *this = dynamic_cast<TreeIterator<NodeType, KeyType, ValueType>&>(
               NodeIterator<NodeType, std::pair<const KeyType, ValueType>>::operator=(other));
    }
    virtual TreeIterator<NodeType, KeyType, ValueType>& operator++()
    {
        node = succ<NodeType>(node);
        return *this;
    }
    virtual TreeIterator<NodeType, KeyType, ValueType>& operator--()
    {
        node = pred<NodeType>(node);
        return *this;
    }
    virtual TreeIterator<NodeType, KeyType, ValueType> operator++(int)
    {
        TreeIterator<NodeType, KeyType, ValueType> temp{*this};
        operator++();
        return temp;
    }
    virtual TreeIterator<NodeType, KeyType, ValueType> operator--(int)
    {
        TreeIterator<NodeType, KeyType, ValueType> temp{*this};
        operator--();
        return temp;
    }
    friend class BinarySearchTree<NodeType, KeyType, ValueType>;
    friend class AVLTree<KeyType, ValueType>;
    friend class RedBlackTree<KeyType, ValueType>;
};

template<typename NodeType, typename KeyType, typename ValueType>
class ConstTreeIterator : public ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>
{
public:
    using ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>::ConstNodeIterator;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>::node;
    ConstTreeIterator<NodeType, KeyType, ValueType>& operator=(const ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>& other)
    {
        return *this = dynamic_cast<ConstTreeIterator<NodeType, KeyType, ValueType>&>(
               ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>::operator=(other));
    }
    virtual ConstTreeIterator<NodeType, KeyType, ValueType>& operator++()
    {
        node = succ<NodeType>(node);
        return *this;
    }
    virtual ConstTreeIterator<NodeType, KeyType, ValueType>& operator--()
    {
        node = pred<NodeType>(node);
        return *this;
    }
    virtual ConstTreeIterator<NodeType, KeyType, ValueType> operator++(int)
    {
        ConstTreeIterator<NodeType, KeyType, ValueType> temp{*this};
        operator++();
        return temp;
    }
    virtual ConstTreeIterator<NodeType, KeyType, ValueType> operator--(int)
    {
        ConstTreeIterator<NodeType, KeyType, ValueType> temp{*this};
        operator--();
        return temp;
    }
    friend class BinarySearchTree<NodeType, KeyType, ValueType>;
    friend class AVLTree<KeyType, ValueType>;
    friend class RedBlackTree<KeyType, ValueType>;
};

template<typename NodeType, typename KeyType, typename ValueType>
class ReverseTreeIterator : public NodeIterator<NodeType, std::pair<const KeyType, ValueType>>
{
public:
    using NodeIterator<NodeType, std::pair<const KeyType, ValueType>>::NodeIterator;
    using NodeIterator<NodeType, std::pair<const KeyType, ValueType>>::node;
    ReverseTreeIterator<NodeType, KeyType, ValueType>& operator=(const NodeIterator<NodeType, std::pair<const KeyType, ValueType>>& other)
    {
        return *this = dynamic_cast<ReverseTreeIterator<NodeType, KeyType, ValueType>&>(
               NodeIterator<NodeType, std::pair<const KeyType, ValueType>>::operator=(other));
    }
    virtual ReverseTreeIterator<NodeType, KeyType, ValueType>& operator++()
    {
        node = pred<NodeType>(node);
        return *this;
    }
    virtual ReverseTreeIterator<NodeType, KeyType, ValueType>& operator--()
    {
        node = succ<NodeType>(node);
        return *this;
    }
    virtual ReverseTreeIterator<NodeType, KeyType, ValueType> operator++(int)
    {
        ReverseTreeIterator<NodeType, KeyType, ValueType> temp{*this};
        operator++();
        return temp;
    }
    virtual ReverseTreeIterator<NodeType, KeyType, ValueType> operator--(int)
    {
        ReverseTreeIterator<NodeType, KeyType, ValueType> temp{*this};
        operator--();
        return temp;
    }
    friend class BinarySearchTree<NodeType, KeyType, ValueType>;
    friend class AVLTree<KeyType, ValueType>;
    friend class RedBlackTree<KeyType, ValueType>;
};

template<typename NodeType, typename KeyType, typename ValueType>
class ConstReverseTreeIterator : public ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>
{
public:
    using ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>::ConstNodeIterator;
    using ConstNodeIterator<NodeType, std::pair<const KeyType, ValueType>>::node;
    ConstReverseTreeIterator<NodeType, KeyType, ValueType>& operator=(const NodeIterator<NodeType, std::pair<const KeyType, ValueType>>& other)
    {
        return *this = dynamic_cast<ConstReverseTreeIterator<NodeType, KeyType, ValueType>&>(
               NodeIterator<NodeType, std::pair<const KeyType, ValueType>>::operator=(other));
    }
    virtual ConstReverseTreeIterator<NodeType, KeyType, ValueType>& operator++()
    {
        node = pred<NodeType>(node);
        return *this;
    }
    virtual ConstReverseTreeIterator<NodeType, KeyType, ValueType>& operator--()
    {
        node = succ<NodeType>(node);
        return *this;
    }
    virtual ConstReverseTreeIterator<NodeType, KeyType, ValueType> operator++(int)
    {
        ConstReverseTreeIterator<NodeType, KeyType, ValueType> temp{*this};
        operator++();
        return temp;
    }
    virtual ConstReverseTreeIterator<NodeType, KeyType, ValueType> operator--(int)
    {
        ConstReverseTreeIterator<NodeType, KeyType, ValueType> temp{*this};
        operator--();
        return temp;
    }
    friend class BinarySearchTree<NodeType, KeyType, ValueType>;
    friend class AVLTree<KeyType, ValueType>;
    friend class RedBlackTree<KeyType, ValueType>;
};

}  // end namespace

#endif