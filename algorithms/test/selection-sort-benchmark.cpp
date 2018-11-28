#include "benchmark.hpp"
#include "../src/selection-sort.hpp"

using namespace bork_lib;

int main()
{
    benchmark(selection_sort<iter_type>, 1000, 100000, 10);
}