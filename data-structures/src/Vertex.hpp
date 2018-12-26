#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <cstddef>
#include <stdexcept>

namespace bork_lib
{

template<typename A, typename V, typename W> class Graph;
template<typename V, typename W> class GraphAL;
template<typename V, typename W> class GraphAM;

template<typename V = size_t>
class Vertex
{
private:
    bool data_is_key;
    bool labeled;
    std::size_t key;
    V vertex_data;
    std::string vertex_label;

public:
    Vertex(bool data_is_key, bool labeled, std::size_t key, V data = {}, std::string label = {})
      : data_is_key{data_is_key}, labeled{labeled}, key{key}, vertex_data{std::move(data)}, vertex_label{std::move(label)} {}

    const V& data() const
    {
        if (data_is_key) {
            throw std::logic_error("Vertex of graph without satellite data contains no data.");
        }
        return vertex_data;
    }

    V& data() { return const_cast<V&>(static_cast<const Vertex&>(*this).data()); }

    const std::string& label() const
    {
        if (!labeled) {
            throw std::logic_error("Label does not exist for unlabeled graph.");
        }

        return vertex_label;
    }

    std::string& label() { return const_cast<std::string&>(static_cast<const Vertex&>(*this).label()); }
};

} // end namespace

#endif
