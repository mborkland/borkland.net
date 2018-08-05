#ifndef BINARY_SEARCH_TREE_HPP
#define BINARY_SEARCH_TREE_HPP

#include "NodeIterator.hpp"
#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <memory>
#include <iostream>

namespace bork_lib
{

template<typename BalanceType, typename KeyType, typename ValueType>
class BinarySearchTree
{
protected:
    struct TreeNode
    {
        std::pair<const KeyType, ValueType> data;   // key and value
        TreeNode* parent;
        BalanceType balance_info;        // parameter used for balancing
        std::unique_ptr<TreeNode> left = std::unique_ptr<TreeNode>(nullptr);
        std::unique_ptr<TreeNode> right = std::unique_ptr<TreeNode>(nullptr);
        explicit TreeNode(std::pair<const KeyType, ValueType> data, TreeNode* parent = nullptr, BalanceType balance_info = 0)
                : data{std::move(data)}, parent{parent}, balance_info{balance_info} {}
    };

public:
    using iterator = TreeIterator<TreeNode, KeyType, ValueType>;
    using const_iterator = ConstTreeIterator<TreeNode, KeyType, ValueType>;
    using reverse_iterator = ReverseTreeIterator<TreeNode, KeyType, ValueType>;
    using const_reverse_iterator = ConstReverseTreeIterator<TreeNode, KeyType, ValueType>;
    using node_iterator = NodeIterator<typename BinarySearchTree<BalanceType, KeyType, ValueType>::TreeNode, std::pair<const KeyType, ValueType>>;
    using value_type = std::pair<const KeyType, ValueType>;
    using size_type = std::size_t;

protected:
    std::unique_ptr<TreeNode> root = std::unique_ptr<TreeNode>(nullptr);;
    size_type sz;

    auto clone_tree(const std::unique_ptr<TreeNode>& other_node, const std::unique_ptr<TreeNode>& prev_node);

    virtual void update(TreeNode* node) = 0;
    virtual void rebalance_insert(TreeNode* node) = 0;
    virtual void rebalance_delete(TreeNode* node) = 0;

    void left_rotate(TreeNode* node);
    void right_rotate(TreeNode* node);

    auto find_node(const KeyType& key) const;
    auto insert_node(std::pair<TreeNode*, TreeNode*> parent_and_node, std::pair<const KeyType, ValueType>&& keyvalue);
    void create_node(std::unique_ptr<TreeNode>& insertion_point, TreeNode* parent_node, std::pair<const KeyType, ValueType>&& keyvalue);
    auto tree_minimum(TreeNode* node) const;
    auto tree_maximum(TreeNode* node) const;

    virtual void delete_node(TreeNode* node) = 0;
    void single_transplant(TreeNode *old_node, std::unique_ptr<TreeNode> &new_node);
    void double_transplant(TreeNode *old_node, TreeNode *successor_node);

public:
    // construction, assignment, and destruction
    BinarySearchTree() : root(nullptr), sz{0} {}
    BinarySearchTree(const BinarySearchTree<BalanceType, KeyType, ValueType>& other)
     : root{std::move(clone_tree(other.root, nullptr))}, sz{other.sz} { }   // copy constructor
    BinarySearchTree(BinarySearchTree<BalanceType, KeyType, ValueType>&& other) noexcept
     : root{std::move(other.root)}, sz{other.sz} { }  // move constructor
    BinarySearchTree<BalanceType, KeyType, ValueType>& operator=(const BinarySearchTree<BalanceType, KeyType, ValueType>& other);  // copy assignment
    BinarySearchTree<BalanceType, KeyType, ValueType>& operator=(BinarySearchTree<BalanceType, KeyType, ValueType>&& other) noexcept;  // move assignment
    virtual ~BinarySearchTree() { clear(); }  // virtual destructor

    inline bool empty() { return !root; }     // is the tree empty?
    inline size_type size() { return sz; }  // how many nodes are in the tree?

