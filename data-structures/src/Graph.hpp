#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <limits>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "Vertex.hpp"

namespace bork_lib
{

/* Represents the state of each vertex during a search of the graph. */
enum class SearchStatus
{
    undiscovered,
    discovered,
    processed
};

/* The data returned for each vertex after a breadth-first search.
 * distance = number of edges from the start vertex
 * parent = vertex visited before each one during the search */
template<typename L>
struct BFSData
{
    std::size_t distance = std::numeric_limits<std::size_t>::max();
    L parent;
    constexpr BFSData()
    {
        if constexpr (std::is_same_v<L, std::string>) {
            parent = "__DFS_SEARCH_NO_PARENT__";
        } else {
            parent = std::numeric_limits<std::size_t>::max();
        }
    }
};

/* The data returned for each vertex after a depth-first search.
 * d_time = time step when the vertex is discovered in the search
 * f_time = time step when the vertex is accessed again after the stack unwinds
 * parent = vertex visited before each one during the search */
template<typename L>
struct DFSData
{
    std::size_t d_time = 0;
    std::size_t f_time = 0;
    L parent;
    constexpr DFSData()
    {
        if constexpr (std::is_same_v<L, std::string>) {
            parent = "__DFS_SEARCH_NO_PARENT__";
        } else {
            parent = std::numeric_limits<std::size_t>::max();
        }
    }
};

/* Provides the default weight for any arithmetic type. The user must
 * create a custom specialization and inject it in the bork_lib namespace
 * in order to use a user-defined weight function. The process is similar to
 * that of std::hash. */
template<typename W, typename Enable = void> struct default_edge_weight {};
template<typename W>
struct default_edge_weight<W, std::enable_if_t<std::is_arithmetic_v<W>>>
{
    W operator()() const noexcept { return static_cast<W>(1); }
};

/* Empty functions that can be passed into the bfs and dfs functions when
 * only the process_edge and/or process_vertex_late functions want to be
 * specified. */
template<typename L> static constexpr void(*empty_vertex_func)(L) = [](L){};
template<typename L, typename W>
static constexpr void(*empty_edge_func)(L, const std::pair<L, W>&) = [](L, const std::pair<L, W>&){};

template<typename AdjStructureType, typename V = size_t, typename W = int>
class Graph
{
private:
    using label_type = typename AdjStructureType::key_type;

    // halts compilation if the wrong function types are passed to bfs or dfs
    template<typename FV1 = void(*)(size_t), typename FE = void(*)(size_t, const std::pair<size_t, W>&), typename FV2 = void(*)(size_t)>
    using enable_search_funcs = std::enable_if_t<std::is_convertible_v<decltype(std::declval<FV1>()(std::declval<label_type>())), void>
         && std::is_convertible_v<decltype(std::declval<FE>()(std::declval<label_type>(), std::declval<const std::pair<label_type, W>&>())), void>
         && std::is_convertible_v<decltype(std::declval<FV2>()(std::declval<label_type>())), void>>;

public:
    // no manual memory management so compiler-generated functions are sufficient
    Graph(const Graph<AdjStructureType, V, W>&) = default;
    Graph(Graph<AdjStructureType, V, W>&&) noexcept = default;
    Graph<AdjStructureType, V, W>& operator=(const Graph<AdjStructureType, V, W>&) = default;
    Graph<AdjStructureType, V, W>& operator=(Graph<AdjStructureType, V, W>&&) noexcept = default;

