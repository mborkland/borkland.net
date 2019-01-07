#ifndef GRAPH_BUILDER_HPP
#define GRAPH_BUILDER_HPP

#include <type_traits>
#include "GraphAL.hpp"
#include "GraphAM.hpp"

namespace bork_lib
{

template<typename L = std::size_t, typename V = std::size_t, typename W = int>
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
    GraphBuilder(const GraphBuilder<L, V, W>&) = default;
    GraphBuilder(GraphBuilder<L, V, W>&&) noexcept = default;
    GraphBuilder<L, V, W>& operator=(const GraphBuilder<L, V, W>&) = default;
    GraphBuilder<L, V, W>& operator=(GraphBuilder<L, V, W>&&) noexcept = default;

    GraphBuilder<L, V, W>& weighted();
    GraphBuilder<L, V, W>& directed();
    GraphBuilder<L, V, W>& use_satellite_data();

    GraphAL<L, V, W> build_adj_list();
    GraphAM<L, V, W> build_adj_matrix();
};

template<typename L, typename V, typename W>
void GraphBuilder<L, V, W>::validation_check()
{
    if (!satellite_data && !std::is_same<V, std::size_t>::value) {
        throw std::invalid_argument("Graph with no satellite data must have vertex of type std::size_t.");
    }

    if (!is_weighted && !std::is_same<W, int>::value) {
        throw std::invalid_argument("Unweighted graph must have weight of type int.");
    }
}

template<typename L, typename V, typename W>
GraphBuilder<L, V, W>& GraphBuilder<L, V, W>::weighted()
{
    is_weighted = true;
    return *this;
}

template<typename L, typename V, typename W>
GraphBuilder<L, V, W>& GraphBuilder<L, V, W>::directed()
{
    is_directed = true;
    return *this;
}

template<typename L, typename V, typename W>
GraphBuilder<L, V, W>& GraphBuilder<L, V, W>::use_satellite_data()
{
    satellite_data = true;
    return *this;
}

template<typename L, typename V, typename W>
GraphAL<L, V, W> GraphBuilder<L, V, W>::build_adj_list()
{
    validation_check();
    return GraphAL<L, V, W>{is_weighted, is_directed, satellite_data, init_capacity};
}

template<typename L, typename V, typename W>
GraphAM<L, V, W> GraphBuilder<L, V, W>::build_adj_matrix()
{
    validation_check();
    return GraphAM<L, V, W>{is_weighted, is_directed, satellite_data, init_capacity};
}

template<typename V = std::size_t, typename W = int>
using BasicGraphBuilder = GraphBuilder<std::size_t, V, W>;

template<typename V = std::size_t, typename W = int>
using LabeledGraphBuilder = GraphBuilder<std::string, V, W>;

} // end namespace

#endif
