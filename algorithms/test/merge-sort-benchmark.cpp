#include "benchmark.hpp"
#include "../src/merge-sort.hpp"

using namespace bork_lib;

int main()
{
    benchmark(merge_sort<iter_type>, 1000, 100000000, 10);
}