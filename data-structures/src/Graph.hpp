#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <limits>
#include <optional>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <typeinfo>
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
 * parent = vertex visited before each one during the search
 * The parent value for a vertex with no parent will be
 * std::numeric_limits<std::size_t>::max() for an unlabeled
 * graph and the string "__BFS_SEARCH_NO_PARENT__" for a labeled
 * graph. */
template<typename L>
struct BFSData
{
    std::size_t distance = std::numeric_limits<std::size_t>::max();
    L parent;
    constexpr BFSData()
    {
        if constexpr (std::is_same_v<L, std::string>) {
            parent = "__BFS_SEARCH_NO_PARENT__";
        } else {
            parent = std::numeric_limits<std::size_t>::max();
        }
    }
};

/* The data returned for each vertex after a depth-first search.
 * d_time = time step when the vertex is discovered in the search
 * f_time = time step when the vertex is accessed again after the stack unwinds
 * parent = vertex visited before each one during the search
 * The parent value for a vertex with no parent will be
 * std::numeric_limits<std::size_t>::max() for an unlabeled
 * graph and the string "__DFS_SEARCH_NO_PARENT__" for a labeled
 * graph. */
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
 * create a custom specialization and inject it into the bork_lib namespace
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
template<typename L> static constexpr void(*empty_vertex_func)(const L&) = [](const L&){};
template<typename L, typename W>
static constexpr void(*empty_edge_func)(const L&, const std::pair<L, W>&) = [](const L&, const std::pair<L, W>&){};

/* Template parameters:
 * AdjStructureType = the type a derived class uses to hold adjacency data
 * L = label type
 * V = vertex type
 * W = weight type */
template<typename AdjStructureType, typename L = std::size_t, typename W = int, typename V = std::size_t>
class Graph
{
private:
    // halts compilation if the wrong function types are passed to bfs or dfs
    template<typename FV1 = void(*)(const L&), typename FE = void(*)(const L&, const std::pair<size_t, W>&), typename FV2 = void(*)(const L&)>
    using enable_search_funcs = std::enable_if_t<
            std::is_convertible_v<decltype(std::declval<FV1>()(std::declval<L>())), void>
         && std::is_convertible_v<decltype(std::declval<FE>()(std::declval<L>(),
                 std::declval<const std::pair<L, W>&>())), void>
         && std::is_convertible_v<decltype(std::declval<FV2>()(std::declval<L>())), void>>;

public:
    virtual ~Graph() = default;

    virtual void add_vertex(const std::vector<std::pair<L, W>>& outgoing_edges,
                            const std::vector<std::pair<L, W>>& incoming_edges,
                            const V& data, const std::string& label);
    void add_vertex() { add_vertex({}, {}, {}, {}); }
    void add_vertex(const V& data, const std::string& label = {}) { add_vertex({}, {}, data, label); }
    void add_vertex(const std::string label) { add_vertex({}, {}, {}, label); }
    void add_vertex(const std::vector<std::pair<L, W>>& outgoing_edges, const V& data = {},
                    const std::string& label = {}) { add_vertex(outgoing_edges, {}, data, label); }
    void add_vertex(const std::vector<std::pair<L, W>>& outgoing_edges,
                    const std::vector<std::pair<L, W>>& incoming_edges, const V &data = {})
                    { add_vertex(outgoing_edges, incoming_edges, data, {}); }
    void add_vertex(const std::vector<std::pair<L, W>>& outgoing_edges, const std::string& label)
                    { add_vertex(outgoing_edges, {}, {}, label); }
    void add_vertex(const std::vector<std::pair<L, W>>& outgoing_edges,
                    const std::vector<std::pair<L, W>>& incoming_edges, const std::string& label)
                    { add_vertex(outgoing_edges, incoming_edges, {}, label); }

    void remove_vertex(const L& label);
    virtual void add_edge(const L& orig, const L& dest, const W& weight) = 0;
    void add_edge(const L& orig, const L& dest) { add_edge(orig, dest, {}); }
    virtual void remove_edge(const L& orig, const L& dest) = 0;

