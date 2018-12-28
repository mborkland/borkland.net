#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <limits>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Vertex.hpp"

namespace bork_lib
{

enum class SearchStatus
{
    undiscovered,
    discovered,
    processed
};

struct BFSData
{
    std::size_t distance = std::numeric_limits<std::size_t>::max();
    std::size_t parent = std::numeric_limits<std::size_t>::max();
};

struct DFSData
{
    std::size_t d_time = 0;
    std::size_t f_time = 0;
    std::size_t parent = std::numeric_limits<std::size_t>::max();
};

template<typename W, typename Enable = void> struct default_weight {};
template<typename W>
struct default_weight<W, std::enable_if_t<std::is_arithmetic_v<W>>>
{
    W operator()() const noexcept { return static_cast<W>(1); }
};

constexpr void(*empty_vertex_func)(std::size_t) = [](std::size_t){};
template<typename W>
constexpr void(*empty_edge_func)(std::size_t, const std::pair<std::size_t, W>&)
    = [](std::size_t, const std::pair<std::size_t, W>&){};

template<typename AdjStructureType, typename V = size_t, typename W = int>
class Graph
{
protected:
    template<typename Enable = void> struct default_weight {};
    template<> struct default_weight<std::enable_if_t<std::is_arithmetic_v<W>>>
    {
        W operator()() const noexcept { return static_cast<W>(1); }
    };

    std::vector<Vertex<V>> vertices;         // the vertices and their associated data
    AdjStructureType adj_structure;          // holds vertex neighbor data
    std::unordered_map<std::string, std::size_t> labels_to_keys;  // maps labeled vertices to their key values
    std::size_t current_key = 0;
    std::size_t highest_key() const { return adj_structure.size() - 1; }
    template<typename... Ts> void valid_vertex_check(Ts... keys) const;
    template<typename... Ts> auto valid_label_check(Ts... labels) const;
    std::unordered_map<std::string, W> create_label_map(const std::unordered_map<std::size_t, W>& key_map) const;
    bool is_weighted;
    bool is_directed;
    bool is_labeled;
    bool data_is_key;
    Graph(bool is_weighted, bool is_directed, bool is_labeled, bool data_is_key)
            : is_weighted{is_weighted}, is_directed{is_directed}, is_labeled{is_labeled},
              data_is_key{data_is_key} {}

public:
    Graph(const Graph<AdjStructureType, V, W>&) = default;
    Graph(Graph<AdjStructureType, V, W>&&) noexcept = default;
    Graph<AdjStructureType, V, W>& operator=(const Graph<AdjStructureType, V, W>&) = default;
    Graph<AdjStructureType, V, W>& operator=(Graph<AdjStructureType, V, W>&&) noexcept = default;

    virtual void add_vertex(const std::vector<std::pair<std::size_t, W>>& outgoing_edges,
                            const std::vector<std::pair<std::size_t, W>>& incoming_edges,
                            const V& data, const std::string& label);
    void add_vertex() { add_vertex({}, {}, {}, {}); }
    void add_vertex(const V& data, const std::string& label = {}) { add_vertex({}, {}, data, label); }
    void add_vertex(const std::string label) { add_vertex({}, {}, {}, label); }
    void add_vertex(const std::vector<std::pair<std::size_t, W>>& outgoing_edges, const V& data = {},
                    const std::string& label = {}) { add_vertex(outgoing_edges, {}, data, label); }
    void add_vertex(const std::vector<std::pair<std::size_t, W>>& outgoing_edges,
                    const std::vector<std::pair<std::size_t, W>>& incoming_edges, const V &data = {})
                    { add_vertex(outgoing_edges, incoming_edges, data, {}); }
    void add_vertex(const std::vector<std::pair<std::size_t, W>>& outgoing_edges, const std::string& label)
                    { add_vertex(outgoing_edges, {}, {}, label); }
    void add_vertex(const std::vector<std::pair<std::size_t, W>>& outgoing_edges,
                    const std::vector<std::pair<std::size_t, W>>& incoming_edges, const std::string& label)
                    { add_vertex(outgoing_edges, incoming_edges, {}, label); }

    virtual void remove_vertex(std::size_t key);
    void remove_vertex(const std::string& label);

    virtual void add_edge(std::size_t orig, std::size_t dest, const W& weight) = 0;
    void add_edge(std::size_t orig, std::size_t dest) { add_edge(orig, dest, {}); }
    void add_edge(const std::string& orig, const std::string& dest, const W& weight = {});

    virtual void remove_edge(std::size_t orig, std::size_t dest) = 0;
    void remove_edge(const std::string& orig, const std::string& dest);

    virtual std::unordered_map<std::size_t, W> neighbors(std::size_t vertex) const = 0;
    virtual std::unordered_map<std::string, W> neighbors(const std::string& vertex) const = 0;

