#ifndef GRAPH_BUILDER_HPP
#define GRAPH_BUILDER_HPP

#include "GraphAL.hpp"
#include "GraphAM.hpp"

template<typename V = size_t, typename W = int>
class GraphBuilder
{
private:
    bool is_weighted = false;
    bool is_directed = false;
    bool is_labeled = false;
    bool data_is_key = true;

public:
    GraphBuilder() = default;
    GraphBuilder(const GraphBuilder&) = default;
    GraphBuilder(GraphBuilder&&) = default;
    GraphBuilder& operator=(const GraphBuilder&) = default;
    GraphBuilder& operator=(GraphBuilder&&) = default;
    GraphBuilder weighted()
    {
        is_weighted = true;
        return *this;
    }
    GraphBuilder directed()
    {
        is_directed = true;
        return *this;
    }
    GraphBuilder labeled()
    {
        is_labeled = true;
        return *this;
    }
    GraphBuilder use_satellite_data()
    {
        data_is_key = false;
        return *this;
    }
    GraphAL<V,W> build_with_adj_list()
    {
        return GraphAL<V,W>{is_weighted, is_directed, is_labeled, data_is_key};
    }
    GraphAM<V,W> build_with_adj_matrix()
    {
        return GraphAM<V,W>{is_weighted, is_directed, is_labeled, data_is_key};
    }
};

#endif
