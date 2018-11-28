#include "benchmark.hpp"
#include "../src/heapsort.hpp"

using namespace bork_lib;

int main()
{
    benchmark(heapsort<iter_type>, 1000, 100000000, 10);
}