    // insert, find, and delete functions
    auto insert(std::pair<const KeyType, ValueType>&& keyvalue);
    ValueType set_default(const KeyType& key, const ValueType& default_val = {});
    iterator find(const KeyType& key) { return iterator{find_node(key).second}; }  // find a node with the given key
    ValueType& operator[](const KeyType& key);
    const ValueType& operator[](const KeyType& key) const;
    bool erase(const KeyType& key);
    auto& erase(node_iterator& iter);
    void clear() { root.reset(nullptr); sz = 0; }  // clear the tree

    // iterator functions
    iterator begin() noexcept { return iterator{tree_minimum(root.get())}; }
    iterator end() noexcept { return iterator{nullptr}; }
    const_iterator cbegin() const noexcept { return const_iterator{tree_minimum(root.get())}; }
    const_iterator cend()  const noexcept { return const_iterator{nullptr}; }
    reverse_iterator rbegin() noexcept { return reverse_iterator{tree_maximum(root.get())}; }
    reverse_iterator rend() noexcept { return reverse_iterator{nullptr}; }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{tree_maximum(root.get())}; }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator{nullptr}; }

    friend class TreeIterator<TreeNode, KeyType, ValueType>;
    friend class ConstTreeIterator<TreeNode, KeyType, ValueType>;
    friend class ReverseTreeIterator<TreeNode, KeyType, ValueType>;
    friend class ConstReverseTreeIterator<TreeNode, KeyType, ValueType>;
    friend TreeNode* succ<TreeNode>(TreeNode* node);
    friend TreeNode* pred<TreeNode>(TreeNode* node);

    // testing functions
    void inorder_print(TreeNode* node);
    TreeNode* tree_root() { return root.get(); }
};

/* Copy assignment: allows assignment to a tree from an lvalue. */
template<typename BalanceType, typename KeyType, typename ValueType>
BinarySearchTree<BalanceType, KeyType, ValueType>& BinarySearchTree<BalanceType, KeyType, ValueType>::
  operator=(const BinarySearchTree<BalanceType, KeyType, ValueType>& other)
{
    if (this != &other) {
        root = std::move(clone_tree(other.root, nullptr));
        sz = other.sz;
    }

    return *this;
}

/* Move assignment: allows assignment to a tree from an rvalue. */
template<typename BalanceType, typename KeyType, typename ValueType>
BinarySearchTree<BalanceType, KeyType, ValueType>& BinarySearchTree<BalanceType, KeyType, ValueType>::
  operator=(BinarySearchTree<BalanceType, KeyType, ValueType>&& other) noexcept
{
    using std::swap;
    root = std::move(other.root);
    std::swap(sz, other.sz);
    return *this;
}

/* Recursive function that, when called on the root, returns a pointer
   to the root of a new tree. A copy is made using a top-down approach,
   where the prev_node parameter provides a pointer to what will be
   the current node's parent node. */
template<typename BalanceType, typename KeyType, typename ValueType>
auto BinarySearchTree<BalanceType, KeyType, ValueType>::clone_tree(const std::unique_ptr<TreeNode>& other_node,
        const std::unique_ptr<TreeNode>& prev_node)
{
    if (!other_node) {
        return std::unique_ptr<TreeNode>(nullptr);
    }
    
    auto new_node = std::make_unique<TreeNode>(TreeNode{other_node->data, prev_node.get(), other_node->balance_info});
    new_node->left = clone_tree(other_node->left, new_node);
    new_node->right = clone_tree(other_node->right, new_node);
    return new_node;
}

/* Helper function used in rebalancing. Performs a left rotation. */
template<typename BalanceType, typename KeyType, typename ValueType>
void BinarySearchTree<BalanceType, KeyType, ValueType>::left_rotate(TreeNode* node)
{
    auto hold = node->right.get();
    auto temp_unique = std::move(node->right);

    if (node == root.get())  {
        root.swap(temp_unique);
    } else if (node == node->parent->left.get()) {
        node->parent->left.swap(temp_unique);
    } else {
        node->parent->right.swap(temp_unique);
    }
    
    hold->parent = node->parent;
    node->right = std::move(hold->left);
    if (node->right) {
        node->right->parent = node;
    }
    hold->left = std::move(temp_unique);
    node->parent = hold;
    update(node);
    update(node->parent);
}

