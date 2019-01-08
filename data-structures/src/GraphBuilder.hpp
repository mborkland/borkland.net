#ifndef GRAPH_BUILDER_HPP
#define GRAPH_BUILDER_HPP

#include <type_traits>
#include "GraphAL.hpp"
#include "GraphAM.hpp"

namespace bork_lib
{

/* Template parameters:
 * L = label type
 * V = vertex type
 * W = weight type */
template<typename L = std::size_t, typename W = int, typename V = std::size_t>
class GraphBuilder
{
private:
    static constexpr std::size_t default_capacity = 8;
    std::size_t init_capacity;
    bool is_weighted = false;
    bool is_directed = false;
    bool satellite_data = false;
    void validation_check();

public:
    explicit GraphBuilder(std::size_t init_capacity = default_capacity) : init_capacity{init_capacity} {}
    GraphBuilder(const GraphBuilder<L, W, V>&) = default;
    GraphBuilder(GraphBuilder<L, W, V>&&) noexcept = default;
    GraphBuilder<L, W, V>& operator=(const GraphBuilder<L, W, V>&) = default;
    GraphBuilder<L, W, V>& operator=(GraphBuilder<L, W, V>&&) noexcept = default;

    GraphBuilder<L, W, V>& weighted();
    GraphBuilder<L, W, V>& directed();
    GraphBuilder<L, W, V>& use_satellite_data();

    GraphAL<L, W, V> build_adj_list();
    GraphAM<L, W, V> build_adj_matrix();
};

/* Validates the template argument types and options chosen for graph. */
template<typename L, typename W, typename V>
void GraphBuilder<L, W, V>::validation_check()
{
    if (!satellite_data && !std::is_same<V, std::size_t>::value) {
        throw std::invalid_argument("Graph with no satellite data must have vertex of type std::size_t.");
    }

    if (!is_weighted && !std::is_same<W, int>::value) {
        throw std::invalid_argument("Unweighted graph must have weight of type int.");
    }
}

template<typename L, typename W, typename V>
GraphBuilder<L, W, V>& GraphBuilder<L, W, V>::weighted()
{
    is_weighted = true;
    return *this;
}

template<typename L, typename W, typename V>
GraphBuilder<L, W, V>& GraphBuilder<L, W, V>::directed()
{
    is_directed = true;
    return *this;
}

template<typename L, typename W, typename V>
GraphBuilder<L, W, V>& GraphBuilder<L, W, V>::use_satellite_data()
{
    satellite_data = true;
    return *this;
}

template<typename L, typename W, typename V>
GraphAL<L, W, V> GraphBuilder<L, W, V>::build_adj_list()
{
    validation_check();
    return GraphAL<L, W, V>{is_weighted, is_directed, satellite_data, init_capacity};
}

template<typename L, typename W, typename V>
GraphAM<L, W, V> GraphBuilder<L, W, V>::build_adj_matrix()
{
    validation_check();
    return GraphAM<L, W, V>{is_weighted, is_directed, satellite_data, init_capacity};
}

/* These aliases should be used instead of naming the L parameter of the graph explicitly. */
template<typename W = int, typename V = std::size_t>
using BasicGraphBuilder = GraphBuilder<std::size_t, W, V>;

template<typename W = int, typename V = std::size_t>
using LabeledGraphBuilder = GraphBuilder<std::string, W, V>;

} // end namespace

#endif
