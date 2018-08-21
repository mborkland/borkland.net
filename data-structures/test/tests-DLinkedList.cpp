#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include "../../catch/catch.hpp"
#include "../src/DLinkedList.hpp"

using bork_lib::DLinkedList;

std::random_device rd;
constexpr int max_value = 100000;
constexpr int num_insertions = 50000;

TEST_CASE("Elements can be added to the end of a DLinkedList", "[DLinkedList]")
{
    constexpr int n = 11288;

    SECTION("Elements can be pushed back without errors")
    {
        DLinkedList<int> dlist;
        for (int i = 1; i <= n; ++i) {
            REQUIRE_NOTHROW(dlist.push_back(i));
        }
    }

    SECTION("The correct elements were added to the list")
    {
        DLinkedList<int> dlist;
        for (int i = 1; i <= n; ++i) {
            dlist.push_back(i);
        }
        int sum = std::accumulate(dlist.begin(), dlist.end(), 0);
        REQUIRE(sum == (n*(n+1))/2);
    }

    SECTION("The elements exist in the list in the correct order")
    {
        DLinkedList<int> dlist;
        for (int i = 1; i <= n; ++i) {
            dlist.push_back(i);
        }
        REQUIRE(std::is_sorted(dlist.begin(), dlist.end()));
    }
}

DLinkedList<int> fill_list_with_random_values()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> dist{-max_value, max_value};
    DLinkedList<int> dlist;
    for (int i = 0; i < num_insertions; ++i) {
        dlist.push_back(dist(mt));
    }

    return dlist;
}

DLinkedList<int> fill_list_with_consecutive_values(int n)
{
    DLinkedList<int> dlist;
    for (int i = 1; i <= n; ++i) {
        dlist.push_back(i);
    }

    return std::move(dlist);
}

TEST_CASE("DLinkedLists can be assigned to", "[DLinkedList]")
{
    SECTION("Move assignment occurs without errors")
    {
        DLinkedList<int> dlist1;
        REQUIRE_NOTHROW(dlist1 = fill_list_with_random_values());
    }

    SECTION("Copy assignment occurs without errors")
    {
        DLinkedList<int> dlist1;
        dlist1 = fill_list_with_random_values();
        DLinkedList<int> dlist2 = {1, 2, 3, 4, 5};
        REQUIRE_NOTHROW(dlist2 = dlist1);
    }

    SECTION("Lists have the same elements after copy assignment")
    {
        DLinkedList<int> dlist1;
        dlist1 = fill_list_with_random_values();
        DLinkedList<int> dlist2 = {1, 2, 3, 4, 5};
        dlist2 = dlist1;
        int sum1 = std::accumulate(dlist1.begin(), dlist1.end(), 0);
        int sum2 = std::accumulate(dlist2.begin(), dlist2.end(), 0);
        REQUIRE(sum1 == sum2);
    }
}

TEST_CASE("DLinkedLists can be constructed from another DLinkedList", "[DLinkedList]" )
{
    SECTION("List has the correct values after move construction")
    {
        constexpr int n = 11288;
        auto dlist = fill_list_with_consecutive_values(n);
        auto sum = std::accumulate(dlist.begin(), dlist.end(), 0);
        REQUIRE(sum == (n*(n+1))/2);
    }

    SECTION("Lists have the same elements after copy construction")
    {
        auto dlist1 = fill_list_with_random_values();
        auto dlist2 = dlist1;
        auto sum1 = std::accumulate(dlist1.begin(), dlist1.end(), 0);
        auto sum2 = std::accumulate(dlist2.begin(), dlist2.end(), 0);
        REQUIRE(sum1 == sum2);
    }
}

TEST_CASE("DLinkedList can be constructed from iterator range", "[DLinkedList]")
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> dist{-max_value, max_value};

    DLinkedList<int> dlist1;
    for (int i = 0; i < num_insertions; ++i) {
        dlist1.push_back(dist(mt));
    }

    REQUIRE(dlist1.size() == num_insertions);

    SECTION("lists have the same elements after iterator range construction") {
        DLinkedList<int> dlist2{dlist1.begin(), dlist1.end()};
        auto sum1 = std::accumulate(dlist1.begin(), dlist1.end(), 0);
        auto sum2 = std::accumulate(dlist2.begin(), dlist2.end(), 0);

        REQUIRE(sum1 == sum2);
    }
}

