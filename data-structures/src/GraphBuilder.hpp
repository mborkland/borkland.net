#ifndef GRAPH_BUILDER_HPP
#define GRAPH_BUILDER_HPP

#include <type_traits>
#include "GraphAL.hpp"
#include "GraphAM.hpp"

namespace bork_lib
{

template<typename V = std::size_t, typename W = int>
class GraphBuilder
{
private:
    bool is_weighted = false;
    bool is_directed = false;
    bool satellite_data = false;
    void validation_check();

public:
    GraphBuilder() = default;
    GraphBuilder(const GraphBuilder<V, W>&) = default;
    GraphBuilder(GraphBuilder<V, W>&&) noexcept = default;
    GraphBuilder<V, W>& operator=(const GraphBuilder<V, W>&) = default;
    GraphBuilder<V, W>& operator=(GraphBuilder<V, W>&&) noexcept = default;

    GraphBuilder<V, W>& weighted();
    GraphBuilder<V, W>& directed();
    GraphBuilder<V, W>& use_satellite_data();

    GraphAL<std::string, V, W> build_adj_list_labeled();
    GraphAL<std::size_t, V, W> build_adj_list();
    GraphAM<V, W> build_adj_matrix();
};

template<typename V, typename W>
void GraphBuilder<V, W>::validation_check()
{
    if (!satellite_data && !std::is_same<V, std::size_t>::value) {
        throw std::invalid_argument("Graph with no satellite data must have vertex of type std::size_t.");
    }

    if (!is_weighted && !std::is_same<W, int>::value) {
        throw std::invalid_argument("Unweighted graph must have weight of type int.");
    }
}

template<typename V, typename W>
GraphBuilder<V, W>& GraphBuilder<V, W>::weighted()
{
    is_weighted = true;
    return *this;
}

template<typename V, typename W>
GraphBuilder<V, W>& GraphBuilder<V, W>::directed()
{
    is_directed = true;
    return *this;
}

template<typename V, typename W>
GraphBuilder<V, W>& GraphBuilder<V, W>::use_satellite_data()
{
    satellite_data = true;
    return *this;
}

template<typename V, typename W>
GraphAL<std::string, V, W> GraphBuilder<V, W>::build_adj_list_labeled()
{
    validation_check();
    return GraphAL<std::string, V, W>{is_weighted, is_directed, true, satellite_data};
}

template<typename V, typename W>
GraphAL<std::size_t, V, W> GraphBuilder<V, W>::build_adj_list()
{
    validation_check();
    return GraphAL<std::size_t, V, W>{is_weighted, is_directed, false, satellite_data};
}

template<typename V, typename W>
GraphAM<V, W> GraphBuilder<V, W>::build_adj_matrix()
{
    validation_check();
    return GraphAM<V, W>{is_weighted, is_directed, false, satellite_data};
}

} // end namespace

#endif
