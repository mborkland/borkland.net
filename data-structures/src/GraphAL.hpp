#ifndef GRAPH_AL_HPP
#define GRAPH_AL_HPP

#include <functional>
#include <unordered_set>
#include "Graph.hpp"

namespace bork_lib
{

template<typename L, typename W, typename V> class GraphBuilder;

/* Template parameters:
 * L = label type
 * V = vertex type
 * W = weight type */
template<typename L = std::size_t, typename W = int, typename V = std::size_t>
class GraphAL : public Graph<std::unordered_map<L, std::unordered_map<L, W>>, L, W, V>
{
private:
    using AdjType = std::unordered_map<L, W>;
    using AdjListType = std::unordered_map<L, AdjType>;
    using Graph<AdjListType, L, W, V>::vertices;
    using Graph<AdjListType, L, W, V>::adj_structure;
    using Graph<AdjListType, L, W, V>::current_key;
    using Graph<AdjListType, L, W, V>::graph_capacity;
    using Graph<AdjListType, L, W, V>::invalid_label_exception;
    using Graph<AdjListType, L, W, V>::invalid_edge_exception;
    using Graph<AdjListType, L, W, V>::invalid_vertex_exception;
    using Graph<AdjListType, L, W, V>::change_label_exception;
    using Graph<AdjListType, L, W, V>::duplicate_label_exception;
    using Graph<AdjListType, L, W, V>::repeat_edge_exception;
    using Graph<AdjListType, L, W, V>::validate_label;
    using Graph<AdjListType, L, W, V>::shift_vertices;
    using Graph<AdjListType, L, W, V>::is_weighted;
    using Graph<AdjListType, L, W, V>::is_directed;
    using Graph<AdjListType, L, W, V>::is_labeled;
    using Graph<AdjListType, L, W, V>::satellite_data;
    
public:
    using Graph<AdjListType, L, W, V>::add_vertex;
    using Graph<AdjListType, L, W, V>::add_edge;
    using Graph<AdjListType, L, W, V>::remove_edge;
    using Graph<AdjListType, L, W, V>::remove_vertex;
    using Graph<AdjListType, L, W, V>::size;
    using label_type = L;
    using vertex_type = V;
    using weight_type = W;

    // no manual memory management so compiler-generated functions are sufficient
    GraphAL(const GraphAL<L, W, V>&) = default;
    GraphAL(GraphAL<L, W, V>&&) = default;
    GraphAL& operator=(const GraphAL<L, W, V>&) = default;
    GraphAL& operator=(GraphAL<L, W, V>&&) = default;

    void add_vertex(const std::vector<std::pair<L, W>>& outgoing_edges,
                    const std::vector<std::pair<L, W>>& incoming_edges,
                    const V& data, const std::string& label) override;

    void add_edge(const label_type& orig, const label_type& dest, const weight_type& weight) override;
    void remove_edge(const label_type& orig, const label_type& dest) override;
    AdjType neighbors(const label_type& label) const override;
    std::optional<W> edge_weight(const label_type& orig, const label_type& dest) const noexcept override;
    void change_label(const label_type& label, const label_type& new_label) override;
    void reserve(std::size_t new_capacity) override;

private:
    void check_edge_list(const std::vector<std::pair<L, W>>& edges) override;

    using iter_type = typename Graph<AdjListType, L, W, V>::iter_type;
    iter_type first_neighbor(const label_type& label) const noexcept override;
    iter_type neighbors_end(const label_type& label) const noexcept override;

    void remove_string_vertex(const std::string& label) override;
    void remove_numeric_vertex(std::size_t key) override;

    GraphAL(bool is_weighted, bool is_directed, bool satellite_data, std::size_t init_capacity)
            : Graph<AdjListType, L, W, V>{is_weighted, is_directed, satellite_data, init_capacity}
            { adj_structure.reserve(init_capacity); }

