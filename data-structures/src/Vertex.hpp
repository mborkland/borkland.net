#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <cstddef>
#include <stdexcept>

namespace bork_lib
{

template<typename A, typename B, typename C> class Graph;

template<typename V = size_t>
class Vertex
{
private:
    bool data_is_key;
    bool labeled;
    V vertex_data;
    std::string vertex_label;

public:
    Vertex(bool data_is_key, bool labeled, V data = {}, std::string label = {})
      : data_is_key{data_is_key}, labeled{labeled}, vertex_data{std::move(data)},
      vertex_label{labeled ? std::move(label) : ""} {}

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
            throw std::logic_error("Label does not exist for unlabeled vertex.");
        }
        return vertex_label;
    }

    template<typename A, typename B, typename C> friend class Graph;
};

} // end namespace

#endif
