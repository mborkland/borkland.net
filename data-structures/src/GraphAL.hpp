#ifndef GRAPH_AL_HPP
#define GRAPH_AL_HPP

#include <iostream>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <functional>
#include "Graph.hpp"

template<typename V = std::size_t, typename W = int>
class GraphAL : public Graph<std::vector<std::unordered_map<std::size_t, W>>, V, W>
{
private:
    using AdjListType = std::vector<std::unordered_map<std::size_t,W>>;
    using Graph<AdjListType, V, W>::vertices;
    using Graph<AdjListType, V, W>::adj_structure;
    using Graph<AdjListType, V, W>::current_key;
    using Graph<AdjListType, V, W>::highest_key;
    using Graph<AdjListType, V, W>::valid_vertex_check;
    using Graph<AdjListType, V, W>::valid_edge_check;
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
    using Graph<AdjListType, V, W>::bfs;
    using Graph<AdjListType, V, W>::dfs;
    using Graph<AdjListType, V, W>::dfs_rec;

    GraphAL(const GraphAL<V, W>&) = default;
    GraphAL(GraphAL<V, W>&&) noexcept = default;
    GraphAL& operator=(const GraphAL<V, W>&) noexcept = default;
    GraphAL& operator=(GraphAL<V, W>&&) noexcept = default;

    void add_vertex(const std::initializer_list<std::pair<const std::size_t, W>> &outgoing_edges,
                    const std::initializer_list<std::pair<const std::size_t, W>> &incoming_edges,
                    const V& data, const std::string& label) override;

    void remove_vertex(std::size_t key) override;
    void remove_vertex(const std::string& label) override;

    void add_edge(std::size_t orig, std::size_t dest, const W& weight) override;

    void remove_edge(std::size_t orig, std::size_t dest) override;
    void remove_edge(const std::string& orig, const std::string& dest) override;

    void print_adj_structure() const override;
};

template<typename V, typename W>
void GraphAL<V, W>::add_vertex(const std::initializer_list<std::pair<const std::size_t, W>> &outgoing_edges,
                               const std::initializer_list<std::pair<const std::size_t, W>> &incoming_edges,
                               const V& data, const std::string& label)
{
    std::size_t max_out = 0;
    std::size_t max_in = 0;
    auto comp = [](const std::pair<std::size_t, W>& e1, const std::pair<std::size_t, W>& e2) { return e1.first < e2.first; };
    if (outgoing_edges.size()) {               // make sure vertices are valid
        max_out = std::max(outgoing_edges, comp).first;
    }
    if (incoming_edges.size()) {
        max_in = std::max(incoming_edges, comp).first;
    }

    if (max_out > adj_structure.size() || max_in > adj_structure.size()) {
        throw std::invalid_argument("Invalid edge in initializer list.");
    }

    Vertex<V> vertex{data_is_key, is_labeled, current_key, data_is_key ? current_key : data, is_labeled ? label : ""};
    vertices.push_back(vertex);
    std::unordered_map<std::size_t, W> temp{outgoing_edges};
    adj_structure.push_back(temp);

    for (const auto& vrtx : (is_directed ? incoming_edges : outgoing_edges)) {
        adj_structure[vrtx.first].insert({current_key, vrtx.second});
    }

    if (is_labeled) {
        if (!labels_to_keys.insert({label, current_key}).second) {
            throw std::invalid_argument("Vertex with label already exists in graph.");
        }
    }

    ++current_key;
}

template<typename V, typename W>
void GraphAL<V, W>::remove_vertex(std::size_t key)
{
    valid_vertex_check(key);
    for (const auto& vertex : adj_structure)
    {
        vertex.remove_if(vertex.begin(), vertex.end(), [key](std::pair<std::size_t, W>& edge){ return edge.first == key; });
        for (const auto& neighbor : vertex)
        {
            if (neighbor.first > key) {   // adjust the values of the keys
                --neighbor.first;
            }
        }
    }
    
    adj_structure.erase(adj_structure.begin() + key);
    labels_to_keys.erase(vertices[key].label());
    vertices.erase(vertices.begin() + key);
}

template<typename V, typename W>
void GraphAL<V, W>::remove_vertex(const std::string& label)
{
    if (!is_labeled) {
        throw std::logic_error("Cannot use this function with unlabeled graph.");
    }

    try {
        remove_vertex(labels_to_keys.at(label));
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("Vertice with given label does not exist in graph.");
    }
}

template<typename V, typename W>
void GraphAL<V, W>::add_edge(std::size_t orig, std::size_t dest, const W& weight)
{
    valid_vertex_check(orig, dest);
    auto edge_pair = std::make_pair(dest, is_weighted ? weight : 1);
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
void GraphAL<V, W>::remove_edge(const std::string& orig, const std::string& dest)
{
    try {
        remove_edge(labels_to_keys.at(orig), labels_to_keys.at(dest));
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("Vertice with given label does not exist in graph.");
    }
}

template<typename V, typename W>
void GraphAL<V, W>::print_adj_structure() const
{
    for (std::size_t i = 0; i < adj_structure.size(); ++i) {
        is_labeled ? std::cout << vertices[i].label : std::cout << i;
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

#endif
