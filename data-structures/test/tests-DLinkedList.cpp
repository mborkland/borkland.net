#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include "../../catch/catch.hpp"
#include "../src/DLinkedList.hpp"

using bork_lib::DLinkedList;

std::random_device rd{};
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

TEST_CASE("Elements can be added to the beginning of a DLinkedList", "[DLinkedList]")
{
    constexpr int n = 11288;

    SECTION("Elements can be pushed front without errors")
    {
        DLinkedList<int> dlist;
        for (int i = 1; i <= n; ++i) {
            REQUIRE_NOTHROW(dlist.push_front(i));
        }
    }

    SECTION("The correct elements were added to the list")
    {
        DLinkedList<int> dlist;
        for (int i = 1; i <= n; ++i) {
            dlist.push_front(i);
        }
        int sum = std::accumulate(dlist.begin(), dlist.end(), 0);
        REQUIRE(sum == (n*(n+1))/2);
    }

    SECTION("The elements exist in the list in the correct order")
    {
        DLinkedList<int> dlist;
        for (int i = 1; i <= n; ++i) {
            dlist.push_front(i);
        }
        REQUIRE(std::is_sorted(dlist.begin(), dlist.end(), [&](auto elem1, auto elem2){ return elem1 > elem2; }));
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

TEST_CASE("DLinkedList insert_before function works as expected", "[DLinkedList]")
{
    constexpr int magic_number = 42;

    SECTION("Insert_before inserts correctly into an empty list")
    {
        DLinkedList<int> dlist;
        auto it = dlist.begin();
        REQUIRE_NOTHROW(dlist.insert_before(it, magic_number));
        REQUIRE(dlist.size() == 1);
        REQUIRE(dlist.front() == dlist.back());
    }

    SECTION("Insert_before inserts correctly into an non-empty list")
    {
        constexpr int n = 50;
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
}

TEST_CASE("DLinkedList insert_after function works as expected", "[DLinkedList]")
{
    constexpr int magic_number = 42;

    SECTION("Insert_after inserts correctly into an empty list")
    {
        DLinkedList<int> dlist;
        auto it = dlist.begin();
        REQUIRE_NOTHROW(dlist.insert_after(it, magic_number));
        REQUIRE(dlist.size() == 1);
        REQUIRE(dlist.front() == dlist.back());
    }

    SECTION("Insert_after inserts correctly into a non-empty list")
    {
        constexpr int n = 50;
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
}

DLinkedList<int> fill_with_even_values(int n)
{
    DLinkedList<int> dlist;
    for (int i = 0; i < n; i += 2) {
        dlist.push_back(i);
    }

    return dlist;
}

TEST_CASE("DLinkedList insert_sorted function works as expected", "[DLinkedList]")
{
    constexpr int magic_number = 42;

    SECTION("Insert_sorted inserts correctly into an empty list")
    {
        DLinkedList<int> dlist;
        dlist.insert_sorted(magic_number);
        REQUIRE(dlist.size() == 1);
        REQUIRE(dlist.front() == dlist.back());
    }

    SECTION("Insert_sorted inserts correctly into a non-empty list")
    {
        constexpr int n = magic_number * 2;

        SECTION("Insert_sorted inserts a unique value correctly")
        {
            auto dlist = fill_with_even_values(n);
            dlist.insert_sorted(magic_number - 1);
            REQUIRE(std::is_sorted(dlist.begin(), dlist.end()));
            for (auto it = dlist.begin(); it != dlist.end(); ++it) {
                if (*it == magic_number - 2) {
                    auto next_it = it;
                    ++next_it;
                    REQUIRE(*next_it == magic_number - 1);
                }

                if (*it == magic_number) {
                    auto prev_it = it;
                    --prev_it;
                    REQUIRE(*prev_it == magic_number - 1);
                }
            }
        }

        SECTION("Insert_sorted inserts a duplicate value correctly")
        {
            auto dlist = fill_with_even_values(n);
            dlist.insert_sorted(magic_number);
            REQUIRE(std::is_sorted(dlist.begin(), dlist.end()));
            for (auto it = dlist.begin(); it != dlist.end(); ++it) {
                if (*it == magic_number) {
                    auto prev_it = it;
                    auto next_it = it;
                    --prev_it;
                    ++next_it;
                    if (*prev_it == magic_number - 2) {
                        REQUIRE(*next_it == magic_number);
                    } else {
                        REQUIRE(*next_it == magic_number + 2);
                    }
                }
            }
        }
    }
}

TEST_CASE("DLinkedList emplace_before function works as expected", "[DLinkedList]")
{
    SECTION("Emplace_before inserts correctly into an empty list")
    {
        using int_pair = std::pair<int, int>;
        DLinkedList<int_pair> dlist;
        auto it = dlist.begin();
        it = dlist.emplace_before(it, 3, 4);
        int_pair front_pair = dlist.front();
        REQUIRE(front_pair.first == 3);
        REQUIRE(front_pair.second == 4);
    }

    SECTION("Emplace_before inserts correctly into a non-empty list")
    {
        using int_pair = std::pair<int, int>;
        DLinkedList<int_pair> dlist;
        auto it = dlist.begin();
        it = dlist.emplace_before(it, 3, 4);
        dlist.emplace_before(it, 5, 6);
        int_pair front_pair = dlist.front();
        REQUIRE(front_pair.first == 5);
        REQUIRE(front_pair.second == 6);
    }
}

TEST_CASE("DLinkedList emplace_after function works as expected", "[DLinkedList]")
{
    SECTION("Emplace_after inserts correctly into an empty list")
    {
        using int_pair = std::pair<int, int>;
        DLinkedList<int_pair> dlist;
        auto it = dlist.begin();
        it = dlist.emplace_after(it, 3, 4);
        int_pair front_pair = dlist.front();
        REQUIRE(front_pair.first == 3);
        REQUIRE(front_pair.second == 4);
    }

    SECTION("Emplace_before inserts correctly into a non-empty list")
    {
        using int_pair = std::pair<int, int>;
        DLinkedList<int_pair> dlist;
        auto it = dlist.begin();
        it = dlist.emplace_after(it, 3, 4);
        dlist.emplace_after(it, 5, 6);
        int_pair back_pair = dlist.back();
        REQUIRE(back_pair.first == 5);
        REQUIRE(back_pair.second == 6);
    }
}

TEST_CASE("DLinkedList emplace_sorted function works as expected", "[DLinkedList]")
{
    SECTION("Emplace_sorted inserts correctly into an empty list")
    {
        using int_pair = std::pair<int, int>;
        DLinkedList<int_pair> dlist;
        dlist.emplace_sorted(3, 4);
        int_pair front_pair = dlist.front();
        REQUIRE(front_pair.first == 3);
        REQUIRE(front_pair.second == 4);
    }

    SECTION("Emplace_sorted inserts correctly into a non-empty list")
    {
        using int_pair = std::pair<int, int>;
        DLinkedList<int_pair> dlist;
        dlist.emplace_sorted(3, 4);
        dlist.emplace_sorted(5, 6);
        int_pair back_pair = dlist.back();
        REQUIRE(back_pair.first == 5);
        REQUIRE(back_pair.second == 6);
    }
}

TEST_CASE("DLinkedList pop_front function works as expected", "[DLinkedList]")
{
    SECTION("An exception is thrown when attempting to pop from an empty list")
    {
        DLinkedList<int> dlist;
        REQUIRE_THROWS_WITH(dlist.pop_front(), Catch::Contains("delete") && Catch::Contains("null pointer"));
    }

    SECTION("Popping from an one-element list results in an empty list")
    {
        DLinkedList<int> dlist{2};
        dlist.pop_front();
        REQUIRE(dlist.empty());
        REQUIRE(dlist.size() == 0);
    }

    SECTION("Popping from an multi-element list works as expected")
    {
        constexpr int n = 50;
        auto dlist = fill_list_with_consecutive_values(n);
        dlist.pop_front();
        REQUIRE(dlist.size() == n - 1);
        REQUIRE(dlist.front() == 2);
    }
}

TEST_CASE("DLinkedList pop_back function works as expected", "[DLinkedList]")
{
    SECTION("An exception is thrown when attempting to pop from an empty list")
    {
        DLinkedList<int> dlist;
        REQUIRE_THROWS_WITH(dlist.pop_back(), Catch::Contains("delete") && Catch::Contains("null pointer"));
    }

    SECTION("Popping from an one-element list results in an empty list")
    {
        DLinkedList<int> dlist{2};
        dlist.pop_back();
        REQUIRE(dlist.empty());
        REQUIRE(dlist.size() == 0);
    }

    SECTION("Popping from an multi-element list works as expected")
    {
        constexpr int n = 50;
        auto dlist = fill_list_with_consecutive_values(n);
        dlist.pop_back();
        REQUIRE(dlist.size() == n - 1);
        REQUIRE(dlist.back() == n - 1);
    }
}

TEST_CASE("DLinkedList find function works as expected", "[DLinkedList]")
{
    SECTION("An element that exists in the list is found")
    {
        constexpr int n = 50;
        constexpr int magic_number = 42;
        auto dlist = fill_list_with_consecutive_values(n);
        auto it = dlist.find(magic_number);
        REQUIRE(*it == magic_number);
        ++it;
        REQUIRE(*it == magic_number + 1);
    }

    SECTION("Searching for an element not in the list returns end iterator")
    {
        constexpr int n = 50;
        constexpr int magic_number1 = -8;
        constexpr int magic_number2 = 67;
        auto dlist = fill_list_with_consecutive_values(50);
        auto it1 = dlist.find(magic_number1);
        auto it2 = dlist.find(magic_number2);
        REQUIRE(it1 == dlist.end());
        REQUIRE(it2 == dlist.end());
    }
}

TEST_CASE("DLinkedList count function works as expected", "[DLinkedList]")
{
    constexpr int n = 50;
    constexpr int magic_number = 42;

    SECTION("The correct count is returned when the value is not in the list")
    {
        DLinkedList<int> dlist;
        for (int i = 0; i < n; ++i) {
            dlist.push_back(magic_number);
        }
        auto num = dlist.count(magic_number  + 1);
        REQUIRE(num == 0);
    }

    SECTION("The correct count is returned when the value is in the list")
    {
        DLinkedList<int> dlist;
        for (int i = 0; i < n; ++i) {
            dlist.push_back(magic_number);
        }
        auto num = dlist.count(magic_number);
        REQUIRE(num == n);
    }
}

TEST_CASE("DLinkedList erase function works as expected", "[DLinkedList]")
{
    SECTION("Erasing from an empty list throws an exception")
    {
        DLinkedList<int> dlist;
        auto it = dlist.begin();
        REQUIRE_THROWS_WITH(dlist.erase(it), Catch::Contains("delete") && Catch::Contains("empty list"));
    }

    SECTION("Erasing from an one-element list results in an empty list")
    {
        DLinkedList<int> dlist{2};
        auto it = dlist.begin();
        it = dlist.erase(it);
        REQUIRE(dlist.empty());
        REQUIRE(dlist.size() == 0);
        REQUIRE(it == dlist.end());
    }

    SECTION("Erasing from a multi-element list works correctly")
    {
        constexpr int n = 50;
        auto dlist = fill_list_with_consecutive_values(n);
        for (auto it = dlist.begin(); it != dlist.end(); ) {
            if (*it % 2 == 0) {
                it = dlist.erase(it);
            } else {
                ++it;
            }
        }

        for (const auto& x : dlist) {
            REQUIRE(x % 2 == 1);
        }
    }
}

TEST_CASE("DLinkedList sort function works as expected")
{
    SECTION("A list of random values is sorted correctly")
    {
        auto dlist = fill_list_with_random_values();
        REQUIRE_FALSE(std::is_sorted(dlist.begin(), dlist.end()));
        dlist.sort();
        REQUIRE(std::is_sorted(dlist.begin(), dlist.end()));
    }
}

// TODO: reverse iterator tests