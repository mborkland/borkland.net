#ifndef GRAPH_AM_HPP
#define GRAPH_AM_HPP

#include <functional>
#include <iterator>
#include "Graph.hpp"

namespace bork_lib
{

template<typename L, typename W, typename V> class GraphBuilder;

/* Template parameters:
 * L = label type
 * V = vertex type
 * W = weight type */
template<typename L = std::size_t, typename W = int, typename V = std::size_t>
class GraphAM : public Graph<std::vector<std::vector<W>>, L, W, V>
{
private:
    using AdjType = std::vector<W>;
    using AdjMatrixType = std::vector<AdjType>;
    using Graph<AdjMatrixType, L, W, V>::vertices;
    using Graph<AdjMatrixType, L, W, V>::adj_structure;
    using Graph<AdjMatrixType, L, W, V>::current_key;
    using Graph<AdjMatrixType, L, W, V>::graph_capacity;
    using Graph<AdjMatrixType, L, W, V>::invalid_label_exception;
    using Graph<AdjMatrixType, L, W, V>::invalid_edge_exception;
    using Graph<AdjMatrixType, L, W, V>::invalid_vertex_exception;
    using Graph<AdjMatrixType, L, W, V>::change_label_exception;
    using Graph<AdjMatrixType, L, W, V>::duplicate_label_exception;
    using Graph<AdjMatrixType, L, W, V>::repeat_edge_exception;
    using Graph<AdjMatrixType, L, W, V>::validate_label;
    using Graph<AdjMatrixType, L, W, V>::shift_vertices;
    using Graph<AdjMatrixType, L, W, V>::is_weighted;
    using Graph<AdjMatrixType, L, W, V>::is_directed;
    using Graph<AdjMatrixType, L, W, V>::is_labeled;
    using Graph<AdjMatrixType, L, W, V>::satellite_data;

public:
    using Graph<AdjMatrixType, L, W, V>::add_vertex;
    using Graph<AdjMatrixType, L, W, V>::add_edge;
    using Graph<AdjMatrixType, L, W, V>::remove_edge;
    using Graph<AdjMatrixType, L, W, V>::remove_vertex;
    using Graph<AdjMatrixType, L, W, V>::size;
    using label_type = L;
    using vertex_type = V;
    using weight_type = W;

    // no manual memory management so compiler-generated functions are sufficient
    GraphAM(const GraphAM<L, W, V>&) = default;
    GraphAM(GraphAM<L, W, V>&&) noexcept = default;
    GraphAM& operator=(const GraphAM<L, W, V>&) = default;
    GraphAM& operator=(GraphAM<L, W, V>&&) noexcept = default;

    void add_vertex(const std::vector<std::pair<L, W>>& outgoing_edges,
                    const std::vector<std::pair<L, W>>& incoming_edges,
                    const V& data, const std::string& label) override;

    void add_edge(const label_type& orig, const label_type& dest, const weight_type& weight) override;
    void remove_edge(const label_type& orig, const label_type& dest) override;
    std::unordered_map<L, W> neighbors(const label_type& label) const override;
    std::optional<W> edge_weight(const label_type& orig, const label_type& dest) const noexcept override;
    void change_label(const label_type& label, const label_type& new_label) override;
    void reserve(std::size_t new_capacity) override;
    void shrink_to_fit();
    void clear() noexcept override;

private:
    std::unordered_map<std::string, std::size_t> labels_to_keys;
    std::vector<std::string> keys_to_labels;

    void check_edge_list(const std::vector<std::pair<L, W>>& edges) override;
    std::size_t get_index(const label_type& label) const;
    std::size_t get_index(const label_type& label);

    mutable std::vector<std::unordered_map<L, W>> dfs_neighbor_lists;  /* a buffer to hold the vertices's neighbor
                                                                          lists during DFS */
    using iter_type = typename Graph<AdjMatrixType, L, W, V>::iter_type;
    iter_type first_neighbor(const label_type& label) const noexcept override;
    iter_type neighbors_end(const label_type& label) const noexcept override;

    void remove_string_vertex(const std::string& label) override;
    void remove_numeric_vertex(std::size_t key) override;

    void increase_capacity(std::size_t new_capacity);

    GraphAM(bool is_weighted, bool is_directed, bool data_is_key, std::size_t init_capacity)
        : Graph<AdjMatrixType, L, W, V>::Graph(is_weighted, is_directed, data_is_key, init_capacity)
        { increase_capacity(graph_capacity); }

