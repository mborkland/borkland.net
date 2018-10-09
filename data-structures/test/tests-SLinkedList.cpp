#include <algorithm>
#include <random>
#include "../../catch/catch.hpp"
#include "../src/SLinkedList.hpp"

using bork_lib::SLinkedList;

std::random_device rd{};
constexpr int max_value = 100000;
constexpr int num_insertions = 50000;

TEST_CASE("Elements can be added to the end of a SLinkedList", "[SLinkedList]")
{
    constexpr int n = 11288;

    SECTION("Elements can be pushed back without errors")
    {
        SLinkedList<int> slist;
        for (int i = 1; i <= n; ++i) {
            REQUIRE_NOTHROW(slist.push_back(i));
        }
    }

    SECTION("The correct elements were added to the list")
    {
        SLinkedList<int> slist;
        for (int i = 1; i <= n; ++i) {
            slist.push_back(i);
        }
        int sum = std::accumulate(slist.begin(), slist.end(), 0);
        REQUIRE(sum == (n*(n+1))/2);
    }

    SECTION("The elements exist in the list in the correct order")
    {
        SLinkedList<int> slist;
        for (int i = 1; i <= n; ++i) {
            slist.push_back(i);
        }
        REQUIRE(std::is_sorted(slist.begin(), slist.end()));
    }
}

TEST_CASE("Elements can be added to the beginning of a SLinkedList", "[SLinkedList]")
{
    constexpr int n = 11288;

    SECTION("Elements can be pushed front without errors")
    {
        SLinkedList<int> slist;
        for (int i = 1; i <= n; ++i) {
            REQUIRE_NOTHROW(slist.push_front(i));
        }
    }

    SECTION("The correct elements were added to the list")
    {
        SLinkedList<int> slist;
        for (int i = 1; i <= n; ++i) {
            slist.push_front(i);
        }
        int sum = std::accumulate(slist.begin(), slist.end(), 0);
        REQUIRE(sum == (n*(n+1))/2);
    }

    SECTION("The elements exist in the list in the correct order")
    {
        SLinkedList<int> slist;
        for (int i = 1; i <= n; ++i) {
            slist.push_front(i);
        }
        REQUIRE(std::is_sorted(slist.begin(), slist.end(), [&](auto elem1, auto elem2){ return elem1 > elem2; }));
    }
}

SLinkedList<int> fill_list_with_random_values()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> dist{-max_value, max_value};
    SLinkedList<int> slist;
    for (int i = 0; i < num_insertions; ++i) {
        slist.push_front(dist(mt));
    }

    return slist;
}

SLinkedList<int> fill_list_with_consecutive_values(int n)
{
    SLinkedList<int> slist;
    for (int i = n; i >= 1; --i) {
        slist.push_front(i);
    }

    return std::move(slist);
}

TEST_CASE("SLinkedLists can be assigned to", "[SLinkedList]")
{
    SECTION("Move assignment occurs without errors")
    {
        SLinkedList<int> slist1;
        REQUIRE_NOTHROW(slist1 = fill_list_with_random_values());
    }

    SECTION("Copy assignment occurs without errors")
    {
        SLinkedList<int> slist1;
        slist1 = fill_list_with_random_values();
        SLinkedList<int> slist2 = {1, 2, 3, 4, 5};
        REQUIRE_NOTHROW(slist2 = slist1);
    }

    SECTION("Lists have the same elements after copy assignment")
    {
        SLinkedList<int> slist1;
        slist1 = fill_list_with_random_values();
        SLinkedList<int> slist2 = {1, 2, 3, 4, 5};
        slist2 = slist1;
        int sum1 = std::accumulate(slist1.begin(), slist1.end(), 0);
        int sum2 = std::accumulate(slist2.begin(), slist2.end(), 0);
        REQUIRE(sum1 == sum2);
    }
}

TEST_CASE("SLinkedLists can be constructed from another SLinkedList", "[SLinkedList]" )
{
    SECTION("List has the correct values after move construction")
    {
        constexpr int n = 11288;
        auto slist = fill_list_with_consecutive_values(n);
        auto sum = std::accumulate(slist.begin(), slist.end(), 0);
        REQUIRE(sum == (n*(n+1))/2);
    }

    SECTION("Lists have the same elements after copy construction")
    {
        auto slist1 = fill_list_with_random_values();
        auto slist2 = slist1;
        auto sum1 = std::accumulate(slist1.begin(), slist1.end(), 0);
        auto sum2 = std::accumulate(slist2.begin(), slist2.end(), 0);
        REQUIRE(sum1 == sum2);
    }
}