    friend GraphBuilder<L, W, V>;
};

/* Adds a vertex to the graph. Overloads are available to make it possible to specify almost any
 * combination of input arguments. However, it is not possible to specify incoming_edges and not
 * outgoing_edges due to ambiguity. Overrides the base class function of the same name, calls the
 * base class function and then handles changes that need to be made to the adjacency list. */
template<typename L, typename W, typename V>
void GraphAL<L, W, V>::add_vertex(const std::vector<std::pair<L, W>>& outgoing_edges,
                                  const std::vector<std::pair<L, W>>& incoming_edges,
                                  const V& data, const std::string& label)
{
    Graph<AdjListType, L, W, V>::add_vertex(outgoing_edges, incoming_edges, data, label);
    label_type actual_label;
    if constexpr (is_labeled) {
        actual_label = label;
    } else {
        actual_label = current_key;
    }

    if (!adj_structure.emplace(actual_label, AdjType{outgoing_edges.begin(), outgoing_edges.end()}).second) {
        throw std::out_of_range{invalid_label_exception};
    }
    for (const auto& edge_pair : is_directed ? incoming_edges : outgoing_edges) {
        adj_structure[edge_pair.first].emplace(actual_label, edge_pair.second);
    }

    if (current_key >= graph_capacity) {
        reserve(graph_capacity * 2);
    }
    ++current_key;
}

/* Adds an edge to the graph. Weight parameter is optional due to an overload in the
 * base class. */
template<typename L, typename W, typename V>
void GraphAL<L, W, V>::add_edge(const label_type& orig, const label_type& dest, const weight_type& weight)
{
    validate_label(orig);
    validate_label(dest);
    auto actual_weight = is_weighted ? weight : default_edge_weight<W>{}();
    auto edge_pair = std::make_pair(dest, actual_weight);
    adj_structure[orig].insert(edge_pair);
    if (!is_directed) {
        adj_structure[dest].insert({orig, actual_weight});
    }
}

/* Removes an edge from the graph. */
template<typename L, typename W, typename V>
void GraphAL<L, W, V>::remove_edge(const label_type& orig, const label_type& dest)
{
    validate_label(orig);
    validate_label(dest);
    if (!adj_structure[orig].erase(dest)) {
        throw std::out_of_range{invalid_edge_exception};
    }

    if (!is_weighted) {
        adj_structure[dest].erase(orig);
    }
}

/* Returns the neighbors of the given vertex as a std::unordered_map<L, W>. */
template<typename L, typename W, typename V>
typename GraphAL<L, W, V>::AdjType GraphAL<L, W, V>::neighbors(const label_type& label) const
{
    try {
        return adj_structure.at(label);
    } catch (std::out_of_range& e) {
        throw std::out_of_range{invalid_label_exception};
    }
}

/* Returns the weight of an edge between two vertices, if it exists. */
template<typename L, typename W, typename V>
std::optional<W> GraphAL<L, W, V>::edge_weight(const label_type& orig, const label_type& dest) const noexcept
{
    validate_label(orig);
    validate_label(dest);
    auto neighbors = adj_structure.at(orig);
    auto it = neighbors.find(dest);
    if (it != neighbors.end()) {
        return neighbors[dest];
    } else {
        return std::nullopt;
    }
}

/* Allows the user to change the label of a labeled graph. */
template<typename L, typename W, typename V>
void GraphAL<L, W, V>::change_label(const label_type& label, const label_type& new_label)
{
    if constexpr (!is_labeled) {
        throw std::logic_error{change_label_exception};
    } else {
        validate_label(label);
        std::for_each(adj_structure.begin(), adj_structure.end(), [&](auto& vertex_pair){
            auto& [vertex_label, neighbor_map] = vertex_pair;
            if (vertex_label == new_label) {
                throw std::invalid_argument{duplicate_label_exception};
            }
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

/* Allows the user to increase the bucket count of the underlying
 * hash tables in order to reduce rehashes. */
template<typename L, typename W, typename V>
void GraphAL<L, W, V>::reserve(std::size_t new_capacity)
{
    if (new_capacity > graph_capacity) {
        vertices.reserve(new_capacity);
        adj_structure.reserve(new_capacity);
        graph_capacity = new_capacity;
    }
}

/* Checks a edge initializer list passed into the add_vertex function for validity. */
template<typename L, typename W, typename V>
void GraphAL<L, W, V>::check_edge_list(const std::vector<std::pair<L, W>>& edges)
{
    std::unordered_set<L> edges_seen;
    for (const auto& [neighbor, _] : edges) {
        auto it = adj_structure.find(neighbor);
        if (it == adj_structure.end()) {
            throw std::invalid_argument{invalid_vertex_exception};
        }

        if (!edges_seen.insert(neighbor).second) {
            throw std::invalid_argument{repeat_edge_exception};
        }
    }
}

/* Used in a DFS search to find the first neighbor in a vertex's neighbor set. */
template<typename L, typename W, typename V>
typename GraphAL<L, W, V>::iter_type GraphAL<L, W, V>::first_neighbor(const label_type& label) const noexcept
{
    return adj_structure.at(label).cbegin();
}

/* Used in a DFS search to indicate the end of a vertex's neighbor set. */
template<typename L, typename W, typename V>
typename GraphAL<L, W, V>::iter_type GraphAL<L, W, V>::neighbors_end(const L& label) const noexcept
{
    return adj_structure.at(label).cend();
}

/* Removes a vertex in a labeled graph. */
template<typename L, typename W, typename V>
void GraphAL<L, W, V>::remove_string_vertex(const std::string& label)
{
    if constexpr (is_labeled) {
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
}

/* Removes a vertex in an unlabeled graph. */
template<typename L, typename W, typename V>
void GraphAL<L, W, V>::remove_numeric_vertex(std::size_t key)
{
    if constexpr (!is_labeled) {
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
        shift_vertices(key);
    }
}

} // end namespace

#endif
