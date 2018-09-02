#ifndef REDBLACKTREE_HPP
#define REDBLACKTREE_HPP

#include <iostream>
#include <utility>
#include "BinarySearchTree.hpp"

namespace bork_lib
{

enum class Color
{
    red, black
};

std::ostream& operator<<(std::ostream& os, const Color& color) {
    if (color == Color::red) {
        os << "R";
    } else {
        os << "NonCopyable";
    }

    return os;
}

template<typename KeyType, typename ValueType>
class RedBlackTree : public BinarySearchTree<Color, KeyType, ValueType>
{
public:
    using TreeNode = typename BinarySearchTree<Color, KeyType, ValueType>::TreeNode;
    using node_iterator = NodeIterator<TreeNode, std::pair<const KeyType, ValueType>>;
    using BinarySearchTree<Color, KeyType, ValueType>::root;
    using BinarySearchTree<Color, KeyType, ValueType>::sz;
    using BinarySearchTree<Color, KeyType, ValueType>::clone_tree;
    using BinarySearchTree<Color, KeyType, ValueType>::left_rotate;
    using BinarySearchTree<Color, KeyType, ValueType>::right_rotate;
    using BinarySearchTree<Color, KeyType, ValueType>::tree_minimum;
    using BinarySearchTree<Color, KeyType, ValueType>::single_transplant;
    using BinarySearchTree<Color, KeyType, ValueType>::double_transplant;
    using BinarySearchTree<Color, KeyType, ValueType>::clear;
    using BinarySearchTree<Color, KeyType, ValueType>::inorder_print;  // testing function

private:
    void update(TreeNode* node) { }  // this function not needed in red-black tree
    void rebalance_insert(TreeNode* node);
    void rebalance_delete(TreeNode* node);
    bool rebalance(TreeNode* node);
    void delete_node(TreeNode* node);

public:
    // construction, assignment, and destruction
    RedBlackTree() : BinarySearchTree<Color, KeyType, ValueType>::BinarySearchTree() { }
    RedBlackTree(const RedBlackTree<KeyType, ValueType>& other)
     : BinarySearchTree<Color, KeyType, ValueType>::BinarySearchTree(other) { }   // copy constructor
    RedBlackTree(RedBlackTree<KeyType, ValueType>&& other) noexcept
     : BinarySearchTree<Color, KeyType, ValueType>::
       BinarySearchTree(std::forward<RedBlackTree<KeyType, ValueType>>(other)) { }   // move constructor
    template<typename InputIterator> RedBlackTree(InputIterator begin, InputIterator end);  // construct from iterator range
    RedBlackTree(std::initializer_list<std::pair<KeyType, ValueType>> li) : RedBlackTree(li.begin(), li.end()) { }  // initializer list constructor
    RedBlackTree<KeyType, ValueType>& operator=(const RedBlackTree<KeyType, ValueType>& other);  // copy assignment
    RedBlackTree<KeyType, ValueType>& operator=(RedBlackTree<KeyType, ValueType>&& other) noexcept;     // move assignment

    auto insert(std::pair<const KeyType, ValueType>&& keyvalue);