TEST_CASE("SLinkedList can be constructed from iterator range", "[SLinkedList]")
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> dist{-max_value, max_value};

    SLinkedList<int> slist1;
    for (int i = 0; i < num_insertions; ++i) {
        slist1.push_back(dist(mt));
    }

    REQUIRE(slist1.size() == num_insertions);

    SECTION("lists have the same elements after iterator range construction") {
        SLinkedList<int> slist2{slist1.begin(), slist1.end()};
        auto sum1 = std::accumulate(slist1.begin(), slist1.end(), 0);
        auto sum2 = std::accumulate(slist2.begin(), slist2.end(), 0);

        REQUIRE(sum1 == sum2);
    }
}

TEST_CASE("SLinkedList empty function returns correct value", "[SLinkedList]")
{
    SECTION("Default-constructed list is empty")
    {
        SLinkedList<int> slist;
        REQUIRE(slist.empty());
    }

    SECTION("List with elements is not empty")
    {
        SLinkedList<int> slist = fill_list_with_random_values();
        REQUIRE_FALSE(slist.empty());
    }

    SECTION("Cleared list is empty")
    {
        SLinkedList<int> slist = fill_list_with_random_values();
        slist.clear();
        REQUIRE(slist.empty());
    }

    SECTION("List with all elements erased is empty")
    {
        SLinkedList<int> slist;
        slist.push_back(1);
        slist.push_back(2);
        slist.pop_front();
        slist.pop_front();
        REQUIRE(slist.empty());
    }
}

TEST_CASE("SLinkedList size function returns correct value", "[SLinkedList]")
{
    SECTION("Default-constructed list has size of zero")
    {
        SLinkedList<int> slist;
        REQUIRE(slist.size() == 0);
    }

    SECTION("List with elements added has correct size")
    {
        auto slist = fill_list_with_random_values();
        REQUIRE(slist.size() == num_insertions);
    }

    SECTION("List with elements removed has correct size")
    {
        auto slist = fill_list_with_random_values();
        for (int i = 0; i < num_insertions / 2; ++i) {
            slist.pop_front();
        }
        REQUIRE(slist.size() == num_insertions / 2);
    }

    SECTION("Cleared list has the correct size")
    {
        auto slist = fill_list_with_random_values();
        slist.clear();
        REQUIRE(slist.size() == 0);
    }
}

TEST_CASE("SLinkedList front and back functions allow access to the head and tail", "[SLinkedList]")
{
    constexpr int n = 11288;

    SECTION("Front function allows access to the head")
    {
        auto slist = fill_list_with_consecutive_values(n);
        REQUIRE(slist.front() == 1);
        slist.front() = 42;
        REQUIRE(slist.front() == 42);
    }

    SECTION("Back function allows access to the tail")
    {
        auto slist = fill_list_with_consecutive_values(n);
        REQUIRE(slist.back() == n);
        slist.back() = 42;
        REQUIRE(slist.back() == 42);
    }
}

TEST_CASE("SLinkedList insert_before function works as expected", "[SLinkedList]")
{
    constexpr int magic_number = 42;

    SECTION("Insert_before inserts correctly into an empty list")
    {
        SLinkedList<int> slist;
        auto it = slist.begin();
        REQUIRE_NOTHROW(slist.insert_before(it, magic_number));
        REQUIRE(slist.size() == 1);
        REQUIRE(slist.front() == slist.back());
    }

    SECTION("Insert_before inserts correctly into an non-empty list")
    {
        constexpr int n = 50;
        REQUIRE(n % 2 == 0);
        auto slist = fill_list_with_consecutive_values(n);

        for (auto it = slist.begin(); it != slist.end(); ++it) {
            if (*it % 2 == 0) {
                it = slist.insert_before(it, magic_number);
            }
        }

        for (auto it = slist.begin(); it != slist.end(); ++it) {
            auto next_it = it;
            ++next_it;
            if (*it == magic_number) {
                int next = *next_it;
                if (next != magic_number + 1) {
                    REQUIRE(next % 2 == 0);
                }
            }
        }
    }
}

