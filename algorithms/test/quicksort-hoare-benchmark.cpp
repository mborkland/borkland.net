#include "benchmark.hpp"
#include "../src/quicksort-hoare.hpp"

using namespace bork_lib;

int main()
{
    benchmark(quicksort_hoare<iter_type>, 1000, 100000000, 10);
}