    virtual std::optional<W> edge_weight(const L& orig, const L& dest) const noexcept = 0;
    virtual std::unordered_map<L, W> neighbors(const L& vertex) const = 0;

    template<typename FV1 = void(*)(const L&), typename FE = void(*)(const L&, const std::pair<L, W>&),
            typename FV2 = void(*)(const L&), typename = enable_search_funcs<FV1, FE, FV2>>
    std::unordered_map<L, BFSData<L>> bfs(const L& start,
            const FV1& process_vertex_early = empty_vertex_func<L>, const FE& process_edge = empty_edge_func<L, W>,
                    const FV2& process_vertex_late = empty_vertex_func<L>) const;

    template<typename FV1 = void(*)(const L&), typename FE = void(*)(const L&, const std::pair<L, W>&),
            typename FV2 = void(*)(const L&), typename = enable_search_funcs<FV1, FE, FV2>>
    std::unordered_map<L, DFSData<L>> dfs(const L& start,
            const FV1& process_vertex_early = empty_vertex_func<L>, const FE& process_edge = empty_edge_func<L, W>,
                    const FV2& process_vertex_late = empty_vertex_func<L>) const;

    const Vertex<V>& operator[](const L& label) const;
    Vertex<V>& operator[](const L& label);
    virtual void change_label(const L& label, const L& new_label) = 0;
    bool weighted() const noexcept { return is_weighted; }
    bool directed() const noexcept { return is_directed; }
    bool labeled() const noexcept { return is_labeled; }
    bool has_satellite_data() const noexcept { return satellite_data; }
    static constexpr std::size_t min_capacity() noexcept { return min_graph_capacity; }
    std::size_t capacity() const noexcept { return graph_capacity; }
    std::size_t size() const noexcept { return vertices.size(); }   // the number of vertices in the graph
    bool empty() const noexcept { return vertices.empty(); }        // is the graph empty?
    virtual void reserve(std::size_t new_capacity) = 0;
    virtual void clear() noexcept;

protected:
    // no manual memory management so compiler-generated functions are sufficient
    Graph(const Graph<AdjStructureType, L, W, V>&) = default;
    Graph(Graph<AdjStructureType, L, W, V>&&) = default;
    Graph<AdjStructureType, L, W, V>& operator=(const Graph<AdjStructureType, L, W, V>&) = default;
    Graph<AdjStructureType, L, W, V>& operator=(Graph<AdjStructureType, L, W, V>&&) = default;

    std::unordered_map<L, Vertex<V>> vertices;         // the vertices and their associated data
    AdjStructureType adj_structure;                             // holds vertex neighbor data
    std::size_t current_key = 0;

    static const std::string invalid_label_exception;
    static const std::string invalid_edge_exception;
    static const std::string invalid_vertex_exception;
    static const std::string change_label_exception;
    static const std::string duplicate_label_exception;
    static const std::string repeat_edge_exception;

    typename std::unordered_map<L, Vertex<V>>::const_iterator validate_label(const L &label) const;
    virtual void check_edge_list(const std::vector<std::pair<L, W>>& edges) = 0;
    template <typename T> std::unordered_map<L, T> initialize_map_for_search(const T& default_value = {}) const;

    using iter_type = typename std::unordered_map<L, W>::const_iterator;
    virtual iter_type first_neighbor(const L& label) const noexcept = 0;
    iter_type next_neighbor(iter_type iter) const noexcept { return ++iter; }
    virtual iter_type neighbors_end(const L& label) const noexcept = 0;

    virtual void remove_string_vertex(const std::string& label) = 0;
    virtual void remove_numeric_vertex(std::size_t key) = 0;
    void shift_vertices(std::size_t removed_key);

    bool is_weighted;
    bool is_directed;
    static constexpr bool is_labeled = std::is_same_v<L, std::string>;
    bool satellite_data;
    static constexpr std::size_t min_graph_capacity = 8;
    std::size_t graph_capacity;

