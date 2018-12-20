#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <random>
#include "../../catch/catch.hpp"
#include "../src/PriorityQueue.hpp"

using bork_lib::PriorityQueue;
using bork_lib::HeapType;

std::random_device rd{};

PriorityQueue<int> create_priority_queue(std::size_t num_insertions, HeapType type = HeapType::max)
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> dist{1, static_cast<int>(num_insertions)};
    PriorityQueue<int> pq{num_insertions, type};
    for (int i = 0; i < num_insertions; ++i){
        pq.insert(i, dist(mt));
    }

    return pq;
}

bool are_pqs_identical(PriorityQueue<int>& pq1, PriorityQueue<int>& pq2)
{
    if (pq1.type() != pq2.type()) {
        return false;
    }

    while (!(pq1.empty() || pq2.empty())) {
        if (pq1.extract() != pq2.extract()) {
            return false;
        }
    }

    return true;
}

TEST_CASE("PriorityQueue can be constructed", "[PriorityQueue]")
{
    SECTION("PriorityQueue can be default-constructed")
    {
        PriorityQueue<int> pq1;
        REQUIRE(pq1.empty());
        REQUIRE(pq1.type() == HeapType::max);

        PriorityQueue<int> pq2{4};
        REQUIRE(pq2.empty());
        REQUIRE(pq2.type() == HeapType::max);
    }

    SECTION("PrioritQueue can be constructed with arguments")
    {
        PriorityQueue<int> pq{4, HeapType::min};
        REQUIRE(pq.empty());
        REQUIRE(pq.type() == HeapType::min);
    }
}

TEST_CASE("PriorityQueue can be copy-constructed", "[PriorityQueue]")
{
    constexpr int num_insertions = 100000;
    auto orig_pq = create_priority_queue(num_insertions);
    auto copied_pq = orig_pq;
    REQUIRE(are_pqs_identical(copied_pq, orig_pq));
}

TEST_CASE("PriorityQueue can be move-constructed", "[PriorityQueue]")
{
    constexpr int num_insertions = 100000;
    auto orig_pq = create_priority_queue(num_insertions);
    auto copied_pq = orig_pq;
    auto moved_pq = std::move(orig_pq);
    REQUIRE(are_pqs_identical(moved_pq, copied_pq));
}

TEST_CASE("PriorityQueue can be copy-assigned", "[PriorityQueue]")
{
    constexpr int orig_num_insertions = 50000;
    constexpr int new_num_insertions = 100000;
    auto orig_pq = create_priority_queue(orig_num_insertions);
    auto new_pq = create_priority_queue(new_num_insertions);
    orig_pq = new_pq;
    REQUIRE(are_pqs_identical(new_pq, orig_pq));
}

TEST_CASE("PriorityQueue can be move-assigned", "[PriorityQueue]")
{
    constexpr int orig_num_insertions = 50000;
    constexpr int new_num_insertions = 100000;
    auto orig_pq = create_priority_queue(orig_num_insertions);
    auto new_pq = create_priority_queue(new_num_insertions);
    auto copied_pq = new_pq;
    orig_pq = std::move(new_pq);
    REQUIRE(are_pqs_identical(copied_pq, orig_pq));
}

TEST_CASE("Objects can be inserted into PriorityQueue,", "[PriorityQueue]")
{
    SECTION("Objects can be inserted without reallocation")
    {
        PriorityQueue<int> pq;  // default length is 8
        pq.insert(1, 4);
        pq.insert(2, 2);
        pq.insert(3, 3);
        REQUIRE(pq.size() == 3);
        REQUIRE(pq.extract().first == 1);
        REQUIRE(pq.extract().first == 3);
        REQUIRE(pq.extract().first == 2);
    }

    SECTION("Objects can be inserted with reallocation")
    {
        constexpr int num_insertions = 100000;
        auto pq = create_priority_queue(num_insertions);
        REQUIRE(pq.size() == 100000);
    }

    SECTION("Correct objects are inserted with reallocation")
    {
        PriorityQueue<int> pq;   // default length is 8
        pq.insert(1, 4);
        pq.insert(2, 6);
        pq.insert(3, 3);
        pq.insert(4, 8);
        pq.insert(5, 1);
        pq.insert(6, 2);
        pq.insert(7, 5);
        pq.insert(8, 7);
        pq.insert(9, 9);
        REQUIRE(pq.size() == 9);
        REQUIRE(pq.extract().first == 9);
        REQUIRE(pq.extract().first == 4);
        REQUIRE(pq.extract().first == 8);
    }
}

