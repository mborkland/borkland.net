#include "benchmark.hpp"
#include "../src/quicksort-lomuto.hpp"

using namespace bork_lib;

int main()
{
    benchmark(quicksort_lomuto<iter_type>, 1000, 100000000, 10);
}