    friend GraphBuilder<L, W, V>;
};

/* Adds a vertex to the graph. Overloads are available to make it possible to specify almost any
 * combination of input arguments. However, it is not possible to specify incoming_edges and not
 * outgoing_edges due to ambiguity. Overrides the base class function of the same name, calls the
 * base class function and then handles changes that need to be made to the adjacency matrix. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::add_vertex(const std::vector<std::pair<L, W>>& outgoing_edges,
                                  const std::vector<std::pair<L, W>>& incoming_edges,
                                  const V& data, const std::string& label)
{
    Graph<AdjMatrixType, L, W, V>::add_vertex(outgoing_edges, incoming_edges, data, label);
    if (current_key >= graph_capacity) {
        reserve(graph_capacity * 2);
    }

    for (std::size_t index = 0; index < current_key; ++index) {  // add new column to the previous rows
        adj_structure[index].emplace_back(W{});
    }

    // sort the outgoing edge list by key
    auto sorted_outgoing = outgoing_edges;
    if constexpr (is_labeled) {
        std::sort(sorted_outgoing.begin(), sorted_outgoing.end(), [this](const auto& a, const auto& b){
            return labels_to_keys[a.first] < labels_to_keys[b.first]; });
    } else {
        std::sort(sorted_outgoing.begin(), sorted_outgoing.end(), [](const auto& a, const auto& b){
            return a.first < b.first; });
    }

    // fill the new row of the matrix
    for (const auto& edge_pair : sorted_outgoing) {
        auto edge_index = get_index(edge_pair.first);
        // fill the row with default values until the edge index
        adj_structure[current_key].resize(edge_index);
        // put the edge weight at the edge index
        adj_structure[current_key].push_back(edge_pair.second);
    }

    // fill the rest of the row with default values
    adj_structure[current_key].resize(current_key + 1);

    for (const auto& edge_pair : (is_directed ? incoming_edges : outgoing_edges)) {
        auto edge_index = get_index(edge_pair.first);
        adj_structure[edge_index][current_key] = edge_pair.second;
    }

    if constexpr (is_labeled) {
        if (!labels_to_keys.emplace(label, current_key).second) {
            throw std::invalid_argument{duplicate_label_exception};
        }
        keys_to_labels.push_back(label);
    }

    dfs_neighbor_lists.resize(size());
    ++current_key;
}

/* Adds an edge to the graph. Weight parameter is optional due to an overload in the
 * base class. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::add_edge(const label_type& orig, const label_type& dest, const weight_type& weight)
{
    validate_label(orig);
    validate_label(dest);
    auto actual_weight = is_weighted ? weight : default_edge_weight<W>{}();
    auto orig_index = get_index(orig);
    auto dest_index = get_index(dest);

    adj_structure[orig_index][dest_index] = actual_weight;
    if (!is_directed) {
        adj_structure[dest_index][orig_index] = actual_weight;
    }
}

/* Removes an edge from the graph. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::remove_edge(const label_type& orig, const label_type& dest)
{
    validate_label(orig);
    validate_label(dest);
    auto orig_index = get_index(orig);
    auto dest_index = get_index(dest);

    if (adj_structure[orig_index][dest_index] != W{}) {
        adj_structure[orig_index][dest_index] = W{};
    } else {
        throw std::out_of_range{invalid_edge_exception};
    }

    if (!is_directed) {
        adj_structure[dest_index][orig_index] = W{};
    }
}

/* Returns the neighbors of the given vertex as a std::unordered_map<L, W>. */
template<typename L, typename W, typename V>
std::unordered_map<L, W> GraphAM<L, W, V>::neighbors(const label_type& label) const
{
    validate_label(label);
    std::unordered_map<L, W> neighbor_map;
    AdjType neighbors;
    if constexpr (is_labeled) {
        neighbors = adj_structure[labels_to_keys.at(label)];
    } else {
        neighbors = adj_structure[label];
    }

    for (std::size_t i = 0; i < neighbors.size(); ++i) {
        if (neighbors[i] != W{}) {
            if constexpr (is_labeled) {
                neighbor_map[keys_to_labels[i]] = neighbors[i];
            } else {
                neighbor_map[i] = neighbors[i];
            }
        }
    }

    return neighbor_map;
}

/* Returns the weight of an edge between two vertices, if it exists. */
template<typename L, typename W, typename V>
std::optional<W> GraphAM<L, W, V>::edge_weight(const label_type& orig, const label_type& dest) const noexcept
{
    validate_label(orig);
    validate_label(dest);
    W weight;
    if constexpr (is_labeled) {
        weight = adj_structure[labels_to_keys.at(orig)][labels_to_keys.at(dest)];
    } else {
        weight = adj_structure[orig][dest];
    }

    if (weight != W{}) {
        return weight;
    } else {
        return std::nullopt;
    }
}

/* Allows the user to change the label of a labeled graph. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::change_label(const label_type& label, const label_type& new_label)
{
    if constexpr (!is_labeled) {
        throw std::logic_error{change_label_exception};
    } else {
        validate_label(label);
        if (labels_to_keys.find(new_label) != labels_to_keys.end()) {
            throw std::invalid_argument{duplicate_label_exception};
        }
        auto label_node = labels_to_keys.extract(label);
        label_node.key() = new_label;
        labels_to_keys.insert(std::move(label_node));
        keys_to_labels[labels_to_keys[new_label]] = new_label;

        vertices.at(label).vertex_label = new_label;
        auto vert_node = vertices.extract(label);
        vert_node.key() = new_label;
        vertices.insert(std::move(vert_node));
    }
}

/* Allows the user to reserve space in the adjacency matrix to reduce reallocations
 * and to increase the bucket count of the underlying hash tables to reduce rehashes. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::reserve(std::size_t new_capacity)
{
    if (new_capacity > graph_capacity) {
        vertices.reserve(new_capacity);
        dfs_neighbor_lists.reserve(new_capacity);
        keys_to_labels.reserve(new_capacity);
        labels_to_keys.reserve(new_capacity);
        auto adj_size = adj_structure.size();
        for (std::size_t index = 0; index < adj_size; ++index) {
            adj_structure[index].reserve(new_capacity);
        }

        increase_capacity(new_capacity);
        graph_capacity = new_capacity;
    }
}

/* Allows the user to reduce the capacity of any std::vector used by the class
 * to a size equal to the number of vertices. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::shrink_to_fit()
{
    if (graph_capacity > size()) {
        dfs_neighbor_lists.shrink_to_fit();
        keys_to_labels.shrink_to_fit();
        auto it = adj_structure.begin();
        std::advance(it, size());
        adj_structure.erase(it, adj_structure.end());
        for (auto& neighbors : adj_structure) {
            neighbors.shrink_to_fit();
        }
        graph_capacity = adj_structure.size();
    }
}

/* Extension of the base class clear function that clears the extra data
 * structures needed by GraphAM. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::clear() noexcept
{
    labels_to_keys.clear();
    keys_to_labels.clear();
    dfs_neighbor_lists.clear();
    Graph<AdjMatrixType, L, W, V>::clear();
}

/* Checks an edge initializer list passed into the add_vertex function for validity. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::check_edge_list(const std::vector<std::pair<L, W>>& edges)
{
    std::unordered_set<L> edges_seen;
    for (const auto& [neighbor, _] : edges) {
        auto neighbor_index = get_index(neighbor);
        if (neighbor_index >= current_key) {
            throw std::invalid_argument{invalid_vertex_exception};
        }

        if (!edges_seen.insert(neighbor).second) {
            throw std::invalid_argument{repeat_edge_exception};
        }
    }
}

/* Returns the correct index of the adjacency matrix that corresponds to a label,
 * const version. */
template<typename L, typename W, typename V>
std::size_t GraphAM<L, W, V>::get_index(const label_type& label) const
{
    if constexpr (is_labeled) {
        return labels_to_keys.at(label);
    } else {
        return label;
    }
}

/* Returns the correct index of the adjacency matrix that corresponds to a label,
 * non-const version. */
template<typename L, typename W, typename V>
std::size_t GraphAM<L, W, V>::get_index(const label_type& label)
{
    if constexpr (is_labeled) {
        return labels_to_keys[label];
    } else {
        return label;
    }
}

/* Used in a DFS search to find the first neighbor in a vertex's neighbor set. */
template<typename L, typename W, typename V>
typename GraphAM<L, W, V>::iter_type GraphAM<L, W, V>::first_neighbor(const label_type& label) const noexcept
{
    auto index = get_index(label);
    dfs_neighbor_lists[index] = neighbors(label);
    return dfs_neighbor_lists[index].cbegin();
}

/* Used in a DFS search to indicate the end of a vertex's neighbor set. */
template<typename L, typename W, typename V>
typename GraphAM<L, W, V>::iter_type GraphAM<L, W, V>::neighbors_end(const label_type& label) const noexcept
{
    auto index = get_index(label);
    return dfs_neighbor_lists[index].cend();
}

/* Removes a vertex in a labeled graph. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::remove_string_vertex(const std::string& label)
{
    if constexpr (is_labeled) {
        auto key = labels_to_keys[label];
        remove_numeric_vertex(key);
        vertices.erase(label);
        dfs_neighbor_lists.resize(size());
        labels_to_keys.erase(label);
        std::for_each(labels_to_keys.begin(), labels_to_keys.end(), [&](auto& label_pair){
            if (label_pair.second > key) {
                --label_pair.second;
            }
        });
        keys_to_labels.erase(keys_to_labels.begin() + static_cast<long long>(key));
    }
}

/* Removes a vertex in an unlabeled graph. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::remove_numeric_vertex(std::size_t key)
{
    auto signed_key = static_cast<long long>(key);
    for (std::size_t index = 0; index < size(); ++index) {
        adj_structure[index].erase(adj_structure[index].begin() + signed_key);
    }
    adj_structure.erase(adj_structure.begin() + signed_key);

    if constexpr (!is_labeled) {
        vertices.erase(key);
        dfs_neighbor_lists.resize(size());
        shift_vertices(key);
    }
}

/* Increases the capacity of the adjacency matrix. */
template<typename L, typename W, typename V>
void GraphAM<L, W, V>::increase_capacity(std::size_t new_capacity)
{
    adj_structure.reserve(new_capacity);
    auto adj_size = adj_structure.size();
    for (std::size_t index = 0; index < new_capacity - adj_size; ++index) {
        std::vector<W> neighbors;
        neighbors.reserve(new_capacity);
        adj_structure.push_back(neighbors);
    }

    graph_capacity = new_capacity;
}

} // end namespace

#endif
