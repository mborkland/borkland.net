#include "benchmark.hpp"
#include "../src/quicksort-random.hpp"

using namespace bork_lib;

int main()
{
    benchmark(quicksort_random<iter_type>, 1000, 100000000, 10);
}