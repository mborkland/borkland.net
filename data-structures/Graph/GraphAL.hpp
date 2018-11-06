#ifndef GRAPH_AL_HPP
#define GRAPH_AL_HPP

#include <iostream>
#include <type_traits>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <limits>
#include "Graph.hpp"

template<typename V = size_t, typename W = int>
class GraphAL : public Graph<std::vector<std::unordered_map<size_t, W>>, V, W>
{
private:
    using AdjListType = std::vector<std::unordered_map<size_t,W>>;
    using Graph<AdjListType, V, W>::vertices;
    using Graph<AdjListType, V, W>::adj_structure;
    using Graph<AdjListType, V, W>::current_key;
    using Graph<AdjListType, V, W>::highest_key;
    using Graph<AdjListType, V, W>::is_weighted;
    using Graph<AdjListType, V, W>::is_directed;
    using Graph<AdjListType, V, W>::is_labeled;
    using Graph<AdjListType, V, W>::data_is_key;
    GraphAL(bool is_weighted, bool is_directed, bool is_labeled, bool data_is_key)
            : Graph<AdjListType, V, W>::Graph(is_weighted, is_directed, is_labeled, data_is_key) {}

public:
    void add_vertex(const std::initializer_list<std::pair<const size_t, W>> &outgoing_edges,
                    const std::initializer_list<std::pair<const size_t, W>> &incoming_edges,
                    const std::string &label = {}, const V &data = {});
    inline void add_vertex() { add_vertex({}, {}, {}, {}); }
    inline void add_vertex(const std::initializer_list<std::pair<const size_t, W>> &outgoing_edges, const V &data = {})
                          { add_vertex(outgoing_edges, {}, {}, data); }
    inline void add_vertex(const std::initializer_list<std::pair<const size_t, W>> &outgoing_edges,
                           const std::initializer_list<std::pair<const size_t, W>> &incoming_edges, const V &data = {})
                          { add_vertex(outgoing_edges, incoming_edges, {}, data); }
    inline void add_vertex(const std::initializer_list<std::pair<const size_t, W>> &outgoing_edges, const std::string &label)
                          { add_vertex(outgoing_edges, {}, label, {}); }
    inline void add_vertex(const std::initializer_list<std::pair<const size_t, W>> &outgoing_edges,
                           const std::initializer_list<std::pair<const size_t, W>> &incoming_edges, const std::string &label)
                          { add_vertex(outgoing_edges, incoming_edges, label, {}); }

    void remove_vertex(size_t key) override;
    void remove_vertex(const std::string& label) override;
    void add_edge(size_t orig, size_t dest, const W& weight = {}) override;
    void print_adj_list() const;
    std::vector<BFSData> bfs(size_t start, std::function<void(size_t)> process_before = {},
                             std::function<void(size_t, const std::pair<const size_t, W>&)> process_during = {},
                             std::function<void(size_t)> process_after = {});
    std::vector<DFSData> dfs(std::function<void(size_t)> process_before = {},
                             std::function<void(size_t, const std::pair<const size_t, W>&)> process_during = {},
                             std::function<void(size_t)> process_after = {});
    void dfs_visit(size_t orig, std::vector<DFSData> &data, std::vector<SearchStatus> &status,
                   size_t &time, std::function<void(size_t)> process_before = {},
                   std::function<void(size_t, const std::pair<const size_t, W>&)> process_during = {},
                   std::function<void(size_t)> process_after = {});
};

// TODO: add copy constructor, copy assignment

template<typename V, typename W>
void GraphAL<V, W>::add_vertex(const std::initializer_list<std::pair<const size_t, W>> &outgoing_edges,
                               const std::initializer_list<std::pair<const size_t, W>> &incoming_edges,
                               const std::string &label, const V &data)
{
    size_t max_out = 0, max_in = 0;
    auto comp = [](const std::pair<size_t, W> &e1, const std::pair<size_t, W> &e2) { return e1.first < e2.first; };
    if (outgoing_edges.size())  // make sure vertices are valid
    {
        max_out = std::max(outgoing_edges, comp).first;
    }
    if (incoming_edges.size())
    {
        max_in = std::max(incoming_edges, comp).first;
    }

    if (max_out > adj_list.size() || max_in > adj_list.size())
    {
        throw std::invalid_argument("Invalid edge in initializer list.");
    }

    Vertex<V> vertex(current_key, data, label, is_labeled, data_is_key);
    vertices.push_back(vertex);
    std::map<size_t, W> temp(outgoing_edges);
    adj_list.push_back(temp);

    for (const auto &x : (is_directed ? incoming_edges : outgoing_edges))
    {
        adj_list[x.first].insert({current_key, x.second});
    }

    ++current_key;
}

