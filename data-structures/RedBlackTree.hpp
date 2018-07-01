#ifndef REDBLACKTREE_HPP
#define REDBLACKTREE_HPP

#include "BinarySearchTree.hpp"
#include <utility>

namespace bork_lib
{

enum class Color
{
    red, black
};

template<typename KeyType, typename ValueType>
class RedBlackTree : public BinarySearchTree<Color, KeyType, ValueType>
{
public:
    using TreeNode = typename BinarySearchTree<int, KeyType, ValueType>::TreeNode;
    using node_iterator = NodeIterator<TreeNode, std::pair<const KeyType, ValueType>>;
    using BinarySearchTree<Color, KeyType, ValueType>::root;
    using BinarySearchTree<Color, KeyType, ValueType>::sz;
    using BinarySearchTree<Color, KeyType, ValueType>::clone_tree;
    using BinarySearchTree<Color, KeyType, ValueType>::left_rotate;
    using BinarySearchTree<Color, KeyType, ValueType>::right_rotate;
    using BinarySearchTree<Color, KeyType, ValueType>::insert;
    using BinarySearchTree<Color, KeyType, ValueType>::tree_minimum;
    using BinarySearchTree<Color, KeyType, ValueType>::transplant;
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

    std::pair<node_iterator&, bool> insert(const std::pair<const KeyType, ValueType>& keyvalue);

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
RedBlackTree<KeyType, ValueType>::RedBlackTree(InputIterator begin, InputIterator end) : RedBlackTree()
{
    while (begin != end)
    {
        BinarySearchTree<Color, KeyType, ValueType>::insert({begin->first, begin->second});
        ++begin;
    }
}

/* Copy assignment: allows assignment to a tree from an lvalue. */
template<typename KeyType, typename ValueType>
RedBlackTree<KeyType, ValueType>& RedBlackTree<KeyType, ValueType>::operator=(const RedBlackTree<KeyType, ValueType> &other)
{
    if (this != &other)
    {
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
    while (node->parent->balance_info == red)
    {
        auto grandparent = node->parent->parent;  // if node's parent is red, it must have a grandparent
        if (node->parent == grandparent->left) // node's parent is the left child of its parent
        {
            auto uncle = grandparent->right;
            if (uncle->balance_info == red)   // case 1: change both red siblings to black
            {                                 // and grandparent to red
                node->parent->balance_info = uncle->balance_info = black;
                grandparent->balance_info = red;
                node = grandparent;
            }
            else
            {
                if (node = node->parent->right)  // case 2: node's uncle is black and node is the right child of its parent
                {
                    node = node->parent;
                    BinarySearchTree<Color, KeyType, ValueType>::left_rotate(node);
                }

                grandparent = node->parent->parent;
                node->parent->balance_info = black;   // case 3: node's uncle is black and node is the left child of its parent
                grandparent->balance_info = red;
                BinarySearchTree<Color, KeyType, ValueType>::left_rotate(grandparent);
            }
        }
        else                                  // node's parent is the right child of its parent
        {
            auto uncle = grandparent->left;
            if (uncle->balance_info == red)   // case 1: change both red siblings to black
            {                                 // and grandparent to red
                node->parent->balance_info = uncle->balance_info = black;
                grandparent->balance_info = red;
                node = grandparent;
            }
            else
            {
                if (node = node->parent->left)  // case 2: node's uncle is black and node is the left child of its parent
                {
                    node = node->parent;
                    BinarySearchTree<Color, KeyType, ValueType>::right_rotate(node);
                }

                grandparent = node->parent->parent;
                node->parent->balance_info = black;   // case 3: node's uncle is black and node is the right child of its parent
                grandparent->balance_info = red;
                BinarySearchTree<Color, KeyType, ValueType>::right_rotate(grandparent);
            }
        }
    }
}

template<typename KeyType, typename ValueType>
void RedBlackTree<KeyType, ValueType>::rebalance_delete(TreeNode* node)
{
    
}

/* Starts at a node and rebalances the tree at that node by performing
   the necessary rotations. Repeats the process by traversing parent
   links all the way up to the root. */
template<typename KeyType, typename ValueType>
void RedBlackTree<KeyType, ValueType>::rebalance(TreeNode* node)
{

}

/* Inserts a node in the tree with the given key-value pair. Redefinition of base class function.
   The default parameter for color in RBNode is red, since a new node is always red, except for
   when the first node of the tree is created. The root also needs to be updated to black after
   rebalancing, so this addition works in that case, as well. */
template<typename NodeType, typename KeyType, typename ValueType>
std::pair<NodeIterator<NodeType, std::pair<const KeyType, ValueType>>&, bool> BinarySearchTree<NodeType, KeyType, ValueType>::
  insert(const std::pair<const KeyType, ValueType>& keyvalue)
{
    auto ret_val = BinarySearchTree<Color, KeyType, ValueType>::insert(keyvalue);
    root->balance_info = black;
    return ret_val;
}

/* Deletes a node from the tree. */
template<typename KeyType, typename ValueType>
void RedBlackTree<KeyType, ValueType>::delete_node(TreeNode* node)
{
    // TODO: FIX THIS!!!!
    --sz;
    if (node == root && !node->left && !node->right)  // only one node in the tree
    {
        delete node;
        root = nullptr;
        return;
    }

    auto node_to_rebalance = node;  // node where rebalancing will begin
    auto node_to_transplant = node;
    Color original_color = node->balance_info;
    if (!node->left)   // case 1: no children or only right child
    {
        BinarySearchTree<Color, KeyType, ValueType>::transplant(node, node->right);
        node_to_rebalance = node->right ? node->right : node->parent;
    }
    else if (!node->right)  // case 2: only left child
    {
        BinarySearchTree<Color, KeyType, ValueType>::transplant(node, node->left);
        node_to_rebalance = node->left;
    }
    else     // both children
    {
        node_to_transplant = tree_minimum(node->right);
        original_color = node_to_transplant->balance_info;
        node_to_rebalance = node_to_transplant->right;
        if (node_to_transplant->parent == node)  // case 4: node's right child is its successor
        {
            node_to_rebalance->parent = node_to_transplant;
        }

            BinarySearchTree<Color, KeyType, ValueType>::transplant(successor, successor->right);
            BinarySearchTree<Color, KeyType, ValueType>::transplant(node, successor);
            successor->right = node->right;
            successor->left = node->left;
            successor->right->parent = successor;
            successor->left->parent = successor;
            node_to_rebalance = tree_minimum(successor->right);
        }
        else   // case 3: node's successor lies within it's right child's left subtree
        {
            BinarySearchTree<Color, KeyType, ValueType>::transplant(node, successor);
            successor->left = node->left;
            successor->left->parent = successor;
            node_to_rebalance = successor;
        }
    }
    
    delete node;
    rebalance_delete(node_to_rebalance);
}

}    // end namespace

#endif
