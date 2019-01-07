#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <cstddef>
#include <stdexcept>

namespace bork_lib
{

template<typename L, typename V, typename W> class GraphAL;
template<typename L, typename V, typename W> class GraphAM;

template<typename V = size_t>
class Vertex
{
private:
    bool satellite_data;
    bool labeled;
    V vertex_data;
    std::string vertex_label;

public:
    Vertex(bool satellite_data, bool labeled, V data = {}, std::string label = {})
      : satellite_data{satellite_data}, labeled{labeled}, vertex_data{std::move(data)},
      vertex_label{labeled ? std::move(label) : ""} {}

    const V& data() const
    {
        if (!satellite_data) {
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

    template<typename A, typename B, typename C> friend class GraphAL;
    template<typename A, typename B, typename C> friend class GraphAM;
};

} // end namespace

#endif
