#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <vector>

namespace bork_lib
{

using iter_type = std::vector<int>::iterator;

void benchmark(std::function<void(iter_type, iter_type)> func, int min, int max, int step)
{
    std::default_random_engine re {};
    std::vector<int> vec;
    std::map<int, double> times;

    for(auto i = min; i <= max; i *= step) {
        std::uniform_int_distribution<> dist{-i, i};
        for (int j = 0; j < i; ++j) {
            vec.push_back(dist(re));
        }

        auto start = std::chrono::high_resolution_clock::now();
        func(vec.begin(), vec.end());
        auto stop = std::chrono::high_resolution_clock::now();

        if (!std::is_sorted(vec.begin(), vec.end())) {
            throw std::runtime_error("Vector not properly sorted.");
        }
        std::chrono::duration<double> time = stop - start;
        times[i] = time.count();
        vec.clear();
    }

    std::cout << "Time to sort:\n";
    for (const auto& x : times) {
        std::cout << x.first << " elements - " << x.second << " sec\n";
    }
}

} // end namespace