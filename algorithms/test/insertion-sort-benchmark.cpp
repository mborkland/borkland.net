#include "benchmark.hpp"
#include "../src/insertion-sort.hpp"

using namespace bork_lib;

int main()
{
    benchmark(insertion_sort<iter_type>, 1000, 100000, 10);
}