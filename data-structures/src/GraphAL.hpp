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

/* Template parameters:
 * L = label type
 * V = vertex type
 * W = weight type */
template<typename L = std::size_t, typename V = std::size_t, typename W = int>
class GraphAL : public Graph<std::unordered_map<L, std::unordered_map<L, W>>, V, W>
{
private:
    using AdjType = std::unordered_map<L, W>;
    using AdjListType = std::unordered_map<L, AdjType>;
    using Graph<AdjListType, V, W>::vertices;
    using Graph<AdjListType, V, W>::adj_structure;
    using Graph<AdjListType, V, W>::current_key;
    using Graph<AdjListType, V, W>::invalid_label_exception;
    using Graph<AdjListType, V, W>::valid_label_check;
    using Graph<AdjListType, V, W>::is_weighted;
    using Graph<AdjListType, V, W>::is_directed;
    using Graph<AdjListType, V, W>::is_labeled;
    using Graph<AdjListType, V, W>::satellite_data;
    
public:
    using Graph<AdjListType, V, W>::add_vertex;
    using Graph<AdjListType, V, W>::add_edge;
    using Graph<AdjListType, V, W>::remove_edge;
    using Graph<AdjListType, V, W>::remove_vertex;
    using label_type = L;
    using vertex_type = V;
    using weight_type = W;

    GraphAL(const GraphAL<L, V, W>&) = default;
    GraphAL(GraphAL<L, V, W>&&) noexcept = default;
    GraphAL& operator=(const GraphAL<L, V, W>&) = default;
    GraphAL& operator=(GraphAL<L, V, W>&&) noexcept = default;

    void add_vertex(const std::vector<std::pair<label_type, W>>& outgoing_edges,
                    const std::vector<std::pair<label_type, W>>& incoming_edges,
                    const V& data, const std::string& label) override;

    void remove_vertex(const label_type& label) override;
    void add_edge(const label_type& orig, const label_type& dest, const W& weight) override;
    void remove_edge(const label_type& orig, const label_type& dest) override;
    AdjType neighbors(const label_type& label) const override;
    void change_label(const label_type& label, const label_type& new_label) override;

private:
    using iter_type = typename Graph<AdjListType, V, W>::iter_type;
    iter_type first_neighbor(const label_type& label) const noexcept override;
    iter_type next_neighbor(iter_type iter) const noexcept override;

    void remove_string_vertex(const std::string& label);
    void remove_numeric_vertex(std::size_t key);

    GraphAL(bool is_weighted, bool is_directed, bool is_labeled, bool satellite_data)
            : Graph<AdjListType, V, W>::Graph(is_weighted, is_directed, is_labeled, satellite_data) {}

    friend GraphBuilder<V, W>;
};

template<typename L, typename V, typename W>
void GraphAL<L, V, W>::add_vertex(const std::vector<std::pair<label_type, W>>& outgoing_edges,
                                  const std::vector<std::pair<label_type, W>>& incoming_edges,
                                  const V& data, const std::string& label)
{
    Graph<AdjListType, V, W>::add_vertex(outgoing_edges, incoming_edges, data, label);
    label_type actual_label;
    if constexpr (std::is_same_v<L, std::string>) {
        actual_label = label;
    } else {
        actual_label = current_key;
    }

    if (!adj_structure.emplace(actual_label, AdjType{outgoing_edges.begin(), outgoing_edges.end()}).second) {
        throw std::invalid_argument("Vertex with label already exists in graph.");
    }
    for (const auto& edge_pair : is_directed ? incoming_edges : outgoing_edges) {
        adj_structure[edge_pair.first].emplace(actual_label, edge_pair.second);
    }
    ++current_key;
}

template<typename L, typename V, typename W>
void GraphAL<L, V, W>::remove_vertex(const label_type& label)
{
    valid_label_check(label);
    if constexpr (std::is_same_v<L, std::string>) {
        remove_string_vertex(label);
    } else {
        remove_numeric_vertex(label);
    }
    --current_key;
}

template<typename L, typename V, typename W>
void GraphAL<L, V, W>::add_edge(const label_type& orig, const label_type& dest, const W& weight)
{
    valid_label_check(orig, dest);
    auto actual_weight = is_weighted ? weight : default_edge_weight<W>{}();
    auto edge_pair = std::make_pair(dest, actual_weight);
    adj_structure[orig].insert(edge_pair);

    if (!is_directed) {
        adj_structure[dest].insert({orig, actual_weight});
    }
}

