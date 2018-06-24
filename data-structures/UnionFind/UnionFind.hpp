#ifndef UNIONFIND_HPP
#define UNIONFIND_HPP

#include <unordered_map>
#include <iostream>
#include <stdexcept>

template<typename T> class UnionFind;  // forward declaration

template<typename T>
struct UFNode
{
private:
    int rank;
    T data;
    UFNode<T> *next;

public:
    UFNode(T data) : rank(0), data(data), next(nullptr) {}
    friend class UnionFind<T>;
};

template<typename T>
class UnionFind
{
private:
    std::unordered_map<T, UFNode<T>*> nodes;

public:
    UnionFind() {}
    ~UnionFind();
    UFNode<T>* find_root(T key);
    void make_set(T data);
    void join(T key1, T key2);
    bool same_set(T key1, T key2);
    void print();
};

template<typename T>
UFNode<T>* UnionFind<T>::find_root(T key)
{
    UFNode<T> *node;
    try
    {
        node = nodes.at(key);
    }
    catch(std::out_of_range &oor)
    {
        throw std::out_of_range("Invalid key.");
    }

    auto root = node;
    for (; root->next; root = root->next);   // find the root
    auto temp = node->next;
    while (temp && temp != root)   // path compression
    {
        node->next = root;
        node = temp;
        temp = temp->next;
    }

    return root;
}

template<typename T>
void UnionFind<T>::make_set(T data)
{
    UFNode<T> *new_set = new UFNode<T>(data);
    nodes.insert({data, new_set});
}

template<typename T>
void UnionFind<T>::join(T key1, T key2)
{
    auto x = find_root(key1);
    auto y = find_root(key2);

    if (x->rank > y->rank)
    {
        y->next = x;
    }
    else
    {
        x->next = y;
        if (x->rank == y->rank)
        {
            ++y->rank;
        }
    }
}

template<typename T>
bool UnionFind<T>::same_set(T key1, T key2)
{
    return find_root(key1) == find_root(key2);
}

template<typename T>
UnionFind<T>::~UnionFind()
{
    for (auto &x : nodes)
        delete x.second;
}

template<typename T>
void UnionFind<T>::print()
{
    for (const auto &x : nodes)
    {
        auto temp = x.second;
        std::cout << (x.second)->data;
        while (temp->next)
        {
            std::cout << " -> " << temp->next->data;
            temp = temp->next;
        }
        std::cout << std::endl;
    }
}

#endif