TEST_CASE("SLinkedList insert_after function works as expected", "[SLinkedList]")
{
    constexpr int magic_number = 42;

    SECTION("Insert_after inserts correctly into an empty list")
    {
        SLinkedList<int> slist;
        auto it = slist.begin();
        REQUIRE_NOTHROW(slist.insert_after(it, magic_number));
        REQUIRE(slist.size() == 1);
        REQUIRE(slist.front() == slist.back());
    }

    SECTION("Insert_after inserts correctly into a non-empty list")
    {
        constexpr int n = 50;
        REQUIRE(n % 2 == 0);
        auto slist = fill_list_with_consecutive_values(n);

        for (auto it = slist.begin(); it != slist.end(); ++it) {
            if (*it % 2 == 1) {
                it = slist.insert_after(it, magic_number);
            }
        }

        for (auto it = slist.begin(); it != slist.end(); ++it) {
            auto next_it = it;
            ++next_it;
            if (*it % 2 == 1) {
                int next = *next_it;
                REQUIRE(next == magic_number);
            }
        }
    }
}

SLinkedList<int> fill_with_even_values(int n)
{
    SLinkedList<int> slist;
    for (int i = 0; i < n; i += 2) {
        slist.push_back(i);
    }

    return slist;
}

TEST_CASE("SLinkedList insert_sorted function works as expected", "[SLinkedList]")
{
    constexpr int magic_number = 42;

    SECTION("Insert_sorted inserts correctly into an empty list")
    {
        SLinkedList<int> slist;
        slist.insert_sorted(magic_number);
        REQUIRE(slist.size() == 1);
        REQUIRE(slist.front() == slist.back());
    }

    SECTION("Insert_sorted inserts correctly into a non-empty list")
    {
        constexpr int n = magic_number * 2;

        SECTION("Insert_sorted inserts a unique value correctly")
        {
            auto slist = fill_with_even_values(n);
            slist.insert_sorted(magic_number - 1);
            REQUIRE(std::is_sorted(slist.begin(), slist.end()));
            for (auto it = slist.begin(); it != slist.end(); ++it) {
                if (*it == magic_number - 2) {
                    auto next_it = it;
                    ++next_it;
                    REQUIRE(*next_it == magic_number - 1);
                }
            }
        }

        SECTION("Insert_sorted inserts a duplicate value correctly")
        {
            auto slist = fill_with_even_values(n);
            slist.insert_sorted(magic_number);
            REQUIRE(std::is_sorted(slist.begin(), slist.end()));
            for (auto it = slist.begin(); it != slist.end(); ++it) {
                if (*it == magic_number) {
                    auto next_it = it;
                    ++next_it;
                    auto next_next_it = next_it;
                    ++next_next_it;
                    if (*next_it == magic_number) {
                        REQUIRE(*next_next_it == magic_number + 2);
                    } else {
                        REQUIRE(*next_it == magic_number + 2);
                        REQUIRE(*next_next_it == magic_number + 4);
                    }
                }
            }
        }
    }
}

TEST_CASE("SLinkedList emplace_before function works as expected", "[SLinkedList]")
{
    SECTION("Emplace_before inserts correctly into an empty list")
    {
        using int_pair = std::pair<int, int>;
        SLinkedList<int_pair> slist;
        auto it = slist.begin();
        it = slist.emplace_before(it, 3, 4);
        int_pair front_pair = slist.front();
        REQUIRE(front_pair.first == 3);
        REQUIRE(front_pair.second == 4);
    }

    SECTION("Emplace_before inserts correctly into a non-empty list")
    {
        using int_pair = std::pair<int, int>;
        SLinkedList<int_pair> slist;
        auto it = slist.begin();
        it = slist.emplace_before(it, 3, 4);
        slist.emplace_before(it, 5, 6);
        int_pair front_pair = slist.front();
        REQUIRE(front_pair.first == 5);
        REQUIRE(front_pair.second == 6);
    }
}