    template<typename FV1 = void(*)(size_t), typename FE = void(*)(size_t, const std::pair<size_t, W>&), typename FV2 = void(*)(size_t),
             typename = std::enable_if_t<std::is_convertible_v<decltype(std::declval<FV1>()(std::declval<std::size_t>())), void>
             && std::is_convertible_v<decltype(std::declval<FE>()(std::declval<std::size_t>(), std::declval<const std::pair<std::size_t, W>&>())), void>
             && std::is_convertible_v<decltype(std::declval<FV2>()(std::declval<std::size_t>())), void>>>
    std::vector<BFSData> bfs(std::size_t start, const FV1& process_vertex_early = empty_vertex_func, const FE& process_edge = empty_edge_func<W>,
                             const FV2& process_vertex_late = empty_vertex_func) const;
    template<typename FV1 = void(*)(size_t), typename FE = void(*)(size_t, const std::pair<size_t, W>&), typename FV2 = void(*)(size_t),
            typename = std::enable_if_t<std::is_convertible_v<decltype(std::declval<FV1>()(std::declval<std::size_t>())), void>
            && std::is_convertible_v<decltype(std::declval<FE>()(std::declval<std::size_t>(), std::declval<const std::pair<std::size_t, W>&>())), void>
            && std::is_convertible_v<decltype(std::declval<FV2>()(std::declval<std::size_t>())), void>>>
    std::vector<DFSData> dfs(const FV1& process_vertex_early = empty_vertex_func, const FE& process_edge = empty_edge_func<W>,
                             const FV2& process_vertex_late = empty_vertex_func) const;
    template<typename FV1 = void(*)(size_t), typename FE = void(*)(size_t, const std::pair<size_t, W>&), typename FV2 = void(*)(size_t),
            typename = std::enable_if_t<std::is_convertible_v<decltype(std::declval<FV1>()(std::declval<std::size_t>())), void>
            && std::is_convertible_v<decltype(std::declval<FE>()(std::declval<std::size_t>(), std::declval<const std::pair<std::size_t, W>&>())), void>
            && std::is_convertible_v<decltype(std::declval<FV2>()(std::declval<std::size_t>())), void>>>
    std::vector<DFSData> dfs_rec(const FV1& process_vertex_early = empty_vertex_func, const FE& process_edge = empty_edge_func<W>,
                                 const FV2& process_vertex_late = empty_vertex_func) const;
    template<typename FV1 = void(*)(size_t), typename FE = void(*)(size_t, const std::pair<size_t, W>&), typename FV2 = void(*)(size_t),
            typename = std::enable_if_t<std::is_convertible_v<decltype(std::declval<FV1>()(std::declval<std::size_t>())), void>
            && std::is_convertible_v<decltype(std::declval<FE>()(std::declval<std::size_t>(), std::declval<const std::pair<std::size_t, W>&>())), void>
            && std::is_convertible_v<decltype(std::declval<FV2>()(std::declval<std::size_t>())), void>>>
    void dfs_visit(std::size_t orig, std::vector<DFSData>& data, std::vector<SearchStatus>& status,
                   std::size_t& time, const FV1& process_vertex_early = empty_vertex_func, const FE& process_edge = empty_edge_func<W>,
                   const FV2& process_vertex_late = empty_vertex_func) const;

    const Vertex<V>& operator[](std::size_t key) const;
    Vertex<V>& operator[](std::size_t key);
    const Vertex<V>& operator[](const std::string& label) const;
    Vertex<V>& operator[](const std::string& label);
    void change_label(const std::string& label, const std::string& new_label);
    bool weighted() { return is_weighted; }
    bool directed() { return is_directed; }
    bool labeled() { return is_labeled; }
    bool has_satellite_data() { return !data_is_key; }
    std::size_t size() { return vertices.size(); }

