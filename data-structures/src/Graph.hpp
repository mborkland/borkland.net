#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <limits>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Vertex.hpp"

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

template<typename AdjStructureType, typename V = size_t, typename W = int>
class Graph
{
private:
    std::vector<Vertex<V>> vertices;         // the vertices and their associated data
    AdjStructureType adj_structure;          // holds vertex neighbor data
    std::unordered_map<std::string, std::size_t> labels_to_keys;  // maps labeled vertices to their key values
    std::size_t current_key = 0;
    std::size_t highest_key() { return adj_structure.size() - 1; }
    template<typename... Ts> void valid_vertex_check(Ts... keys);
    void valid_edge_check(std::size_t orig, std::size_t dest);
    virtual std::unordered_map<std::size_t, W> neighbors(std::size_t vertex) = 0;
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

    virtual void add_vertex(const std::initializer_list<std::pair<const std::size_t, W>> &outgoing_edges,
                            const std::initializer_list<std::pair<const std::size_t, W>> &incoming_edges,
                            const V& data, const std::string& label) = 0;
    void add_vertex() { add_vertex({}, {}, {}, {}); }
    void add_vertex(const V& data, const std::string& label = {}) { add_vertex({}, {}, data, label); }
    void add_vertex(const std::string label) { add_vertex({}, {}, {}, label); }
    void add_vertex(const std::initializer_list<std::pair<const std::size_t, W>>& outgoing_edges, const V& data = {},
                    const std::string& label = {}) { add_vertex(outgoing_edges, {}, data, label); }
    void add_vertex(const std::initializer_list<std::pair<const std::size_t, W>> &outgoing_edges,
                    const std::initializer_list<std::pair<const std::size_t, W>> &incoming_edges, const V &data = {})
                    { add_vertex(outgoing_edges, incoming_edges, data, {}); }
    void add_vertex(const std::initializer_list<std::pair<const std::size_t, W>>& outgoing_edges, const std::string& label)
                    { add_vertex(outgoing_edges, {}, {}, label); }
    void add_vertex(const std::initializer_list<std::pair<const std::size_t, W>> &outgoing_edges,
                    const std::initializer_list<std::pair<const std::size_t, W>> &incoming_edges, const std::string& label)
                    { add_vertex(outgoing_edges, incoming_edges, {}, label); }

    virtual void remove_vertex(std::size_t key) = 0;
    virtual void remove_vertex(const std::string& label) = 0;

    virtual void add_edge(std::size_t orig, std::size_t dest, const W& weight) = 0;
    void add_edge(std::size_t orig, std::size_t dest) { add_edge(orig, dest, {}); }

    virtual void remove_edge(std::size_t orig, std::size_t dest) = 0;
    virtual void remove_edge(const std::string& orig, const std::string& dest) = 0;

    template<typename BeforeFunc, typename DuringFunc, typename AfterFunc>
    std::vector<BFSData> bfs(std::size_t start, BeforeFunc&& process_before = {}, DuringFunc&& process_during = {},
                             AfterFunc&& process_after = {}) const;
    template<typename BeforeFunc, typename DuringFunc, typename AfterFunc>
    std::vector<DFSData> dfs(BeforeFunc&& process_before = {}, DuringFunc&& process_during = {},
                                 AfterFunc&& process_after = {}) const;
    template<typename BeforeFunc, typename DuringFunc, typename AfterFunc>
    std::vector<DFSData> dfs_rec(BeforeFunc&& process_before = {}, DuringFunc&& process_during = {},
                             AfterFunc&& process_after = {}) const;
    template<typename BeforeFunc, typename DuringFunc, typename AfterFunc>
    void dfs_visit(std::size_t orig, std::vector<DFSData>& data, std::vector<SearchStatus>& status,
                   std::size_t& time, BeforeFunc&& process_before, DuringFunc&& process_during,
                   AfterFunc&& process_after) const;

    bool weighted() { return is_weighted; }
    bool directed() { return is_directed; }
    bool labeled() { return is_labeled; }
    bool has_satellite_data() { return !data_is_key; }

    // testing function
    virtual void print_adj_structure() const = 0;
};

template<typename AdjStructureType, typename V, typename W>
template<typename... Ts>
void Graph<AdjStructureType, V, W>::valid_vertex_check(Ts... keys)
{
    if ((keys || ...) > highest_key()) {
        throw std::out_of_range("Invalid vertex.");
    }
}

template<typename AdjStructureType, typename V, typename W>
void Graph<AdjStructureType, V, W>::valid_edge_check(std::size_t orig, std::size_t dest)
{
    
}