    virtual void add_vertex(const std::vector<std::pair<label_type, W>>& outgoing_edges,
                            const std::vector<std::pair<label_type, W>>& incoming_edges,
                            const V& data, const std::string& label);
    void add_vertex() { add_vertex({}, {}, {}, {}); }
    void add_vertex(const V& data, const std::string& label = {}) { add_vertex({}, {}, data, label); }
    void add_vertex(const std::string label) { add_vertex({}, {}, {}, label); }
    void add_vertex(const std::vector<std::pair<label_type, W>>& outgoing_edges, const V& data = {},
                    const std::string& label = {}) { add_vertex(outgoing_edges, {}, data, label); }
    void add_vertex(const std::vector<std::pair<label_type, W>>& outgoing_edges,
                    const std::vector<std::pair<label_type, W>>& incoming_edges, const V &data = {})
    { add_vertex(outgoing_edges, incoming_edges, data, {}); }
    void add_vertex(const std::vector<std::pair<label_type, W>>& outgoing_edges, const std::string& label)
    { add_vertex(outgoing_edges, {}, {}, label); }
    void add_vertex(const std::vector<std::pair<label_type, W>>& outgoing_edges,
                    const std::vector<std::pair<label_type, W>>& incoming_edges, const std::string& label)
    { add_vertex(outgoing_edges, incoming_edges, {}, label); }

    virtual void remove_vertex(const label_type& label) = 0;
    virtual void add_edge(const label_type& orig, const label_type& dest, const W& weight) = 0;
    void add_edge(const label_type& orig, const label_type& dest) { add_edge(orig, dest, {}); }
    virtual void remove_edge(const label_type& orig, const label_type& dest) = 0;

    virtual std::unordered_map<label_type, W> neighbors(const label_type& vertex) const = 0;

    template<typename FV1 = void(*)(label_type), typename FE = void(*)(label_type, const std::pair<label_type, W>&),
            typename FV2 = void(*)(label_type), typename = enable_search_funcs<FV1, FE, FV2>>
    std::unordered_map<label_type, BFSData<label_type>> bfs(const label_type& start, const FV1& process_vertex_early = empty_vertex_func<label_type>,
            const FE& process_edge = empty_edge_func<label_type, W>, const FV2& process_vertex_late = empty_vertex_func<label_type>) const;

    template<typename FV1 = void(*)(label_type), typename FE = void(*)(label_type, const std::pair<label_type, W>&),
            typename FV2 = void(*)(label_type), typename = enable_search_funcs<FV1, FE, FV2>>
    std::unordered_map<label_type, DFSData<label_type>> dfs(const FV1& process_vertex_early = empty_vertex_func<label_type>,
            const FE& process_edge = empty_edge_func<label_type, W>, const FV2& process_vertex_late = empty_vertex_func<label_type>) const;

    Vertex<V>& operator[](const label_type& label);
    const Vertex<V>& operator[](const label_type& label) const;
    virtual void change_label(const label_type& label, const label_type& new_label) = 0;
    bool weighted() const noexcept { return is_weighted; }
    bool directed() const noexcept { return is_directed; }
    bool labeled() const noexcept { return is_labeled; }
    bool has_satellite_data() const noexcept { return satellite_data; }
    std::size_t size() const noexcept { return vertices.size(); }   // the number of vertices in the graph
    bool empty() const noexcept { return vertices.empty(); }        // is the graph empty?
    void clear() noexcept;

protected:
    std::unordered_map<label_type, Vertex<V>> vertices;         // the vertices and their associated data
    AdjStructureType adj_structure;                             // holds vertex neighbor data
    std::size_t current_key = 0;

    static const std::out_of_range invalid_label_exception;

    template<typename... Ts> void valid_label_check(Ts... labels) const;
    void check_edge_list(const std::vector<std::pair<label_type, W>>& edges);
    template <typename T> std::unordered_map<label_type, T> initialize_map_for_search(const T& default_value = {}) const;

    using iter_type = decltype(adj_structure[{}].cbegin());
    virtual iter_type first_neighbor(const label_type& label) const noexcept = 0;
    virtual iter_type next_neighbor(iter_type iter) const noexcept = 0;

    bool is_weighted;
    bool is_directed;
    bool is_labeled;
    bool satellite_data;