    // testing function
    virtual void print_adj_structure() const = 0;
};

template<typename AdjStructureType, typename V, typename W>
template<typename... Ts>
void Graph<AdjStructureType, V, W>::valid_vertex_check(Ts... keys) const
{
    if ((keys || ...) > highest_key()) {
        throw std::out_of_range("Invalid vertex.");
    }
}

template<typename AdjStructureType, typename V, typename W>
template<typename... Ts>
auto Graph<AdjStructureType, V, W>::valid_label_check(Ts... labels) const
{
    if (!is_labeled) {
        throw std::logic_error("Cannot use this function with unlabeled graph.");
    }

    try {
        auto keys = std::make_tuple(labels_to_keys.at(labels)...);
        return keys;
    } catch (const std::out_of_range& e) {
        throw std::out_of_range("Vertice with given label does not exist in graph.");
    }
}

template<typename AdjStructureType, typename V, typename W>
std::unordered_map<std::string, W> Graph<AdjStructureType, V, W>::create_label_map(const std::unordered_map<std::size_t, W>& key_map) const
{
    std::unordered_map<std::string, W> label_map;
    for (const auto& key_pair : key_map) {
        const auto& label = vertices[key_pair.first].label();
        label_map[label] = key_pair.second;
    }

    return label_map;
}

template<typename AdjStructureType, typename V, typename W>
void Graph<AdjStructureType, V, W>::add_vertex(const std::vector<std::pair<std::size_t, W>>& outgoing_edges,
                                               const std::vector<std::pair<std::size_t, W>>& incoming_edges,
                                               const V& data, const std::string& label)
{
    std::size_t max_out = 0;
    std::size_t max_in = 0;
    auto comp = [](const std::pair<std::size_t, W>& e1, const std::pair<std::size_t, W>& e2) { return e1.first < e2.first; };
    if (outgoing_edges.size()) {               // make sure vertices are valid
        max_out = std::max_element(outgoing_edges.begin(), outgoing_edges.end(), comp)->first;
    }
    if (incoming_edges.size()) {
        max_in = std::max_element(incoming_edges.begin(), incoming_edges.end(), comp)->first;
    }

    if (max_out > adj_structure.size() || max_in > adj_structure.size()) {
        throw std::invalid_argument("Invalid edge in initializer list.");
    }

    Vertex<V> vertex{data_is_key, is_labeled, data, label};
    vertices.push_back(vertex);

    if (is_labeled) {
        if (!labels_to_keys.insert({vertex.label(), current_key}).second) {
            throw std::invalid_argument("Vertex with label already exists in graph.");
        }
    }
}

template<typename AdjStructureType, typename V, typename W>
void Graph<AdjStructureType, V, W>::add_edge(const std::string& orig, const std::string& dest, const W& weight)
{
    auto keys = valid_label_check(orig, dest);
    add_edge(std::get<0>(keys), std::get<1>(keys), weight);
}

template<typename AdjStructureType, typename V, typename W>
void Graph<AdjStructureType, V, W>::remove_edge(const std::string& orig, const std::string& dest)
{
    auto keys = valid_label_check(orig, dest);
    remove_edge(std::get<0>(keys), std::get<1>(keys));
}

template<typename AdjStructureType, typename V, typename W>
void Graph<AdjStructureType, V, W>::remove_vertex(std::size_t key)
{
    auto signed_key = static_cast<long long>(key);
    adj_structure.erase(adj_structure.begin() + signed_key);
    if (is_labeled) {
        labels_to_keys.erase(vertices[key].label());
        for (auto& label_pair : labels_to_keys) {
            if (label_pair.second > key) {
                --label_pair.second;
            }
        }
    }
    vertices.erase(vertices.begin() + signed_key);
}

template<typename AdjStructureType, typename V, typename W>
void Graph<AdjStructureType, V, W>::remove_vertex(const std::string& label)
{
    remove_vertex(std::get<0>(valid_label_check(label)));
}

template<typename AdjStructureType, typename V, typename W>
template<typename FV1, typename FE, typename FV2, typename>
std::vector<BFSData> Graph<AdjStructureType, V, W>::bfs(std::size_t start, const FV1& process_vertex_early, const FE& process_edge,
                                                        const FV2& process_vertex_late) const
{
    valid_vertex_check(start);
    auto num_vertices = adj_structure.size();
    std::vector<BFSData> data(num_vertices);
    data[start].distance = 0;
    std::vector<SearchStatus> status(num_vertices, SearchStatus::undiscovered);
    status[start] = SearchStatus::discovered;

    std::queue<std::size_t> bfs_queue;
    bfs_queue.push(start);
    while (!bfs_queue.empty()) {
        auto orig = bfs_queue.front();
        bfs_queue.pop();
        process_vertex_early(orig);
        status[orig] = SearchStatus::processed;

        for (const auto& neighbor : neighbors(orig)) {
            auto dest = neighbor.first;
            if (is_directed || (status[dest] != SearchStatus::processed)) {
                process_edge(orig, neighbor);
            }

            if (status[dest] == SearchStatus::undiscovered) {
                bfs_queue.push(dest);
                status[dest] = SearchStatus::discovered;
                data[dest].distance = data[orig].distance + 1;
                data[dest].parent = orig;
            }
        }

        process_vertex_late(orig);
    }

    return data;
}

template<typename AdjStructureType, typename V, typename W>
template<typename FV1, typename FE, typename FV2, typename>
std::vector<DFSData> Graph<AdjStructureType, V, W>::dfs(const FV1& process_vertex_early, const FE& process_edge,
                                                        const FV2& process_vertex_late) const
{
    auto num_vertices = adj_structure.size();
    std::vector<DFSData> data(num_vertices);
    std::vector<std::pair<std::size_t, std::size_t>> start_finish_times(num_vertices, {0, 0});
    std::vector<SearchStatus> status(num_vertices, SearchStatus::undiscovered);
    std::size_t time = 0;

    for (std::size_t index = 0; index < num_vertices; ++index) {
        std::stack<std::size_t> dfs_stack;
        if (status[index] == SearchStatus::undiscovered) {
            dfs_stack.push(index);
        }

        while (!dfs_stack.empty()) {
            ++time;
            auto orig = dfs_stack.top();
            if (status[orig] == SearchStatus::undiscovered) {
                data[orig].d_time = time;
                status[orig] = SearchStatus::discovered;
                process_vertex_early(orig);
            }

            bool processed = true;
            for (const auto& neighbor : neighbors(orig)) {
                auto dest = neighbor.first;
                if (status[dest] == SearchStatus::undiscovered) {
                    processed = false;
                    data[dest].parent = orig;
                    process_edge(orig, neighbor);
                    dfs_stack.push(dest);
                } else if (is_directed || (status[dest] != SearchStatus::processed)) {
                    process_edge(orig, neighbor);
                }
            }

            if (processed) {
                dfs_stack.pop();
                status[orig] = SearchStatus::processed;
                if (!data[orig].f_time) {  // check might not be necessary
                    data[orig].f_time = time;
                }

                process_vertex_late(orig);
            }
        }
    }

    return data;
}

template<typename AdjStructureType, typename V, typename W>
template<typename FV1, typename FE, typename FV2, typename>
std::vector<DFSData> Graph<AdjStructureType, V, W>::dfs_rec(const FV1& process_vertex_early, const FE& process_edge,
                                                            const FV2& process_vertex_late) const
{
    auto num_vertices = adj_structure.size();
    std::vector<DFSData> data(num_vertices);
    std::vector<SearchStatus> status(num_vertices, SearchStatus::undiscovered);
    std::size_t time = 0;

    for (std::size_t index = 0; index < num_vertices; ++index) {
        if (status[index] == SearchStatus::undiscovered) {
            dfs_visit(index, data, status, time, process_vertex_early, process_edge, process_vertex_late);
        }
    }

    return data;
}

template<typename AdjStructureType, typename V, typename W>
template<typename FV1, typename FE, typename FV2, typename>
void Graph<AdjStructureType, V, W>::dfs_visit(std::size_t orig, std::vector<DFSData>& data, std::vector<SearchStatus>& status,
                              std::size_t& time, const FV1& process_vertex_early, const FE& process_edge,
                              const FV2& process_vertex_late) const
{
    ++time;
    data[orig].d_time = time;
    status[orig] = SearchStatus::discovered;
    process_vertex_early(orig);

    for (const auto& neighbor : neighbors(orig)) {
        auto dest = neighbor.first;
        if (status[dest] == SearchStatus::undiscovered) {
            data[dest].parent = orig;
            process_edge(orig, neighbor);
            dfs_visit(dest, data, status, time, process_vertex_early, process_edge, process_vertex_late);
        } else if (is_directed || (status[dest] != SearchStatus::processed)) {
            process_edge(orig, neighbor);
        }
    }

    process_vertex_late(orig);
    status[orig] = SearchStatus::processed;
    ++time;
    data[orig].f_time = time;
}

template<typename AdjStructureType, typename V, typename W>
const Vertex<V>& Graph<AdjStructureType, V, W>::operator[](std::size_t key) const
{
    valid_vertex_check(key);
    return vertices[key];
}

template<typename AdjStructureType, typename V, typename W>
Vertex<V>& Graph<AdjStructureType, V, W>::operator[](std::size_t key)
{
    return const_cast<Vertex<V>&>(static_cast<const Graph<AdjStructureType, V, W>&>(*this)[key]);
}

template<typename AdjStructureType, typename V, typename W>
const Vertex<V>& Graph<AdjStructureType, V, W>::operator[](const std::string& label) const
{
    return vertices[std::get<0>(valid_label_check(label))];
}

template<typename AdjStructureType, typename V, typename W>
Vertex<V>& Graph<AdjStructureType, V, W>::operator[](const std::string& label)
{
    return const_cast<Vertex<V>&>(static_cast<const Graph<AdjStructureType, V, W>&>(*this)[label]);
}

template<typename AdjStructureType, typename V, typename W>
void Graph<AdjStructureType, V, W>::change_label(const std::string& label, const std::string& new_label)
{
    auto key = std::get<0>(valid_label_check(label));
    auto nh = labels_to_keys.extract(label);
    nh.key() = new_label;
    labels_to_keys.insert(std::move(nh));
    vertices[key].vertex_label = new_label;
}

} // end namespace

#endif
