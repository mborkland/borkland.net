#ifndef TREEITERATOR_HPP
#define TREEITERATOR_HPP

#include <iterator>
#include <utility>

namespace bork_lib
{

template<typename B, typename K, typename V> class BinarySearchTree;
template<typename K, typename V> class AVLTree;
template<typename K, typename V> class RedBlackTree;
template<typename N, typename B, typename K, typename V> class ConstTreeIterator;
template<typename N, typename B, typename K, typename V> class ReverseTreeIterator;
template<typename N, typename B, typename K, typename V> class ConstReverseTreeIterator;

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

template<typename NodeType, typename BalanceType, typename KeyType, typename ValueType>
class TreeIterator
{
private:

    using tree_iterator = TreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using const_tree_iterator = ConstTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using reverse_tree_iterator = ReverseTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using const_reverse_tree_iterator = ConstReverseTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
public:
    NodeType* node;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair<const KeyType, ValueType>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit TreeIterator(NodeType* node = nullptr) : node{node} { }
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

    friend class BinarySearchTree<BalanceType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

template<typename NodeType, typename BalanceType, typename KeyType, typename ValueType>
class ConstTreeIterator
{
private:
    NodeType* node;
    using tree_iterator = TreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using const_tree_iterator = ConstTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using reverse_tree_iterator = ReverseTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using const_reverse_tree_iterator = ConstReverseTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair<const KeyType, ValueType>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ConstTreeIterator(NodeType* node = nullptr) : node{node} { }
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

    friend class BinarySearchTree<BalanceType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

template<typename NodeType, typename BalanceType, typename KeyType, typename ValueType>
class ReverseTreeIterator
{
private:
    NodeType* node;
    using tree_iterator = TreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using const_tree_iterator = ConstTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using reverse_tree_iterator = ReverseTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using const_reverse_tree_iterator = ConstReverseTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair<const KeyType, ValueType>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ReverseTreeIterator(NodeType* node = nullptr) : node{node} { }
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

    friend class BinarySearchTree<BalanceType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

template<typename NodeType, typename BalanceType, typename KeyType, typename ValueType>
class ConstReverseTreeIterator
{
private:
    NodeType* node;
    using tree_iterator = TreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using const_tree_iterator = ConstTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using reverse_tree_iterator = ReverseTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
    using const_reverse_tree_iterator = ConstReverseTreeIterator<NodeType, BalanceType, KeyType, ValueType>;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair<const KeyType, ValueType>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    explicit ConstReverseTreeIterator(NodeType* node = nullptr) : node{node} { }
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

    friend class BinarySearchTree<BalanceType, KeyType, value_type>;
    friend class AVLTree<KeyType, value_type>;
    friend class RedBlackTree<KeyType, value_type>;
};

} // end namespace

#endif