/* Helper function used in rebalancing. Performs a right rotation. */
template<typename BalanceType, typename KeyType, typename ValueType>
void BinarySearchTree<BalanceType, KeyType, ValueType>::right_rotate(TreeNode* node)
{
    auto hold = (node->left).get();
    auto temp_unique = std::move(node->left);

    if (node == root.get()) {
        root.swap(temp_unique);
    } else if (node == (node->parent->left).get()) {
        node->parent->left.swap(temp_unique);
    } else {
        node->parent->right.swap(temp_unique);
    }
    
    hold->parent = node->parent;
    node->left = std::move(hold->right);
    if (node->left) {
        node->left->parent = node;
    }
    hold->right = std::move(temp_unique);
    node->parent = hold;
    update(node);
    update(node->parent);
}

/* Inserts a node in the tree with the given key-value pair. */
template<typename BalanceType, typename KeyType, typename ValueType>
auto BinarySearchTree<BalanceType, KeyType, ValueType>::insert(std::pair<const KeyType, ValueType>&& keyvalue)
{
    if (empty()) {
        ++sz;
        root = std::make_unique<TreeNode>(TreeNode{std::forward<std::pair<const KeyType, ValueType>>(keyvalue)});
        iterator iter {};
        iter.node = root.get();
        return std::pair<iterator, bool>{iter, true};
    }

    return insert_node(find_node(keyvalue.first), std::move(keyvalue));
}

template<typename BalanceType, typename KeyType, typename ValueType>
auto BinarySearchTree<BalanceType, KeyType, ValueType>::insert_node(std::pair<TreeNode*, TreeNode*> parent_and_node,
        std::pair<const KeyType, ValueType>&& keyvalue)
{
    iterator iter {};
    auto parent = parent_and_node.first;
    auto node = parent_and_node.second;
    TreeNode* new_node {};
    if (node) {
        iter.node = node;
        return std::pair<iterator, bool>{iter, false};
    } else if (keyvalue.first > (parent->data).first) {
        create_node(parent->right, parent, std::move(keyvalue));
        new_node = parent->right.get();
    } else {
        create_node(parent->left, parent, std::move(keyvalue));
        new_node = parent->left.get();
    }

    ++sz;
    rebalance_insert(parent);
    iter.node = new_node;
    return std::pair<iterator, bool>{iter, true};
}

template<typename BalanceType, typename KeyType, typename ValueType>
void BinarySearchTree<BalanceType, KeyType, ValueType>::create_node(std::unique_ptr<TreeNode>& insertion_point,
        TreeNode* parent_node, std::pair<const KeyType, ValueType>&& keyvalue)
{
    insertion_point = std::make_unique<TreeNode>(
            TreeNode{std::forward<std::pair<const KeyType, ValueType>>(keyvalue), parent_node});
}

template<typename BalanceType, typename KeyType, typename ValueType>
ValueType BinarySearchTree<BalanceType, KeyType, ValueType>::set_default(const KeyType& key, const ValueType& default_val)
{
    auto parent_and_node = find_node(key);
    auto node = parent_and_node.second;
    if (node) {
        return (node->data).second;
    } else {
        insert_node(parent_and_node, {key, default_val});
        return default_val;
    }
}

/* Helper function returns a pointer to the node with the given key, if it exists. */
template<typename BalanceType, typename KeyType, typename ValueType>
auto BinarySearchTree<BalanceType, KeyType, ValueType>::find_node(const KeyType& key) const
{
    auto node = root.get();
    TreeNode* node_parent = nullptr;
    while (node) {
        if (key == (node->data).first) {
            break;
        }

        node_parent = node;
        node = (key < (node->data).first) ? (node->left).get() : (node->right).get();
    }

    return std::pair<TreeNode*, TreeNode*>{node_parent, node};
}

/* Allows retrieval of a value using subscripting. If the key does not
   exist, then it is added to the tree as the default value for ValueType.
   The return value is an non-const lvalue reference, so the value can
   be changed via assignment. */
template<typename BalanceType, typename KeyType, typename ValueType>
ValueType& BinarySearchTree<BalanceType, KeyType, ValueType>::operator[](const KeyType& key)
{
    auto parent_and_node = find_node(key);
    auto node = parent_and_node.second;
    if (node) {
        return (node->data).second;
    } else {
        return (*(insert_node(parent_and_node, {key, ValueType{}}).first)).second;
    }
}