    Graph(bool is_weighted, bool is_directed, bool satellite_data, std::size_t init_capacity)
            : is_weighted{is_weighted}, is_directed{is_directed}, satellite_data{satellite_data},
              graph_capacity{std::max(init_capacity, min_graph_capacity)} { vertices.reserve(graph_capacity); }
};

/* Adds a vertex to the graph. Overloads are available to make it possible to specify almost any
 * combination of input arguments. However, it is not possible to specify incoming_edges and not
 * outgoing_edges due to ambiguity. This function should be extended by any derived class, as it
 * only checks the validity of the edge lists and adds the vertex to the vertices data structure.
 * Derived classes are responsible for changes that needs to be made to their adjacency structures. */
template<typename AdjStructureType, typename L, typename W, typename V>
void Graph<AdjStructureType, L, W, V>::add_vertex(const std::vector<std::pair<L, W>>& outgoing_edges,
                                               const std::vector<std::pair<L, W>>& incoming_edges,
                                               const V& data, const std::string& label)
{
    check_edge_list(outgoing_edges);
    check_edge_list(incoming_edges);
    Vertex<V> vertex{satellite_data, is_labeled, data, label};
    L actual_label;
    if constexpr (is_labeled) {
        actual_label = label;
    } else {
        actual_label = current_key;
    }
    vertices.emplace(actual_label, vertex);
}

/* Removes a vertex from the graph. Delegates to other functions that are
 * defined by the derived classes. Which function to delegate to depends
 * on if the graph is labeled or not. */
