#ifndef AVLTREE_HPP
#define AVLTREE_HPP

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>
#include "BinarySearchTree.hpp"

namespace bork_lib
{

template<typename KeyType, typename ValueType>
class AVLTree : public BinarySearchTree<int, KeyType, ValueType>
{
public:
    using TreeNode = typename BinarySearchTree<int, KeyType, ValueType>::TreeNode;
    using BinarySearchTree<int, KeyType, ValueType>::root;
    using BinarySearchTree<int, KeyType, ValueType>::sz;
    using BinarySearchTree<int, KeyType, ValueType>::clone_tree;
    using BinarySearchTree<int, KeyType, ValueType>::left_rotate;
    using BinarySearchTree<int, KeyType, ValueType>::right_rotate;
    using BinarySearchTree<int, KeyType, ValueType>::insert;
    using BinarySearchTree<int, KeyType, ValueType>::tree_minimum;
    using BinarySearchTree<int, KeyType, ValueType>::single_transplant;
    using BinarySearchTree<int, KeyType, ValueType>::double_transplant;
    using BinarySearchTree<int, KeyType, ValueType>::clear;

private:
    int height(TreeNode* node) noexcept { return node ? node->balance_info : -1; } // the AVL height of a node
    void update(TreeNode* node);
    void rebalance_insert(TreeNode* node);
    void rebalance_delete(TreeNode* node);
    bool rebalance(TreeNode* node);
    void delete_node(TreeNode* node);

public:
    // construction, assignment, and destruction
    AVLTree() : BinarySearchTree<int, KeyType, ValueType>::BinarySearchTree() { }
    AVLTree(const AVLTree<KeyType, ValueType>& other)
    : BinarySearchTree<int, KeyType, ValueType>::BinarySearchTree(other) { }   // copy constructor
    AVLTree(AVLTree<KeyType, ValueType>&& other) noexcept
    : BinarySearchTree<int, KeyType, ValueType>::BinarySearchTree(std::forward<AVLTree<KeyType, ValueType>>(other)) { }   // move constructor
    template<typename InputIterator> AVLTree(InputIterator begin, InputIterator end);    // construct from iterator range
    AVLTree(std::initializer_list<std::pair<KeyType, ValueType>> li)
    : AVLTree<KeyType, ValueType>::AVLTree(li.begin(), li.end()) { }  // initializer list constructor
    AVLTree<KeyType, ValueType>& operator=(const AVLTree<KeyType, ValueType>& other) = default;  // copy assignment
    AVLTree<KeyType, ValueType>& operator=(AVLTree<KeyType, ValueType>&& other) noexcept = default;     // move assignment

    inline int tree_height() const noexcept { return root ? root->balance_info : -1; }  // the height of the tree

    friend class TreeIterator<TreeNode, KeyType, ValueType>;
    friend class ConstTreeIterator<TreeNode, KeyType, ValueType>;
    friend class ReverseTreeIterator<TreeNode, KeyType, ValueType>;
    friend class ConstReverseTreeIterator<TreeNode, KeyType, ValueType>;
    friend TreeNode* succ<TreeNode>(TreeNode* node);
    friend TreeNode* pred<TreeNode>(TreeNode* node);
};

/* Allows construction of a tree from an iterator range. The iterators
   must point to pairs. */
template<typename KeyType, typename ValueType>
template<typename InputIterator>
AVLTree<KeyType, ValueType>::AVLTree(InputIterator begin, InputIterator end)
{
    while (begin != end) {
        insert({begin->first, begin->second});
        ++begin;
    }
}

/* Updates the AVL height of a node. */
template<typename KeyType, typename ValueType>
void AVLTree<KeyType, ValueType>::update(TreeNode* node)
{
    node->balance_info = std::max(height(node->left.get()), height(node->right.get())) + 1;
}

template<typename KeyType, typename ValueType>
void AVLTree<KeyType, ValueType>::rebalance_insert(TreeNode* node)
{
    while (node) {
        if (rebalance(node))
            break;
        node = node->parent;
    }
}

template<typename KeyType, typename ValueType>
void AVLTree<KeyType, ValueType>::rebalance_delete(TreeNode* node)
{
    while (node) {
        rebalance(node);
        node = node->parent;
    }
}

/* Rebalances the tree at a node by performing the necessary rotations.
   Returns a bool indicating whether rebalancing occurred. */
template<typename KeyType, typename ValueType>
bool AVLTree<KeyType, ValueType>::rebalance(TreeNode* node)
{
    update(node);
    if (abs(height(node->left.get()) - height(node->right.get())) == 2) {  // node needs rebalancing
        if (height(node->left.get()) == height(node->right.get()) + 2) {
            if (height(node->left->left.get()) == height(node->right.get()) + 1) {  // case 1
                right_rotate(node);
            } else {   // case 2
                left_rotate(node->left.get());
                right_rotate(node);
            }
        } else {
            if (height(node->right->right.get()) == height(node->left.get()) + 1) {   // case 3
                left_rotate(node);
            } else {    // case 4
                right_rotate(node->right.get());
                left_rotate(node);       
            }
        }

        return true;
    }

    return false;
}

/* Deletes a node from the tree. */
template<typename KeyType, typename ValueType>
void AVLTree<KeyType, ValueType>::delete_node(TreeNode* node)
{
    --sz;
    if (node == root.get() && !node->left && !node->right) { // only one node in the tree
        clear();
        return;
    }

    auto node_to_rebalance = node->parent;  // the node where rebalancing will begin
    if (!node->left) {  // case 1: no children or only right child
        single_transplant(node, node->right);
    } else if (!node->right) { // case 2: only left child
        single_transplant(node, node->left);
    } else {    // both children
        auto successor = tree_minimum(node->right.get());
        if (successor->parent != node) {  // case 3: node's successor lies within it's right child's left subtree
            double_transplant(node, successor);
        } else {  // case 4: node's right child is its successor
            auto temp_unique = std::move(node->left);
            single_transplant(node, node->right);
            successor->left = std::move(temp_unique);
            successor->left->parent = successor;
        }

        node_to_rebalance = successor;
    }

    rebalance_delete(node_to_rebalance);
}

}  // end namespace

#endif