    Graph(bool is_weighted, bool is_directed, bool is_labeled, bool satellite_data)
            : is_weighted{is_weighted}, is_directed{is_directed}, is_labeled{is_labeled},
              satellite_data{satellite_data} {}


};

/* Adds a vertex to the graph. Overloads are available to make it possible to specify almost any
 * combination of input arguments. However, it is not possible to specify incoming_edges and not
 * outgoing_edges due to ambiguity. This function should be extended by any derived class. */
template<typename AdjStructureType, typename V, typename W>
void Graph<AdjStructureType, V, W>::add_vertex(const std::vector<std::pair<label_type, W>>& outgoing_edges,
                                               const std::vector<std::pair<label_type, W>>& incoming_edges,
                                               const V& data, const std::string& label)
{


    check_edge_list(outgoing_edges);
    check_edge_list(incoming_edges);
    Vertex<V> vertex{satellite_data, is_labeled, data, label};
    label_type actual_label;
    if constexpr (std::is_same_v<typename AdjStructureType::key_type, std::string>) {
        actual_label = label;
    } else {
        actual_label = current_key;
    }
    vertices.emplace(actual_label, vertex);
}

/* Breadth-first search implementation. Let L = std::size_t for an unlabeled graph and std::string for a labeled
 * graph. The user must specify the start vertex as a valid vertex of type L. Optional functions, including capturing
 * lambdas, can be provided to process vertices when they are first discovered (early), or after all their neighbors
 * have been discovered (late). Vertex process functions must return void and accept a single L argument. An
 * additional optional function can be provided to process an edge when an unprocessed neighbor is found in a
 * vertex's neighbor list. The edge process function must return void and accept an L as its first argument and a
 * reference to a const std::pair<L, W> as its second, where W is the type of the weight function (int by default).
 * In order to not provide a process function that comes before one that is provided in the argument list, the empty
 * function pointers, bork_lib::empty_vertex_func<L> and bork_lib::empty_edge<L, W> can be passed into the function. */
template<typename AdjStructureType, typename V, typename W>
template<typename FV1, typename FE, typename FV2, typename>
std::unordered_map<typename Graph<AdjStructureType, V, W>::label_type, BFSData<typename Graph<AdjStructureType, V, W>::label_type>>
Graph<AdjStructureType, V, W>::bfs(const label_type& start, const FV1& process_vertex_early, const FE& process_edge,
        const FV2& process_vertex_late) const
{
    valid_label_check(start);
    auto data = initialize_map_for_search<BFSData<label_type>>();
    data[start].distance = 0;
    auto status = initialize_map_for_search<SearchStatus>(SearchStatus::undiscovered);
    status[start] = SearchStatus::discovered;

    std::queue<label_type> bfs_queue;
    bfs_queue.push(start);
    while (!bfs_queue.empty()) {
        auto orig = bfs_queue.front();
        bfs_queue.pop();
        process_vertex_early(orig);
        status[orig] = SearchStatus::processed;

        auto neighbor_map = neighbors(orig);
        for (const auto& neighbor : neighbor_map) {
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

/* Fully iterative depth-first search implementation that visits the vertices in the same order as a recursive
 * implementation. Always starts the search at the first vertex in the adj_structure, which can vary if the
 * adj_structure is an unordered_map. Will always discover all vertices in the graph, even if it is not connected
 * because of the main loop. For information on the process functions, see the comment for the BFS function above.
 * See https://cs.stackexchange.com/questions/102130/process-functions-in-iterative-and-recursive-depth-first-search/102220
 * for information on this version of DFS. */
template<typename AdjStructureType, typename V, typename W>
template<typename FV1, typename FE, typename FV2, typename>
std::unordered_map<typename Graph<AdjStructureType, V, W>::label_type, DFSData<typename Graph<AdjStructureType, V, W>::label_type>>
Graph<AdjStructureType, V, W>::dfs(const FV1& process_vertex_early, const FE& process_edge,
                                   const FV2& process_vertex_late) const
{
    auto data = initialize_map_for_search<DFSData<label_type>>();
    auto start_finish_times = initialize_map_for_search<std::pair<std::size_t, std::size_t>>({0, 0});
    auto status = initialize_map_for_search<SearchStatus>(SearchStatus::undiscovered);
    std::size_t time = 0;

    for (const auto& [label, _] : adj_structure) {
        std::stack<std::pair<label_type, iter_type>> dfs_stack;
        if (status[label] == SearchStatus::undiscovered) {
            dfs_stack.push({label, first_neighbor(label)});
        }

        while (!dfs_stack.empty()) {
            auto& orig_pair = dfs_stack.top();
            auto [orig, orig_iter] = orig_pair;
            if (status[orig] == SearchStatus::undiscovered) {
                ++time;
                data[orig].d_time = time;
                status[orig] = SearchStatus::discovered;
                process_vertex_early(orig);
            }

            if (orig_iter != adj_structure.at(orig).end()) {
                auto dest = orig_iter->first;
                orig_pair = {orig, next_neighbor(orig_pair.second)};
                if (status[dest] == SearchStatus::undiscovered) {
                    data[dest].parent = orig;
                    process_edge(orig, *orig_iter);
                    dfs_stack.push({dest, first_neighbor(dest)});
                } else if (is_directed || (status[dest] != SearchStatus::processed)) {
                    process_edge(orig, *orig_iter);
                }
            } else {
                ++time;
                dfs_stack.pop();
                status[orig] = SearchStatus::processed;
                data[orig].f_time = time;
                process_vertex_late(orig);
            }
        }
    }

    return data;
}

/* Allows a Vertex object to be accessed via subscripting, const version. */
template<typename AdjStructureType, typename V, typename W>
const Vertex<V>& Graph<AdjStructureType, V, W>::operator[](const label_type& label) const
{
    try {
        return vertices.at(label);
    } catch (const std::out_of_range& e) {
        throw invalid_label_exception;
    }
}

/* Allows a Vertex object to be accessed via subscripting, non-const version. If the graph holds
 * satellite data, the data held in the Vertex object can be modified via this function. */
template<typename AdjStructureType, typename V, typename W>
Vertex<V>& Graph<AdjStructureType, V, W>::operator[](const label_type& label)
{
    return const_cast<Vertex<V> &>(static_cast<const Graph<AdjStructureType, V, W> &>(*this)[label]);
}

/* Removes all vertices and edges, resulting in an empty graph. */
template<typename AdjStructureType, typename V, typename W>
void Graph<AdjStructureType, V, W>::clear() noexcept
{
    vertices.clear();
    adj_structure.clear();
    current_key = 0;
}

/* Out_of_range exception that is used in multiple places. */
template<typename AdjStructureType, typename V, typename W>
const std::out_of_range Graph<AdjStructureType, V, W>::
    invalid_label_exception{"Vertex with given label does not exist in graph."};

/* Determines if a variadic number of labels exist by seeing if they are found in the vertices hash table. */
template<typename AdjStructureType, typename V, typename W>
template<typename... Ts>
void Graph<AdjStructureType, V, W>::valid_label_check(Ts... labels) const
{
    try {
        (vertices.at(labels), ...);
    } catch (const std::out_of_range& e) {
        throw invalid_label_exception;
    }
}

/* Checks a edge initializer list passed into the add_vertex function for validity. */
template<typename AdjStructureType, typename V, typename W>
void Graph<AdjStructureType, V, W>::check_edge_list(const std::vector<std::pair<label_type, W>>& edges)
{
    for (const auto& [neighbor, _] : edges) {
        try {
            adj_structure.at(neighbor);
        } catch (const std::out_of_range& e) {
            throw std::invalid_argument{"Invalid edge in initializer list."};
        }
    }
}

/* Initializes maps with default values for a depth-first search. */
template<typename AdjStructureType, typename V, typename W>
template<typename T>
std::unordered_map<typename Graph<AdjStructureType, V, W>::label_type, T>
        Graph<AdjStructureType, V, W>::initialize_map_for_search(const T& default_value) const
{
    std::unordered_map<label_type, T> map(vertices.size());
    for (const auto& vertex_pair : vertices) {
        map[vertex_pair.first] = default_value;
    }

    return map;
}

} // end namespace

#endif