template<typename AdjStructureType, typename V, typename W>
template<typename BeforeFunc, typename DuringFunc, typename AfterFunc>
std::vector<BFSData> Graph<AdjStructureType, V, W>::bfs(std::size_t start, BeforeFunc&& process_before, DuringFunc&& process_during,
                                                        AfterFunc&& process_after) const
{
    if (start > highest_key()) {
        throw std::out_of_range("Invalid vertex.");
    }

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
        if (process_before) {
            process_before(orig);
        }
        status[orig] = SearchStatus::processed;

        for (const auto& neighbor : neighbors(orig)) {
            auto dest = neighbor.first;
            if (process_during && (status[dest] != SearchStatus::processed || is_directed)) {
                process_during(orig, neighbor);
            }

            if (status[dest] == SearchStatus::undiscovered) {
                bfs_queue.push(dest);
                status[dest] = SearchStatus::discovered;
                data[dest].distance = data[orig].distance + 1;
                data[dest].parent = orig;
            }
        }

        if (process_after) {
            process_after(orig);
        }
    }

    return data;
}

template<typename AdjStructureType, typename V, typename W>
template<typename BeforeFunc, typename DuringFunc, typename AfterFunc>
std::vector<DFSData> Graph<AdjStructureType, V, W>::dfs(BeforeFunc&& process_before, DuringFunc&& process_during,
                                                            AfterFunc&& process_after) const
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
            if (status[index] == SearchStatus::undiscovered) {
                data[orig].d_time = time;
                status[orig] = SearchStatus::discovered;
                if (process_before) {
                    process_before(orig);
                }
            }

            bool processed = true;
            for (const auto& neighbor : neighbors(orig)) {
                auto dest = neighbor.first;
                if (status[dest] == SearchStatus::undiscovered) {
                    processed = false;
                    data[dest].parent = orig;
                    if (process_during) {
                        process_during(orig, neighbor);
                    }

                    dfs_stack.push(dest);
                } else if (process_during && (status[dest] != SearchStatus::processed || is_directed)) {
                    process_during(orig, neighbor);
                }
            }

            if (processed) {
                dfs_stack.pop();
                status[orig] = SearchStatus::processed;
                if (!data[orig].f_time) {  // check might not be necessary
                    data[orig].f_time = time;
                }

                if (process_after) {
                    process_after(orig);
                }
            }
        }
    }

    return data;
}

template<typename AdjStructureType, typename V, typename W>
template<typename BeforeFunc, typename DuringFunc, typename AfterFunc>
std::vector<DFSData> Graph<AdjStructureType, V, W>::dfs_rec(BeforeFunc&& process_before, DuringFunc&& process_during,
                                                        AfterFunc&& process_after) const
{
    auto num_vertices = adj_structure.size();
    std::vector<DFSData> data(num_vertices);
    std::vector<SearchStatus> status(num_vertices, SearchStatus::undiscovered);
    std::size_t time = 0;

    for (std::size_t index = 0; index < num_vertices; ++index) {
        if (status[index] == SearchStatus::undiscovered) {
            dfs_visit(index, data, status, time, std::forward<BeforeFunc>(process_before),
                    std::forward<DuringFunc>(process_during), std::forward<AfterFunc>(process_after));
        }
    }

    return data;
}

template<typename AdjStructureType, typename V, typename W>
template<typename BeforeFunc, typename DuringFunc, typename AfterFunc>
void Graph<AdjStructureType, V, W>::dfs_visit(std::size_t orig, std::vector<DFSData>& data, std::vector<SearchStatus>& status,
                              std::size_t& time, BeforeFunc&& process_before, DuringFunc&& process_during,
                              AfterFunc&& process_after) const
{
    ++time;
    data[orig].d_time = time;
    status[orig] = SearchStatus::discovered;
    if (process_before) {
        process_before(orig);
    }

    for (const auto& neighbor : neighbors(orig)) {
        auto dest = neighbor.first;
        if (status[dest] == SearchStatus::undiscovered) {
            data[dest].parent = orig;
            if (process_during) {
                process_during(orig, neighbor);
            }

            dfs_visit(dest, data, status, time, std::forward<BeforeFunc>(process_before),
                      std::forward<DuringFunc>(process_during), std::forward<AfterFunc>(process_after));
        } else if (process_during && (status[dest] != SearchStatus::processed || is_directed)) {
            process_during(orig, neighbor);
        }
    }

    if (process_after) {
        process_after(orig);
    }

    status[orig] = SearchStatus::processed;
    ++time;
    data[orig].f_time = time;
}

#endif
