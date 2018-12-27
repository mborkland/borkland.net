#ifndef GRAPH_AL_HPP
#define GRAPH_AL_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <vector>
#include "Graph.hpp"

namespace bork_lib
{

template<typename V, typename W> class GraphBuilder;

template<typename V = std::size_t, typename W = int>
class GraphAL : public Graph<std::vector<std::unordered_map<std::size_t, W>>, V, W>
{
private:
    using AdjType = std::unordered_map<std::size_t, W>;
    using AdjListType = std::vector<AdjType>;
    using Graph<AdjListType, V, W>::vertices;
    using Graph<AdjListType, V, W>::adj_structure;
    using Graph<AdjListType, V, W>::current_key;
    using Graph<AdjListType, V, W>::highest_key;
    using Graph<AdjListType, V, W>::valid_vertex_check;
    using Graph<AdjListType, V, W>::valid_label_check;
    using Graph<AdjListType, V, W>::create_label_map;
    using Graph<AdjListType, V, W>::is_weighted;
    using Graph<AdjListType, V, W>::is_directed;
    using Graph<AdjListType, V, W>::is_labeled;
    using Graph<AdjListType, V, W>::data_is_key;
    using Graph<AdjListType, V, W>::labels_to_keys;

    GraphAL(bool is_weighted, bool is_directed, bool is_labeled, bool data_is_key)
            : Graph<AdjListType, V, W>::Graph(is_weighted, is_directed, is_labeled, data_is_key) {}

public:
    using Graph<AdjListType, V, W>::add_vertex;
    using Graph<AdjListType, V, W>::add_edge;
    using Graph<AdjListType, V, W>::remove_edge;
    using Graph<AdjListType, V, W>::remove_vertex;
    using Graph<AdjListType, V, W>::bfs;
    using Graph<AdjListType, V, W>::dfs;
    using Graph<AdjListType, V, W>::dfs_rec;

    GraphAL(const GraphAL<V, W>&) = default;
    GraphAL(GraphAL<V, W>&&) noexcept = default;
    GraphAL& operator=(const GraphAL<V, W>&) = default;
    GraphAL& operator=(GraphAL<V, W>&&) noexcept = default;

    void add_vertex(const std::vector<std::pair<std::size_t, W>>& outgoing_edges,
                    const std::vector<std::pair<std::size_t, W>>& incoming_edges,
                    const V& data, const std::string& label) override;

    void remove_vertex(std::size_t key) override;

    void add_edge(std::size_t orig, std::size_t dest, const W& weight) override;

    void remove_edge(std::size_t orig, std::size_t dest) override;

    AdjType neighbors(std::size_t vertex) const override;
    std::unordered_map<std::string, W> neighbors(const std::string& vertex) const override;

    void print_adj_structure() const override;

    friend GraphBuilder<V, W>;
};

template<typename V, typename W>
void GraphAL<V, W>::add_vertex(const std::vector<std::pair<std::size_t, W>>& outgoing_edges,
                               const std::vector<std::pair<std::size_t, W>>& incoming_edges,
                               const V& data, const std::string& label)
{
    Graph<AdjListType, V, W>::add_vertex(outgoing_edges, incoming_edges, data, label);
    adj_structure.emplace_back(AdjType{outgoing_edges.begin(), outgoing_edges.end()});
    for (const auto& edge_pair : (is_directed ? incoming_edges : outgoing_edges)) {
        adj_structure[edge_pair.first].insert({current_key, edge_pair.second});
    }

    ++current_key;
}

template<typename V, typename W>
void GraphAL<V, W>::remove_vertex(std::size_t key)
{
    valid_vertex_check(key);
    for (auto& neighbor_map : adj_structure) {
        std::unordered_map<std::size_t, W> new_neighbor_map;
        for (const auto& neighbor : neighbor_map) {
            if (neighbor.first != key) {
                auto new_key = neighbor.first > key ? neighbor.first - 1 : neighbor.first;
                new_neighbor_map.emplace(new_key, std::move(neighbor.second));
            }
        }

        neighbor_map = new_neighbor_map;
    }

    Graph<AdjListType, V, W>::remove_vertex(key);
}

template<typename V, typename W>
void GraphAL<V, W>::add_edge(std::size_t orig, std::size_t dest, const W& weight)
{
    valid_vertex_check(orig, dest);
    auto actual_weight = is_weighted ? weight : default_weight<W>{}();
    auto edge_pair = std::make_pair(dest, actual_weight);
    adj_structure[orig].insert(edge_pair);

    if (!is_directed) {
        adj_structure[dest].insert({orig, weight});
    }
}

template<typename V, typename W>
void GraphAL<V, W>::remove_edge(std::size_t orig, std::size_t dest)
{
    valid_vertex_check(orig, dest);
    if (!adj_structure[orig].erase(dest)) {
        throw std::out_of_range("Edge does not exist in graph.");
    }

    if (!is_weighted) {
        adj_structure[dest].erase(orig);
    }
}

template<typename V, typename W>
typename GraphAL<V, W>::AdjType GraphAL<V, W>::neighbors(std::size_t vertex) const
{
    valid_vertex_check(vertex);
    return adj_structure[vertex];
}

template<typename V, typename W>
std::unordered_map<std::string, W> GraphAL<V, W>::neighbors(const std::string& vertex) const
{
    return create_label_map(adj_structure[std::get<0>(valid_label_check(vertex))]);
}

template<typename V, typename W>
void GraphAL<V, W>::print_adj_structure() const
{
    for (std::size_t i = 0; i < adj_structure.size(); ++i) {
        is_labeled ? std::cout << vertices[i].label() : std::cout << i;
        std::cout << ": ";
        for (const auto &x : adj_structure[i]) {
            std::cout << x.first;
            if (is_weighted) {
                std::cout << '/' << x.second;
            }

            std::cout << " ";
        }

        std::cout << std::endl;
    }
}

// TODO: add Kruskal's, Prim's, Dijkstra's, etc.

} // end namespace

#endif