/*TEST_CASE("DLinkedList constructed having n elements, all equal to k", "[DLinkedList]")
{
    constexpr int n = 11288;

    SECTION("k value is specified")
    {
        constexpr int k = 12;
        DLinkedList<int> dlist(n, k);
        int sum = std::accumulate(dlist.begin(), dlist.end(), 0);
        REQUIRE(sum == n*k);
    }

    SECTION("Default k value is used")
    {
        DLinkedList<int> dlist(n);
        for (const auto& x : dlist) {
            REQUIRE(x == 0);
        }
    }
}*/

TEST_CASE("DLinkedList empty function returns correct value", "[DLinkedList]")
{
    SECTION("Default-constructed list is empty")
    {
        DLinkedList<int> dlist;
        REQUIRE(dlist.empty());
    }

    SECTION("List with elements is not empty")
    {
        DLinkedList<int> dlist = fill_list_with_random_values();
        REQUIRE_FALSE(dlist.empty());
    }

    SECTION("Cleared list is empty")
    {
        DLinkedList<int> dlist = fill_list_with_random_values();
        dlist.clear();
        REQUIRE(dlist.empty());
    }

    SECTION("List with all elements erased is empty")
    {
        DLinkedList<int> dlist;
        dlist.push_back(1);
        dlist.push_back(2);
        dlist.pop_front();
        dlist.pop_front();
        REQUIRE(dlist.empty());
    }
}

TEST_CASE("DLinkedList size function returns correct value", "[DLinkedList]")
{
    SECTION("Default-constructed list has size of zero")
    {
        DLinkedList<int> dlist;
        REQUIRE(dlist.size() == 0);
    }

    SECTION("List with elements added has correct size")
    {
        auto dlist = fill_list_with_random_values();
        REQUIRE(dlist.size() == num_insertions);
    }

    SECTION("List with elements removed has correct size")
    {
        auto dlist = fill_list_with_random_values();
        for (int i = 0; i < num_insertions / 2; ++i) {
            dlist.pop_front();
        }
        REQUIRE(dlist.size() == num_insertions / 2);
    }

    SECTION("Cleared list has the correct size")
    {
        auto dlist = fill_list_with_random_values();
        dlist.clear();
        REQUIRE(dlist.size() == 0);
    }
}

TEST_CASE("DLinkedList front and back functions allow access to the head and tail", "[DLinkedList]")
{
    constexpr int n = 11288;

    SECTION("Front function allows access to the head")
    {
        auto dlist = fill_list_with_consecutive_values(n);
        REQUIRE(dlist.front() == 1);
        dlist.front() = 42;
        REQUIRE(dlist.front() == 42);
    }

    SECTION("Back function allows access to the tail")
    {
        auto dlist = fill_list_with_consecutive_values(n);
        REQUIRE(dlist.back() == n);
        dlist.back() = 42;
        REQUIRE(dlist.back() == 42);
    }
}

TEST_CASE("DLinkedList insert_before function works as expected")
{
    constexpr int n = 50;
    constexpr int magic_number = 42;
    REQUIRE(n % 2 == 0);
    auto dlist = fill_list_with_consecutive_values(n);

    for (auto it = dlist.begin(); it != dlist.end(); ++it) {
        if (*it % 2 == 0) {
            it = dlist.insert_before(it, magic_number);
        }
    }

    for (auto it = dlist.begin(); it != dlist.end(); ++it) {
        auto prev_it = it;
        auto next_it = it;
        --prev_it;
        ++next_it;
        if (*it == magic_number) {
            continue;
        } else if (*it % 2 == 0) {
            REQUIRE(*prev_it == magic_number);
        } else {
            REQUIRE(*next_it == magic_number);
        }
    }
}

TEST_CASE("DLinkedList insert_after function works as expected")
{
    constexpr int n = 50;
    constexpr int magic_number = 42;
    REQUIRE(n % 2 == 0);
    auto dlist = fill_list_with_consecutive_values(n);

    for (auto it = dlist.begin(); it != dlist.end(); ++it) {
        if (*it % 2 == 1) {
            it = dlist.insert_after(it, magic_number);
        }
    }

    for (auto it = dlist.begin(); it != dlist.end(); ++it) {
        auto prev_it = it;
        auto next_it = it;
        --prev_it;
        ++next_it;
        if (*it == magic_number) {
            continue;
        } else if (*it % 2 == 0) {
            REQUIRE(*prev_it == magic_number);
        } else {
            REQUIRE(*next_it == magic_number);
        }
    }
}

TEST_CASE("DLinkedList insert_sorted function works as expected")
{

}


