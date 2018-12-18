#ifndef UNIONFIND_HPP
#define UNIONFIND_HPP

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace bork_lib
{

template<typename T>
class UnionFind
{
private:
    struct UFNode
    {
        T key;
        int rank;
        UFNode* next = nullptr;

        explicit UFNode(T key, int rank = 0) : key{std::move(key)}, rank{rank} {}
    };

    std::unordered_map<T, UFNode*> nodes;
    UFNode* find_root(const T& key) const noexcept;

public:
    UnionFind() = default;
    UnionFind(const UnionFind<T>& other);
    UnionFind(UnionFind<T>&& other) noexcept : nodes{std::move(other.nodes)} {}
    UnionFind<T>& operator=(const UnionFind<T>& other);
    UnionFind<T>& operator=(UnionFind<T>&& other) noexcept;
    ~UnionFind() { clear(); }
    void make_set(const T& key);
    void join(const T& key1, const T& key2) noexcept;
    bool same_set(const T& key1, const T& key2) const noexcept;
    void clear();
};

/* Private function to find the root of a node with the given key. */
template<typename T>
typename UnionFind<T>::UFNode* UnionFind<T>::find_root(const T& key) const noexcept
{
    UFNode* node;
    try {
        node = nodes.at(key);
    } catch(const std::out_of_range& e) {
        throw std::out_of_range("Invalid key.");
    }

    auto root = node;
    for (; root->next; root = root->next);   // find the root
    auto temp = node->next;
    while (temp && temp != root) {   // path compression
        node->next = root;
        node = temp;
        temp = temp->next;
    }

    return root;
}

/* Copy constructor. */
template<typename T>
UnionFind<T>::UnionFind(const UnionFind<T>& other)
{
    std::unordered_map<T, T> pointer_map;
    for (const auto& x : other.nodes) {
        nodes[x.first] = new UFNode(x.second->key, x.second->rank);
        if (x.second->next) {
            pointer_map.insert({x.first, x.second->next->key});
        }
    }

    for (const auto& x : pointer_map) {
        nodes[x.first]->next = nodes[x.second];
    }
}

/* Copy assignment. Uses copy-and-swap idiom. */
template<typename T>
UnionFind<T>& UnionFind<T>::operator=(const UnionFind<T>& other)
{
    if (this != &other) {
        auto temp = other;
        using std::swap;
        swap(nodes, temp.nodes);
    }

    return *this;
}

/* Move assignment. */
template<typename T>
UnionFind<T>& UnionFind<T>::operator=(UnionFind<T>&& other) noexcept
{
    if (this != &other) {
        using std::swap;
        swap(nodes, other.nodes);
    }

    return *this;
}

/* Adds a new set to the structure. */
template<typename T>
void UnionFind<T>::make_set(const T& key)
{
    nodes.insert({key, new UFNode{key}});
}

/* Joins the set containing key1 with the set containing key2. */
template<typename T>
void UnionFind<T>::join(const T& key1, const T& key2) noexcept
{
    auto root1 = find_root(key1);
    auto root2 = find_root(key2);

    if (root1->rank > root2->rank) {
        root2->next = root1;
    } else {
        root1->next = root2;
        if (root1->rank == root2->rank) {
            ++root2->rank;
        }
    }
}

/* Returns whether or not two keys are in the same set. */
template<typename T>
bool UnionFind<T>::same_set(const T& key1, const T& key2) const noexcept
{
    return find_root(key1) == find_root(key2);
}

/* Clears the structure. */
template<typename T>
void UnionFind<T>::clear()
{
    for (const auto& x : nodes) {
        delete x.second;
    }

    nodes.clear();
}

}  // end namespace

#endif