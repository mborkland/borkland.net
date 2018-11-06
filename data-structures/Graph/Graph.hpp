#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <limits>
#include <string>
#include <vector>
#include "Vertex.hpp"

enum class SearchStatus
{
    undiscovered, discovered, processed
};

struct BFSData
{
    std::size_t distance = std::numeric_limits<std::size_t>::max();
    std::size_t parent = std::numeric_limits<std::size_t>::max();
};

struct DFSData
{
    size_t d_time;
    size_t f_time;
    std::size_t parent = std::numeric_limits<std::size_t>::max();
};

template<typename AdjStructureType, typename V = size_t, typename W = int>
class Graph
{
private:
    std::vector<Vertex<V>> vertices;         // the vertices and their associated data
    AdjStructureType adj_structure;          // holds vertex neighbor data
    size_t current_key = 0;
    size_t highest_key() { return  adj_structure.size() - 1; }
    virtual AdjStructureType neighbors(std::size_t vertex) = 0;
    bool is_weighted;
    bool is_directed;
    bool is_labeled;
    bool data_is_key;
    Graph(bool is_weighted, bool is_directed, bool is_labeled, bool data_is_key)
          : is_weighted{is_weighted}, is_directed{is_directed}, is_labeled{is_labeled},
            data_is_key{data_is_key} {}

public:
    // TODO: add_vertex functions
    virtual void remove_vertex(std::size_t key) = 0;
    virtual void remove_vertex(const std::string& label) = 0;
    virtual void add_edge(size_t orig, size_t dest, const W& weight = {}) = 0;



    // testing function
    void print_adj_structure() = 0;
};

#endif
