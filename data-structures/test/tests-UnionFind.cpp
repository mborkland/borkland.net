#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include "../../catch/catch.hpp"
#include "../src/UnionFind.hpp"

using bork_lib::UnionFind;

std::random_device rd{};

UnionFind<int> create_union_find(int num_sets)
{
    UnionFind<int> uf;
    for (int i = 0; i < num_sets; ++i) {
        uf.make_set(i);
    }

    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> dist{0, num_sets - 1};
    for (int i = 0; i < num_sets / 5; ++i) {
        int key1 = dist(mt);
        int key2;
        do {
            key2 = dist(mt);
        } while (key2 == key1);

        uf.join(key1, key2);
    }

    return uf;
}

bool are_ufs_identical(const UnionFind<int>& uf1, const UnionFind<int>& uf2, int num_sets)
{
    for (int i = 0; i < num_sets; ++i) {
        for (int j = 0; j < num_sets; ++j) {
            if (uf1.same_set(i, j) != uf2.same_set(i, j)) {
                return false;
            }
        }
    }

    return true;
}

TEST_CASE("Copy construction results in an identical structure", "[UnionFind]")
{
    constexpr int num_sets = 100;
    auto orig_uf = create_union_find(num_sets);
    auto new_uf = orig_uf;
    REQUIRE(are_ufs_identical(new_uf, orig_uf, num_sets));
}

TEST_CASE("Move construction resutls in an identical structure", "[UnionFind]")
{
    constexpr int num_sets = 100;
    auto orig_uf = create_union_find(num_sets);
    auto copied_uf = orig_uf;
    auto moved_uf = std::move(orig_uf);
    REQUIRE(are_ufs_identical(moved_uf, copied_uf, num_sets));
}

TEST_CASE("Copy assignment results in an identical structure", "[UnionFind]")
{
    constexpr int num_sets = 100;
    auto orig_uf = create_union_find(num_sets);
    auto new_uf = create_union_find(num_sets / 10);
    new_uf = orig_uf;
    REQUIRE(are_ufs_identical(new_uf, orig_uf, num_sets));
}

TEST_CASE("Move assignment results in an identical structure", "[UnionFind]")
{
    constexpr int num_sets = 100;
    auto orig_uf = create_union_find(num_sets);
    auto copied_uf = orig_uf;
    auto moved_uf = create_union_find(num_sets / 10);
    moved_uf = std::move(orig_uf);
    REQUIRE(are_ufs_identical(moved_uf, copied_uf, num_sets));
}

TEST_CASE("Join function puts two keys in the same set", "[UnionFind]")
{
    constexpr int num_sets = 100000;
    UnionFind<int> uf;
    for (int i = 0; i < num_sets; ++i) {
        uf.make_set(i);
    }

    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> dist{0, num_sets - 1};
    std::unordered_map<int, std::unordered_set<int>> keys_in_same_set;
    for (int i = 0; i < num_sets / 5; ++i) {
        int key1 = dist(mt);
        int key2;
        do {
            key2 = dist(mt);
        } while (key2 == key1);

        uf.join(key1, key2);
        keys_in_same_set[key1].insert(key2);
        keys_in_same_set[key2].insert(key1);
    }

    for (auto x : keys_in_same_set) {
        for (auto y : x.second) {
            REQUIRE(uf.same_set(x.first, y));
        }
    }
}