template<typename AdjStructureType, typename L, typename W, typename V>
void Graph<AdjStructureType, L, W, V>::remove_vertex(const L& label)
{
    validate_label(label);
    if constexpr (is_labeled) {
        remove_string_vertex(label);
    } else {
        remove_numeric_vertex(label);
    }
    --current_key;
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
template<typename AdjStructureType, typename L, typename W, typename V>
template<typename FV1, typename FE, typename FV2, typename>
std::unordered_map<L, BFSData<L>> Graph<AdjStructureType, L, W, V>::bfs(const L& start, const FV1& process_vertex_early,
        const FE& process_edge, const FV2& process_vertex_late) const
{
    validate_label(start);
    auto data = initialize_map_for_search<BFSData<L>>();
    data[start].distance = 0;
    auto status = initialize_map_for_search<SearchStatus>(SearchStatus::undiscovered);
    status[start] = SearchStatus::discovered;

    std::queue<L> bfs_queue;
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
 * because of the main loop. For information on the process functions, see the comment for the BFS function above. See
 * https://cs.stackexchange.com/questions/102130/process-functions-in-iterative-and-recursive-depth-first-search/102220
 * for information on this version of DFS. */
template<typename AdjStructureType, typename L, typename W, typename V>
template<typename FV1, typename FE, typename FV2, typename>
std::unordered_map<L, DFSData<L>> Graph<AdjStructureType, L, W, V>::dfs(const L& start,
        const FV1& process_vertex_early, const FE& process_edge, const FV2& process_vertex_late) const
{
    auto data = initialize_map_for_search<DFSData<L>>();
    auto start_finish_times = initialize_map_for_search<std::pair<std::size_t, std::size_t>>({0, 0});
    auto status = initialize_map_for_search<SearchStatus>(SearchStatus::undiscovered);
    std::stack<std::pair<L, iter_type>> dfs_stack;
    dfs_stack.push({start, first_neighbor(start)});
    std::size_t time = 0;

    while (!dfs_stack.empty()) {
        auto& orig_pair = dfs_stack.top();
        auto [orig, orig_iter] = orig_pair;
        if (status[orig] == SearchStatus::undiscovered) {
            ++time;
            data[orig].d_time = time;
            status[orig] = SearchStatus::discovered;
            process_vertex_early(orig);
        }

        if (orig_iter != neighbors_end(orig)) {
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

    return data;
}

/* Allows a Vertex object to be accessed via subscripting, const version. */
template<typename AdjStructureType, typename L, typename W, typename V>
const Vertex<V>& Graph<AdjStructureType, L, W, V>::operator[](const L& label) const
{
    return validate_label(label)->second;
}

/* Allows a Vertex object to be accessed via subscripting, non-const version. If the graph holds
 * satellite data, the data held in the Vertex object can be modified via this function. */
template<typename AdjStructureType, typename L, typename W, typename V>
Vertex<V>& Graph<AdjStructureType, L, W, V>::operator[](const L& label)
{
    return const_cast<Vertex<V> &>(static_cast<const Graph<AdjStructureType, L, W, V>&>(*this)[label]);
}

/* Removes all vertices and edges, resulting in an empty graph. Can be
 * extended by a derived class to clear any additional data structures
 * used. */
template<typename AdjStructureType, typename L, typename W, typename V>
void Graph<AdjStructureType, L, W, V>::clear() noexcept
{
    vertices.clear();
    adj_structure.clear();
    current_key = 0;
    graph_capacity = 0;
    reserve(min_graph_capacity);
}

/* Out_of_range exceptions that are used in multiple places. */
template<typename AdjStructureType, typename L, typename W, typename V>
const std::string Graph<AdjStructureType, L, W, V>::
    invalid_label_exception{"Vertex with given label does not exist in graph."};

template<typename AdjStructureType, typename L, typename W, typename V>
const std::string Graph<AdjStructureType, L, W, V>::
    invalid_edge_exception{"Edge does not exist in graph."};

template<typename AdjStructureType, typename L, typename W, typename V>
const std::string Graph<AdjStructureType, L, W, V>::
        invalid_vertex_exception{"Vertex does not exist in graph."};

template<typename AdjStructureType, typename L, typename W, typename V>
const std::string Graph<AdjStructureType, L, W, V>::
    change_label_exception{"Label can only be changed for labeled graphs."};

template<typename AdjStructureType, typename L, typename W, typename V>
const std::string Graph<AdjStructureType, L, W, V>::
    duplicate_label_exception{"Label already exists in graph."};

template<typename AdjStructureType, typename L, typename W, typename V>
const std::string Graph<AdjStructureType, L, W, V>::
    repeat_edge_exception{"Repeat edge in initializer list."};

/* Searches for a label in the vertices hash table. Returns an iterator to it if it exists and
 * throws an exception if it doesn't. */
template<typename AdjStructureType, typename L, typename W, typename V>
typename std::unordered_map<L, Vertex<V>>::const_iterator
        Graph<AdjStructureType, L, W, V>::validate_label(const L &label) const
{
    auto it = vertices.find(label);
    if (it != vertices.end()) {
        return it;
    } else {
        throw std::out_of_range{invalid_label_exception};
    }
}

/* Initializes maps with default values for a depth-first search. */
template<typename AdjStructureType, typename L, typename W, typename V>
template<typename T>
std::unordered_map<L, T> Graph<AdjStructureType, L, W, V>::initialize_map_for_search(const T& default_value) const
{
    std::unordered_map<L, T> map(vertices.size());
    for (const auto& vertex_pair : vertices) {
        map[vertex_pair.first] = default_value;
    }

    return map;
}

/* Creates a new vertices map after a vertex is removed in an unlabeled graph. */
template<typename AdjStructureType, typename L, typename W, typename V>
void Graph<AdjStructureType, L, W, V>::shift_vertices(std::size_t removed_key)
{
    std::unordered_map<L, Vertex<V>> new_vertices;
    std::for_each(vertices.begin(), vertices.end(), [&](const auto& vertex_pair){
        const auto& [old_key, vertex] = vertex_pair;
        if (old_key != removed_key) {
            auto new_key = old_key > removed_key ? old_key - 1 : old_key;
            new_vertices.emplace(new_key, std::move(vertex));
        }
    });

    vertices = new_vertices;
}

} // end namespace

#endif