    friend class TreeIterator<Color, KeyType, ValueType>;
    friend class ConstTreeIterator<Color, KeyType, ValueType>;
    friend class ReverseTreeIterator<Color, KeyType, ValueType>;
    friend class ConstReverseTreeIterator<Color, KeyType, ValueType>;
    friend TreeNode* succ<TreeNode>(TreeNode* node);
    friend TreeNode* pred<TreeNode>(TreeNode* node);
};

/* Allows construction of a tree from an iterator range. The iterators
   must point to pairs. */
template<typename KeyType, typename ValueType>
template<typename InputIterator>
RedBlackTree<KeyType, ValueType>::RedBlackTree(InputIterator begin, InputIterator end)
{
    while (begin != end) {
        insert({begin->first, begin->second});
        ++begin;
    }
}

/* Copy assignment: allows assignment to a tree from an lvalue. */
template<typename KeyType, typename ValueType>
RedBlackTree<KeyType, ValueType>& RedBlackTree<KeyType, ValueType>::operator=(const RedBlackTree<KeyType, ValueType> &other)
{
    if (this != &other) {
        clear();
        root = clone_tree(other.root, nullptr);
        sz = other.sz;
    }

    return *this;
}

/* Move assignent: allows assignment to a tree from an rvalue. */
template<typename KeyType, typename ValueType>
RedBlackTree<KeyType, ValueType>& RedBlackTree<KeyType, ValueType>::operator=(RedBlackTree<KeyType, ValueType>&& other) noexcept
{
    return *this = dynamic_cast<RedBlackTree<KeyType, ValueType>&>(
        BinarySearchTree<Color, KeyType, ValueType>::
        operator=(std::forward<RedBlackTree<KeyType, ValueType>>(other)));
}

/* Inserting a node may cause a violation of the red-black properties. This
   function restores the tree to a red-black tree by making the necessary
   rotations and the changes to node color. (Algorithm adapted from pseudocode
   in CLRS 3rd edition.) */
template<typename KeyType, typename ValueType>
void RedBlackTree<KeyType, ValueType>::rebalance_insert(TreeNode* node)
{
    while (node->parent->balance_info == Color::red) {
        auto grandparent = node->parent->parent;  // if node's parent is red, it must have a grandparent
        if (node->parent == grandparent->left.get()) { // node's parent is the left child of its parent
            auto uncle = grandparent->right.get();
            if (uncle->balance_info == Color::red) {   // case 1: change both red siblings to black and grandparent to red
                node->parent->balance_info = uncle->balance_info = Color::black;
                grandparent->balance_info = Color::red;
                node = grandparent;
            } else {
                if (node == node->parent->right.get()) {  // case 2: node's uncle is black and node is the right child of its parent
                    node = node->parent;
                    left_rotate(node);
                }

                grandparent = node->parent->parent;
                node->parent->balance_info = Color::black;   // case 3: node's uncle is black and node is the left child of its parent
                grandparent->balance_info = Color::red;
                right_rotate(grandparent);
            }
        } else {                                 // node's parent is the right child of its parent
            auto uncle = grandparent->left.get();
            if (uncle->balance_info == Color::red) {   // case 1: change both red siblings to black and grandparent to red
                node->parent->balance_info = uncle->balance_info = Color::black;
                grandparent->balance_info = Color::red;
                node = grandparent;
            } else {
                if (node == node->parent->left.get()) {  // case 2: node's uncle is black and node is the left child of its parent
                    node = node->parent;
                    right_rotate(node);
                }

                grandparent = node->parent->parent;
                node->parent->balance_info = Color::black;   // case 3: node's uncle is black and node is the right child of its parent
                grandparent->balance_info = Color::red;
                left_rotate(grandparent);
            }
        }
    }
}

template<typename KeyType, typename ValueType>
void RedBlackTree<KeyType, ValueType>::rebalance_delete(TreeNode* node)
{
    // TODO: fix this!!!
    while (node != root.get() && node->balance_info == Color::black) {
        if (node == node->parent->left.get()) {
            auto sibling = node->parent->right.get();
            if (sibling->balance_info == Color::red) {  // case 1: node's sibling is red
                sibling->balance_info == Color::black;
                sibling->parent->balance_info = Color::red;
                left_rotate(node->parent);
                sibling = node->parent->right.get();
            }

            if (sibling->left->balance_info == Color::black && sibling->right->balance_info == Color::black) {
                sibling->balance_info = Color::red;   // case 2: node's sibling is black, and both of the sibling's
                node = node->parent;                  //         children are black
            } else {         // node's sibling is black
                if (sibling->right->balance_info == Color::black) {  // case 3: the sibling's left and right children
                    sibling->left->balance_info = Color::black;      //         are red and black, respectively
                    sibling->balance_info = Color::red;
                    right_rotate(sibling);
                    sibling = node->parent->right.get();
                }

                sibling->balance_info = node->parent->balance_info;
                node->parent->balance_info = Color::black;
                sibling->right->balance_info = Color::black;
                left_rotate(node->parent);
                node = root.get();
            }
        } else {

        }

        node->balance_info = Color::black;
    }
}

/* Inserts a node in the tree with the given key-value pair. Redefinition of base class function.
   The default parameter for color in RBNode is red, since a new node is always red, except for
   when the first node of the tree is created. The root also needs to be updated to black after
   rebalancing, so this addition works in that case, as well. */
template<typename KeyType, typename ValueType>
auto RedBlackTree<KeyType, ValueType>::insert(std::pair<const KeyType, ValueType>&& keyvalue)
{
    auto ret_val = BinarySearchTree<Color, KeyType, ValueType>::insert(
            std::forward<std::pair<const KeyType, ValueType>>(keyvalue));
    root->balance_info = Color::black;
    return ret_val;
}

/* Deletes a node from the tree. */
template<typename KeyType, typename ValueType>
void RedBlackTree<KeyType, ValueType>::delete_node(TreeNode* node)
{
    --sz;
    if (node == root && !node->left && !node->right) {  // only one node in the tree
        clear();
        return;
    }

    TreeNode* node_to_rebalance;  // node where rebalancing will begin
    auto original_color = node->balance_info;
    if (!node->left) {  // case 1: no children or only right child
        node_to_rebalance = node->right.get();
        single_transplant(node, node->right);
    } else if (!node->right) {  // case 2: only left child
        node_to_rebalance = node->left.get();
        single_transplant(node, node->left);
    } else {                  // both children
        auto successor = tree_minimum(node->right.get());
        original_color = successor->balance_info;
        node_to_rebalance = successor->right ? successor->right.get() : successor->parent;

        auto node_color = node->balance_info;
        if (successor->parent != node) {  // case 3: node's successor lies within it's right child's left subtree
            double_transplant(node, successor);
        } else {  // case 4: node's right child is its successor
            auto temp_unique = std::move(node->left);
            single_transplant(node, node->right);
            successor->left = std::move(temp_unique);
            successor->left->parent = successor;
        }

        successor->balance_info = node_color;
    }

    if (original_color == Color::black) {
        rebalance_delete(node_to_rebalance);
    }

}

}    // end namespace

#endif
