#ifndef GRAPH_BUILDER_HPP
#define GRAPH_BUILDER_HPP

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
    bool is_labeled = false;
    bool data_is_key = true;
    void validation_check();

public:
    GraphBuilder() = default;
    GraphBuilder(const GraphBuilder<V, W>&) = default;
    GraphBuilder(GraphBuilder<V, W>&&) noexcept = default;
    GraphBuilder<V, W>& operator=(const GraphBuilder<V, W>&) = default;
    GraphBuilder<V, W>& operator=(GraphBuilder<V, W>&&) noexcept = default;

    GraphBuilder<V, W>& weighted();
    GraphBuilder<V, W>& directed();
    GraphBuilder<V, W>& labeled();
    GraphBuilder<V, W>& use_satellite_data();

    GraphAL<V, W> build_adj_list();
    GraphAM<V, W> build_adj_matrix();
};

template<typename V, typename W>
void GraphBuilder<V, W>::validation_check()
{
    if (data_is_key && !std::is_same<V, size_t>::value) {
        throw std::invalid_argument("Data can't equal key if type is not std::size_t.");
    }

    if (!std::is_same<W, int>::value) {
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
GraphBuilder<V, W>& GraphBuilder<V, W>::labeled()
{
    is_labeled = true;
    return *this;
}

template<typename V, typename W>
GraphBuilder<V, W>& GraphBuilder<V, W>::use_satellite_data()
{
    data_is_key = false;
    return *this;
}

template<typename V, typename W>
GraphAL<V, W> GraphBuilder<V, W>::build_adj_list()
{
    validation_check();
    return GraphAL<V, W>{is_weighted, is_directed, is_labeled, data_is_key};
}

template<typename V, typename W>
GraphAM<V, W> GraphBuilder<V, W>::build_adj_matrix()
{
    validation_check();
    return GraphAM<V, W>{is_weighted, is_directed, is_labeled, data_is_key};
}

} // end namespace

#endif
