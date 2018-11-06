#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <type_traits>
#include <stdexcept>

template<typename A, typename V, typename W> class Graph;
template<typename V, typename W> class GraphAL;
template<typename V, typename W> class GraphAM;

template<typename V = size_t>
class Vertex
{
private:
    std::size_t key;
    bool is_labeled;

public:
    V data;
    std::string label;

    Vertex(size_t key, bool is_labeled = false, bool data_is_key = true, V data = {}, const std::string& label = {})
           : key{key}, is_labeled{is_labeled}, data{data_is_key ? key : data}, label{labeled ? label : ""}
    {
        if (data_is_key && !std::is_same<V, size_t>::value)
            throw std::invalid_argument("Data can't equal key if type is not size_t");
    }

    bool labeled() { return is_labeled; }

    template<typename A, typename T, typename W> friend class Graph;
    template<typename T, typename W> friend class GraphAL;
    template<typename T, typename W> friend class GraphAM;
};

#endif