TEST_CASE("SLinkedList emplace_after function works as expected", "[SLinkedList]")
{
    SECTION("Emplace_after inserts correctly into an empty list")
    {
        using int_pair = std::pair<int, int>;
        SLinkedList<int_pair> slist;
        auto it = slist.begin();
        it = slist.emplace_after(it, 3, 4);
        int_pair front_pair = slist.front();
        REQUIRE(front_pair.first == 3);
        REQUIRE(front_pair.second == 4);
    }

    SECTION("Emplace_before inserts correctly into a non-empty list")
    {
        using int_pair = std::pair<int, int>;
        SLinkedList<int_pair> slist;
        auto it = slist.begin();
        it = slist.emplace_after(it, 3, 4);
        slist.emplace_after(it, 5, 6);
        int_pair back_pair = slist.back();
        REQUIRE(back_pair.first == 5);
        REQUIRE(back_pair.second == 6);
    }
}

TEST_CASE("SLinkedList emplace_sorted function works as expected", "[SLinkedList]")
{
    SECTION("Emplace_sorted inserts correctly into an empty list")
    {
        using int_pair = std::pair<int, int>;
        SLinkedList<int_pair> slist;
        slist.emplace_sorted(3, 4);
        int_pair front_pair = slist.front();
        REQUIRE(front_pair.first == 3);
        REQUIRE(front_pair.second == 4);
    }

    SECTION("Emplace_sorted inserts correctly into a non-empty list")
    {
        using int_pair = std::pair<int, int>;
        SLinkedList<int_pair> slist;
        slist.emplace_sorted(3, 4);
        slist.emplace_sorted(5, 6);
        int_pair back_pair = slist.back();
        REQUIRE(back_pair.first == 5);
        REQUIRE(back_pair.second == 6);
    }
}

TEST_CASE("SLinkedList pop_front function works as expected", "[SLinkedList]")
{
    SECTION("An exception is thrown when attempting to pop from an empty list")
    {
        SLinkedList<int> slist;
        REQUIRE_THROWS_WITH(slist.pop_front(), Catch::Contains("delete") && Catch::Contains("null pointer"));
    }

    SECTION("Popping from an one-element list results in an empty list")
    {
        SLinkedList<int> slist{2};
        slist.pop_front();
        REQUIRE(slist.empty());
        REQUIRE(slist.size() == 0);
    }

    SECTION("Popping from an multi-element list works as expected")
    {
        constexpr int n = 50;
        auto slist = fill_list_with_consecutive_values(n);
        slist.pop_front();
        REQUIRE(slist.size() == n - 1);
        REQUIRE(slist.front() == 2);
    }
}

TEST_CASE("SLinkedList pop_back function works as expected", "[SLinkedList]")
{
    SECTION("An exception is thrown when attempting to pop from an empty list")
    {
        SLinkedList<int> slist;
        REQUIRE_THROWS_WITH(slist.pop_back(), Catch::Contains("delete") && Catch::Contains("null pointer"));
    }

    SECTION("Popping from an one-element list results in an empty list")
    {
        SLinkedList<int> slist{2};
        slist.pop_back();
        REQUIRE(slist.empty());
        REQUIRE(slist.size() == 0);
    }

    SECTION("Popping from an multi-element list works as expected")
    {
        constexpr int n = 50;
        auto slist = fill_list_with_consecutive_values(n);
        slist.pop_back();
        REQUIRE(slist.size() == n - 1);
        REQUIRE(slist.back() == n - 1);
    }
}

TEST_CASE("SLinkedList find function works as expected", "[SLinkedList]")
{
    SECTION("An element that exists in the list is found")
    {
        constexpr int n = 50;
        constexpr int magic_number = 42;
        auto slist = fill_list_with_consecutive_values(n);
        auto it = slist.find(magic_number);
        REQUIRE(*it == magic_number);
        ++it;
        REQUIRE(*it == magic_number + 1);
    }

    SECTION("Searching for an element not in the list returns end iterator")
    {
        constexpr int n = 50;
        constexpr int magic_number1 = -8;
        constexpr int magic_number2 = 67;
        auto slist = fill_list_with_consecutive_values(n);
        auto it1 = slist.find(magic_number1);
        auto it2 = slist.find(magic_number2);
        REQUIRE(it1 == slist.end());
        REQUIRE(it2 == slist.end());
    }
}

