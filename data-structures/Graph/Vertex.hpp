#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <type_traits>
#include <stdexcept>

template<typename V, typename W>
class GraphAL;
template<typename V, typename W>
class GraphAM;

template<typename V = size_t>
class Vertex
{
private:
    size_t key;

public:
    V data;
    std::string label;
    bool labeled;

    Vertex(size_t key, const V &data = {}, const std::string &label = {},
           bool labeled = false, bool data_is_key = true)
           : key(key), data(data_is_key ? key : data),
             label(labeled ? label : ""), labeled(labeled)
    {
        if (data_is_key && !std::is_same<V, size_t>::value)
            throw std::invalid_argument("Data can't equal key if type is not size_t");
    }

    template<typename T, typename W>
    friend class GraphAL;
    template<typename T, typename W>
    friend class GraphAM;
};

#endif