template<typename L, typename V, typename W>
void GraphAL<L, V, W>::remove_edge(const label_type& orig, const label_type& dest)
{
    valid_label_check(orig, dest);
    if (!adj_structure[orig].erase(dest)) {
        throw std::out_of_range("Edge does not exist in graph.");
    }

    if (!is_weighted) {
        adj_structure[dest].erase(orig);
    }
}

template<typename L, typename V, typename W>
typename GraphAL<L, V, W>::AdjType GraphAL<L, V, W>::neighbors(const label_type& label) const
{
    try {
        return adj_structure.at(label);
    } catch (std::out_of_range& e) {
        throw invalid_label_exception;
    }
}

template<typename L, typename V, typename W>
void GraphAL<L, V, W>::change_label(const label_type& label, const label_type& new_label)
{
    if constexpr (std::is_same_v<L, std::size_t>) {
        throw std::logic_error("Label can only be changed for labeled graphs.");
    } else {
        valid_label_check(label);
        std::for_each(adj_structure.begin(), adj_structure.end(), [&](auto& vertex_pair){
            auto& neighbor_map = vertex_pair.second;
            auto neighbor_node = neighbor_map.extract(label);
            if (!neighbor_node.empty()) {
                neighbor_node.key() = new_label;
                neighbor_map.insert(std::move(neighbor_node));
            }
        });

        auto adj_node = adj_structure.extract(label);
        adj_node.key() = new_label;
        adj_structure.insert(std::move(adj_node));

        vertices.at(label).vertex_label = new_label;
        auto vert_node = vertices.extract(label);
        vert_node.key() = new_label;
        vertices.insert(std::move(vert_node));
    }
}

template<typename L, typename V, typename W>
typename GraphAL<L, V, W>::iter_type GraphAL<L, V, W>::first_neighbor(const label_type& label) const noexcept
{
    return adj_structure.at(label).cbegin();
}

template<typename L, typename V, typename W>
typename GraphAL<L, V, W>::iter_type GraphAL<L, V, W>::next_neighbor(iter_type iter) const noexcept
{
    return ++iter;
}

template<typename L, typename V, typename W>
void GraphAL<L, V, W>::remove_string_vertex(const std::string& label)
{
    std::for_each(adj_structure.begin(), adj_structure.end(), [&](auto& vertex_pair){
        auto& neighbor_map = vertex_pair.second;
        for (auto it = neighbor_map.begin(); it != neighbor_map.end(); ) {
            if (it->first == label) {
                it = neighbor_map.erase(it);
            } else {
                ++it;
            }
        }
    });
    vertices.erase(label);
    adj_structure.erase(label);
}

template<typename L, typename V, typename W>
void GraphAL<L, V, W>::remove_numeric_vertex(std::size_t key)
{
    AdjListType new_adj_structure;
    std::for_each(adj_structure.begin(), adj_structure.end(), [&](const auto& vertex_pair){
        auto [vertex, neighbor_map] = vertex_pair;
        if (vertex != key) {
            auto new_vertex = vertex > key ? vertex - 1 : vertex;
            AdjType new_neighbor_map;
            std::for_each(neighbor_map.begin(), neighbor_map.end(), [&](const auto& neighbor_pair){
                auto [neighbor, weight] = neighbor_pair;
                if (neighbor != key) {
                    auto new_neighbor = neighbor > key ? neighbor - 1 : neighbor;
                    new_neighbor_map.emplace(new_neighbor, std::move(weight));
                }
            });

            new_adj_structure.emplace(new_vertex, std::move(new_neighbor_map));
        }
    });

    adj_structure = new_adj_structure;

    std::unordered_map<label_type, Vertex<V>> new_vertices;
    std::for_each(vertices.begin(), vertices.end(), [&](const auto& vertex_pair){
        const auto& [old_key, vertex] = vertex_pair;
        if (old_key != key) {
            auto new_key = old_key > key ? old_key - 1 : old_key;
            new_vertices.emplace(new_key, std::move(vertex));
        }
    });

    vertices = new_vertices;
}

} // end namespace

#endif