/* Same as above, but returns a const lvalue reference. */
template<typename BalanceType, typename KeyType, typename ValueType>
const ValueType& BinarySearchTree<BalanceType, KeyType, ValueType>::operator[](const KeyType& key) const
{
    auto node = find_node(key).second;
    if (node) {
        return (node->data).second;
    } else {
        throw std::out_of_range("Key does not exist.");
    }
}

/* Erases a node with a given key, if it exists. Does nothing if the
   key does not exist. Returns a boolean indicating whether an entry
   was erased. */
template<typename BalanceType, typename KeyType, typename ValueType>
bool BinarySearchTree<BalanceType, KeyType, ValueType>::erase(const KeyType& key)
{
    if (empty()) {
        throw std::out_of_range("Can't erase from empty tree.");
    }

    auto node = find_node(key).second;
    if (node) {
        delete_node(node);
        return true;
    }

    return false;
}

/* Erases a node using a NodeIterator. Returns an iterator pointing to the
   successor of the deleted node. */
template<typename BalanceType, typename KeyType, typename ValueType>
auto& BinarySearchTree<BalanceType, KeyType, ValueType>::erase(node_iterator& iter)
{
    if (empty()) {
        throw std::out_of_range("Can't erase from empty tree.");
    }
    
    auto node = iter.node;
    if (!node) {
        throw std::invalid_argument("Can't delete null pointer.");
    }

    ++iter;
    delete_node(node);
    return iter;
}

/* Helper function used by the delete_node function. Deletes old_node by changing
   ownership of its parent, which now owns new_node. */
template<typename BalanceType, typename KeyType, typename ValueType>
void BinarySearchTree<BalanceType, KeyType, ValueType>::single_transplant(TreeNode* old_node,
        std::unique_ptr<TreeNode> &new_node)
{
    auto parent = old_node->parent;
    TreeNode* owner;
    if (old_node == root.get()) {
        root = std::move(new_node);
        owner = root.get();
    } else if (old_node == parent->left.get()) {
        parent->left = std::move(new_node);
        owner = parent->left.get();
    } else {
        parent->right = std::move(new_node);
        owner = parent->right.get();
    }

    if (owner) {
        owner->parent = parent;
    }
}

template<typename BalanceType, typename KeyType, typename ValueType>
void BinarySearchTree<BalanceType, KeyType, ValueType>::double_transplant(TreeNode* old_node,
        TreeNode* successor_node)
{
    auto successor_child = std::move(successor_node->right);
    auto successor_parent = successor_node->parent;
    auto node_left_child = std::move(old_node->left);
    auto node_right_child = std::move(old_node->right);
    single_transplant(old_node, successor_parent->left);
    successor_node->left = std::move(node_left_child);
    successor_node->right = std::move(node_right_child);
    successor_node->left->parent = successor_node->right->parent = successor_node;
    successor_parent->left = std::move(successor_child);
    if (successor_parent->left) {
        successor_parent->left->parent = successor_parent;
    }
}

/* Returns a pointer to the node with lowest key in the subtree
   rooted at node. */
template<typename BalanceType, typename KeyType, typename ValueType>
auto BinarySearchTree<BalanceType, KeyType, ValueType>::tree_minimum(TreeNode* node) const
{
    while (node->left) {
        node = (node->left).get();
    }

    return node;
}

/* Returns a pointer to the node with the highest key in the subtree
   rooted at node. */
template<typename BalanceType, typename KeyType, typename ValueType>
auto BinarySearchTree<BalanceType, KeyType, ValueType>::tree_maximum(TreeNode* node) const
{
    while (node->right) {
        node = (node->right).get();
    }

    return node;
}


// testing function
template<typename BalanceType, typename KeyType, typename ValueType>
void BinarySearchTree<BalanceType, KeyType, ValueType>::inorder_print(TreeNode* node)
{
    if (!node) {
        return;
    }
    
    inorder_print(node->left.get());
    std::cout << node->data.first << '/' << node->balance_info << ' ';
    inorder_print(node->right.get());
}

}  // end namespace

#endif