template<typename V, typename W>
void GraphAL<V, W>::remove_vertex(size_t key)
{
    if (key < 0 || key > highest_key())
        throw std::out_of_range("Invalid vertex.");
  
    for (auto &x : adj_list)
    {
        x.remove_if(x.begin(), x.end(), [key](std::pair<size_t, W> &e) { return e.first == key; });
        for (auto &y : x)
        {
            if (y.first > key)   // adjust the values of the keys
                --y.first;
        }
    }

    adj_list.erase(adj_list.begin() + key);
    vertices.erase(vertices.begin() + key);
}

template<typename V, typename W>
void GraphAL<V, W>::add_edge(const size_t orig, const size_t dest, const W &weight)
{
    if (orig > highest_key() || dest > highest_key())
        throw std::out_of_range("Invalid vertex.");

    std::pair<size_t, W> edge_pair;
    if (!is_weighted)
    {
        if (!std::is_same<W, int>::value)
            throw std::invalid_argument("Unis_weighted graph must have weight of type int.");
        edge_pair = std::make_pair(dest, 1);
    }
    else
    {
        edge_pair = std::make_pair(dest, weight);
    }

    adj_list[orig].insert(edge_pair);

    if (!is_directed)
    {
        adj_list[dest].insert({orig, weight});
    }
}

template<typename V, typename W>
void GraphAL<V, W>::print_adj_list() const
{
    for (size_t i = 0; i < adj_list.size(); ++i)
    {
        is_labeled ? std::cout << vertices[i].label : std::cout << i;
        std::cout << ": ";
        for (const auto &x : adj_list[i])
        {
            std::cout << x.first;
            if (is_weighted)
            {
                std::cout << '/' << x.second;
            }
            std::cout << " ";
        }
    std::cout << std::endl;
    }
}

template<typename V, typename W>
std::vector<BFSData> GraphAL<V, W>::bfs(size_t start, std::function<void(size_t)> process_before,
                                                         std::function<void(size_t, const std::pair<const size_t, W>&)> process_during,
                                                         std::function<void(size_t)> process_after)
{
    if (start > highest_key())
        throw std::out_of_range("Invalid vertex.");

    std::vector<BFSData> data(adj_list.size());
    data[start].distance = 0;
    std::vector<SearchStatus> status(adj_list.size(), undiscovered);
    status[start] = discovered;

    std::queue<size_t> bfs_queue;
    bfs_queue.push(start);
    while (!bfs_queue.empty())
    {
        size_t orig = bfs_queue.front();
        bfs_queue.pop();
        if (process_before)
        {
            process_before(orig);
        }
        status[orig] = processed;
        for (const auto &neighbor : neighbors(orig))
        {
            size_t dest = neighbor.first;
            if (process_during && (status[dest] != processed || is_directed))
            {
                process_during(orig, neighbor);
            }
            if (status[dest] == undiscovered)
            {
                bfs_queue.push(dest);
                status[dest] = discovered;
                data[dest].distance = data[orig].distance + 1;
                data[dest].parent = orig;
            }
        }

        if (process_after)
        {
            process_after(orig);
        }
    }

    return data;
}

template<typename V, typename W>
std::vector<DFSData> GraphAL<V, W>::dfs(std::function<void(size_t)> process_before,
                                        std::function<void(size_t, const std::pair<const size_t, W>&)> process_during,
                                        std::function<void(size_t)> process_after)
{
    std::vector<DFSData> data(adj_list.size());
    std::vector<SearchStatus> status(adj_list.size(), undiscovered);
    size_t time = 0;
    
    for (size_t i = 0; i < adj_list.size(); ++i)
    {
        if (status[i] == undiscovered)
        {
            dfs_visit(i, data, status, time, process_before, process_during, process_after);
        }
    }

    return data;
}

template<typename V, typename W>
void GraphAL<V, W>::dfs_visit(size_t orig, std::vector<DFSData> &data, std::vector<SearchStatus> &status,
                              size_t &time, std::function<void(size_t)> process_before,
                              std::function<void(size_t, const std::pair<const size_t, W>&)> process_during,
                              std::function<void(size_t)> process_after)
{
    ++time;
    data[orig].d_time = time;
    status[orig] = discovered;
    if (process_before)
    {
        process_before(orig);
    }

    for (const auto &neighbor : neighbors(orig))
    {
        size_t dest = neighbor.first;
        if (status[dest] == undiscovered)
        {
            data[dest].parent = orig;
            if (process_during)
            {
                process_during(orig, neighbor);
            }
            dfs_visit(dest, data, status, time, process_before, process_during, process_after);
        }
        else if (process_during && (status[dest] != processed || is_directed))
        {
            process_during(orig, neighbor);
        }
    }

    if (process_after)
    {
        process_after(orig);
    }
    status[orig] = processed;
    ++time;
    data[orig].f_time = time;
}

// TODO: add Kruskal's, Prim's, Dijkstra's, etc.

#endif
