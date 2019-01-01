#ifndef GRAPH_AM_HPP
#define GRAPH_AM_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>
#include "Graph.hpp"

namespace bork_lib
{

template<typename V, typename W> class GraphBuilder;

template<typename V = std::size_t, typename W = int>
class GraphAM : public Graph<std::vector<std::vector<W>>, V, W>
{
private:
    using AdjType = std::vector<W>;
    using AdjMatrixType = std::vector<AdjType>;
    using Graph<AdjMatrixType, V, W>::vertices;
    using Graph<AdjMatrixType, V, W>::adj_structure;
    using Graph<AdjMatrixType, V, W>::current_key;
    using Graph<AdjMatrixType, V, W>::highest_key;
    using Graph<AdjMatrixType, V, W>::valid_vertex_check;
    using Graph<AdjMatrixType, V, W>::valid_label_check;
    using Graph<AdjMatrixType, V, W>::create_label_map;
    using Graph<AdjMatrixType, V, W>::is_weighted;
    using Graph<AdjMatrixType, V, W>::is_directed;
    using Graph<AdjMatrixType, V, W>::is_labeled;
    using Graph<AdjMatrixType, V, W>::satellite_data;
    using Graph<AdjMatrixType, V, W>::labels_to_keys;

    GraphAM(bool is_weighted, bool is_directed, bool is_labeled, bool data_is_key)
            : Graph<AdjMatrixType, V, W>::Graph(is_weighted, is_directed, is_labeled, data_is_key) {}

public:
    using Graph<AdjMatrixType, V, W>::add_vertex;
    using Graph<AdjMatrixType, V, W>::add_edge;
    using Graph<AdjMatrixType, V, W>::remove_edge;
    using Graph<AdjMatrixType, V, W>::remove_vertex;
    using Graph<AdjMatrixType, V, W>::bfs;
    using Graph<AdjMatrixType, V, W>::dfs;
    using Graph<AdjMatrixType, V, W>::dfs_rec;

    GraphAM(const GraphAM<V, W>&) = default;
    GraphAM(GraphAM<V, W>&&) noexcept = default;
    GraphAM& operator=(const GraphAM<V, W>&) = default;
    GraphAM& operator=(GraphAM<V, W>&&) noexcept = default;

    void add_vertex(const std::initializer_list<std::pair<const std::size_t, W>> &outgoing_edges,
                    const std::initializer_list<std::pair<const std::size_t, W>> &incoming_edges,
                    const V& data, const std::string& label) override;

    void remove_vertex(std::size_t key) override;

    void add_edge(std::size_t orig, std::size_t dest, const W& weight) override;

    void remove_edge(std::size_t orig, std::size_t dest) override;

    std::unordered_map<std::size_t, W> neighbors(std::size_t vertex) const override;
    std::unordered_map<std::string, W> neighbors(const std::string& vertex) const override;

    friend GraphBuilder<V, W>;
};

template<typename V, typename W>
void GraphAM<V, W>::add_vertex(const std::initializer_list<std::pair<const std::size_t, W>> &outgoing_edges,
                               const std::initializer_list<std::pair<const std::size_t, W>> &incoming_edges,
                               const V& data, const std::string& label)
{
    Graph<AdjMatrixType, V, W>::add_vertex(outgoing_edges, incoming_edges, data, label);
    AdjType temp(current_key);
    for (const auto& edge_pair : outgoing_edges) {
        temp[edge_pair.first] = edge_pair.second;
    }
    adj_structure.push_back(temp);
    for (const auto& edge_pair : (is_directed ? incoming_edges : outgoing_edges)) {
        adj_structure[edge_pair.first][current_key] = edge_pair.second;
    }

    ++current_key;
}

template<typename V, typename W>
void GraphAM<V, W>::remove_vertex(std::size_t key)
{
    valid_vertex_check(key);
    for (auto& vertex : adj_structure) {
        vertex.erase(vertex.begin() + static_cast<long long>(key));
    }
    Graph<AdjMatrixType, V, W>::remove_vertex(key);
}

template<typename V, typename W>
void GraphAM<V, W>::add_edge(std::size_t orig, std::size_t dest, const W& weight)
{
    valid_vertex_check(orig, dest);
    auto actual_weight = is_weighted ? weight : default_edge_weight<W>{}();
    adj_structure[orig][dest] = actual_weight;
    if (!is_directed) {
        adj_structure[dest][orig] = actual_weight;
    }
}

template<typename V, typename W>
void GraphAM<V, W>::remove_edge(std::size_t orig, std::size_t dest)
{
    valid_vertex_check(orig, dest);
    if (adj_structure[orig][dest]) {
        adj_structure[orig][dest] = {};
    } else {
        throw std::out_of_range("Edge does not exist in graph.");
    }

    if (!is_weighted) {
        adj_structure[dest][orig] = {};
    }
}

template<typename V, typename W>
std::unordered_map<std::size_t, W> GraphAM<V, W>::neighbors(std::size_t vertex) const
{
    valid_vertex_check(vertex);
    std::unordered_map<std::size_t, W> neighbor_map;
    auto neighbors = adj_structure[vertex];
    for (std::size_t i = 0; i < neighbors.size(); ++i) {
        if (neighbors[i] != W{}) {
            neighbor_map[i] = neighbors[i];
        }
    }

    return neighbor_map;
}

template<typename V, typename W>
std::unordered_map<std::string, W> GraphAM<V, W>::neighbors(const std::string& vertex) const
{
    return create_label_map(neighbors(std::get<0>(valid_label_check(vertex))));
}

} // end namespace

#endif