template<typename Compare, typename NumericLimit>
std::pair<PriorityQueue<int>, int> extreme_test(std::size_t num_insertions, HeapType type, Compare comp, NumericLimit numeric_limit)
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> dist{1, static_cast<int>(num_insertions)};
    PriorityQueue<int> pq{num_insertions, type};
    int extreme_priority = numeric_limit;
    for (int i = 0; i < num_insertions; ++i) {
        int priority = dist(mt);
        pq.insert(i, priority);
        if (comp(priority, extreme_priority)) {
            extreme_priority = priority;
        }
    }

    return {pq, extreme_priority};
}

TEST_CASE("Extreme function returns min or max priority element")
{
    SECTION("Extreme function returns min element for a min heap")
    {
        constexpr int num_insertions = 100000;
        auto extreme_pair = extreme_test(num_insertions, HeapType::min, std::less<>(), std::numeric_limits<int>::max());
        REQUIRE(extreme_pair.first.extreme().second == extreme_pair.second);
    }

    SECTION("Extreme function returns max element for a max heap")
    {
        constexpr int num_insertions = 100000;
        auto extreme_pair = extreme_test(num_insertions, HeapType::max, std::greater<>(), std::numeric_limits<int>::min());
        REQUIRE(extreme_pair.first.extreme().second == extreme_pair.second);
    }
}

TEST_CASE("Extract function returns min or max priority element and removes it")
{
    SECTION("Extract function returns min element for a min heap and removes it")
    {
        constexpr int num_insertions = 100000;
        auto extreme_pair = extreme_test(num_insertions, HeapType::min, std::less<>(), std::numeric_limits<int>::max());
        auto pq = extreme_pair.first;
        auto min_priority = extreme_pair.second;
        REQUIRE(pq.extract().second == min_priority);
        REQUIRE(pq.extreme().second >= min_priority);
    }

    SECTION("Extract function returns max element for a max heap and removes it")
    {
        constexpr int num_insertions = 100000;
        auto extreme_pair = extreme_test(num_insertions, HeapType::max, std::greater<>(), std::numeric_limits<int>::min());
        auto pq = extreme_pair.first;
        auto max_priority = extreme_pair.second;
        REQUIRE(pq.extract().second == max_priority);
        REQUIRE(pq.extreme().second <= max_priority);
    }
}

TEST_CASE("Size function returns correct size", "[PriorityQueue]")
{
    constexpr int num_insertions = 100000;
    constexpr int num_extractions = 50000;
    auto pq = create_priority_queue(num_insertions);
    REQUIRE(pq.size() == num_insertions);

    for (int i = 0; i < num_extractions; ++i) {
        pq.extract();
    }
    REQUIRE(pq.size() == num_insertions - num_extractions);

    pq.insert(1, 1);
    pq.insert(2, 2);
    REQUIRE(pq.size() == num_insertions - num_extractions + 2);
}

TEST_CASE("Empty function returns correct boolean value", "[PriorityQueue]")
{
    PriorityQueue<int> pq;
    REQUIRE(pq.empty());

    pq.insert(1, 1);
    pq.insert(2, 2);
    REQUIRE_FALSE(pq.empty());
}

TEST_CASE("Clear function erases all elements from heap", "[PriorityQueue]")
{
    SECTION("Clearing a heap results in an empty heap")
    {
        constexpr int num_insertions = 100000;
        auto pq = create_priority_queue(num_insertions);
        pq.clear();
        REQUIRE(pq.size() == 0);
        REQUIRE(pq.empty());
    }

    SECTION("Elements can be inserted again into a cleared heap")
    {
        constexpr int num_insertions = 100000;
        auto pq = create_priority_queue(num_insertions);
        pq.clear();
        pq.insert(1, 4);
        pq.insert(2, 6);
        pq.insert(3, 3);
        pq.insert(4, 8);
        pq.insert(5, 1);
        pq.insert(6, 2);
        pq.insert(7, 5);
        pq.insert(8, 7);
        pq.insert(9, 9);
        REQUIRE(pq.size() == 9);
        REQUIRE(pq.extract().first == 9);
        REQUIRE(pq.extract().first == 4);
        REQUIRE(pq.extract().first == 8);
    }
}