TEST_CASE("SLinkedList count function works as expected", "[SLinkedList]")
{
    constexpr int n = 50;
    constexpr int magic_number = 42;

    SECTION("The correct count is returned when the value is not in the list")
    {
        SLinkedList<int> slist;
        for (int i = 0; i < n; ++i) {
            slist.push_back(magic_number);
        }
        auto num = slist.count(magic_number  + 1);
        REQUIRE(num == 0);
    }

    SECTION("The correct count is returned when the value is in the list")
    {
        SLinkedList<int> slist;
        for (int i = 0; i < n; ++i) {
            slist.push_back(magic_number);
        }
        auto num = slist.count(magic_number);
        REQUIRE(num == n);
    }
}

TEST_CASE("SLinkedList erase function works as expected", "[SLinkedList]")
{
    SECTION("Erasing from an empty list throws an exception")
    {
        SLinkedList<int> slist;
        auto it = slist.begin();
        REQUIRE_THROWS_WITH(slist.erase(it), Catch::Contains("delete") && Catch::Contains("empty list"));
    }

    SECTION("Erasing from an one-element list results in an empty list")
    {
        SLinkedList<int> slist{2};
        auto it = slist.begin();
        it = slist.erase(it);
        REQUIRE(slist.empty());
        REQUIRE(slist.size() == 0);
        REQUIRE(it == slist.end());
    }

    SECTION("Erasing from a multi-element list works correctly")
    {
        constexpr int n = 50;
        auto slist = fill_list_with_consecutive_values(n);
        for (auto it = slist.begin(); it != slist.end(); ) {
            if (*it % 2 == 0) {
                it = slist.erase(it);
            } else {
                ++it;
            }
        }

        for (const auto& x : slist) {
            REQUIRE(x % 2 == 1);
        }
    }
}

TEST_CASE("SLinkedList sort function works as expected")
{
    SECTION("A list of random values is sorted correctly")
    {
        auto slist = fill_list_with_random_values();
        REQUIRE_FALSE(std::is_sorted(slist.begin(), slist.end()));
        bork_lib::list_sort(slist);
        REQUIRE(std::is_sorted(slist.begin(), slist.end()));
    }
}

struct A
{
    int x;
    int y;
};

TEST_CASE("SLinkedList can hold objects that cannot be sorted", "[SLinkedList]")
{
    SLinkedList<A> slist;
    A a1{3, 4};
    A a2{5, 6};
    slist.push_back(a1);
    slist.push_back(a2);
    REQUIRE(slist.front().x == 3);
    REQUIRE(slist.front().y == 4);
}

template<typename T>
struct NonCopyable
{
    T x;
    T y;
    explicit NonCopyable(T x = {}, T y = {}) : x{x}, y{y} { }
    NonCopyable(const NonCopyable& other) = delete;
    NonCopyable& operator=(const NonCopyable& other) = delete;
    NonCopyable(NonCopyable&& other) noexcept : x{std::move(other.x)}, y{std::move(other.y)} { }
    NonCopyable& operator=(NonCopyable&& other) noexcept
    {
        using std::swap;
        swap(x, other.x);
        swap(y, other.y);
        return *this;
    }
    ~NonCopyable() = default;
    friend bool operator<=(const NonCopyable& lhs, const NonCopyable& rhs)
    {
        return lhs.x <= rhs.x;
    }
};

TEST_CASE("DLinkedList can hold non-copyable and non-movable objects", "[DLinkedList]")
{
    SECTION("DLinkedList can hold non-copyable objects")
    {
        using no_copy = NonCopyable<int>;
        SLinkedList <no_copy> slist;
        slist.emplace_back(1, 2);
        slist.emplace_back(3, 4);
        slist.emplace_front(5, 6);
        REQUIRE(slist.size() == 3);
        auto it = slist.begin();
        REQUIRE(it->x == 5);
        REQUIRE(it->y == 6);
        ++it;
        REQUIRE(it->x == 1);
        REQUIRE(it->y == 2);
        ++it;
        REQUIRE(it->x == 3);
        REQUIRE(it->y == 4);
        bork_lib::list_sort(slist);
    }
}

