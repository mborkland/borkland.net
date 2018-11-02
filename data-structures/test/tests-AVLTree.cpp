#include <algorithm>
#include <random>
#include <string>
#include <vector>
#include "../../catch/catch.hpp"
#include "../src/AVLTree.hpp"

using bork_lib::AVLTree;

std::random_device rd;
constexpr int max_key_value = 100000;
constexpr int max_val_value = 25000;
constexpr int num_insertions = 50000;
int num_added = 0;

AVLTree<int, int> fill_tree_with_random_values()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};
    AVLTree<int, int> tree;
    for (int i = 0; i < num_insertions; ++i) {
        if (tree.insert({key_dist(mt), val_dist(mt)}).second) {
            ++num_added;
        }
    }

    return tree;
}

AVLTree<int, int> fill_tree_with_consecutive_values(int n)
{
    AVLTree<int, int> tree;
    for (int i = 1; i <= n; ++i) {
        tree.insert({i,i});
    }

    return std::move(tree);
}

TEST_CASE("AVLTrees can be assigned to", "[AVLTree]")
{
    SECTION("Move assignment occurs without errors")
    {
        AVLTree<int, int> tree;
        REQUIRE_NOTHROW(tree = std::move(fill_tree_with_random_values()));
    }

    SECTION("Copy assignment occurs without errors")
    {
        AVLTree<int, int> tree1;
        tree1 = fill_tree_with_random_values();
        AVLTree<int, int> tree2 = {{1,1}, {2,2}, {3,3}, {4,4}, {5,5}};
        REQUIRE_NOTHROW(tree2 = tree1);
    }

    SECTION("Trees have the correct elements after move assignment")
    {
        AVLTree<int, int> tree1 = fill_tree_with_random_values();
        std::vector<int> key_vec {};
        std::vector<int> val_vec {};
        for (const auto& x : tree1) {
            key_vec.push_back(x.first);
            val_vec.push_back(x.second);
        }

        AVLTree<int, int> tree2 = {{1,1}, {2,2}, {3,3}, {4,4}, {5,5}};
        tree2 = std::move(tree1);
        auto key_it = key_vec.begin();
        auto val_it = val_vec.begin();
        std::for_each(tree2.begin(), tree2.end(), [&](auto elem) {
            REQUIRE(elem.first == *key_it);
            REQUIRE(elem.second == *val_it);
            ++key_it;
            ++val_it;
        });
    }

    SECTION("Trees have the same elements after copy assignment")
    {
        AVLTree<int, int> tree1;
        tree1 = fill_tree_with_random_values();
        AVLTree<int, int> tree2 = {{1,1}, {2,2}, {3,3}, {4,4}, {5,5}};
        tree2 = tree1;
        auto it = tree1.begin();
        std::for_each(tree2.begin(), tree2.end(), [&](auto elem) {
            REQUIRE(elem.first == it->first);
            REQUIRE(elem.second == it->second);
            ++it;
        });
    }
}

TEST_CASE("AVLTrees can be constructed from another AVLTree", "[AVLTree]" )
{
    SECTION("Tree has the correct values after move construction")
    {
        constexpr int n = 11288;
        auto tree = std::move(fill_tree_with_consecutive_values(n));
        std::size_t sum = 0;
        std::for_each(tree.begin(), tree.end(), [&](auto elem) { sum += elem.first; });
        REQUIRE(sum == (n*(n+1))/2);
    }

    SECTION("Trees have the same elements after copy construction")
    {
        auto tree1 = fill_tree_with_random_values();
        auto tree2 = tree1;
        auto it = tree1.begin();
        std::for_each(tree2.begin(), tree2.end(), [&](auto elem) {
            REQUIRE(elem.first == it->first);
            REQUIRE(elem.second == it->second);
            ++it;
        });
    }
}

TEST_CASE("AVLTree can be constructed from iterator range", "[AVLTree]")
{
    SECTION("Trees have the same elements after iterator range construction")
    {
        auto tree1 = fill_tree_with_random_values();
        AVLTree<int, int> tree2{tree1.begin(), tree1.end()};
        auto it = tree1.begin();
        std::for_each(tree2.begin(), tree2.end(), [&](auto elem) {
            REQUIRE(elem.first == it->first);
            REQUIRE(elem.second == it->second);
            ++it;
        });
    }
}

TEST_CASE("AVLTree empty function returns correct value", "[AVLTree]")
{
    SECTION("Default-constructed tree is empty")
    {
        AVLTree<int, int> tree {};
        REQUIRE(tree.empty());
    }

    SECTION("Tree with elements is not empty")
    {
        AVLTree<int, int> tree = {{1,1}, {2,2}};
        REQUIRE_FALSE(tree.empty());
    }

    SECTION("Cleared tree is empty")
    {
        AVLTree<int, int> tree = fill_tree_with_random_values();
        tree.clear();
        REQUIRE(tree.empty());
    }

    SECTION("Tree with all elements erased is empty")
    {
        AVLTree<int, int> tree = {{1,1}, {2,2}};
        tree.erase(1);
        tree.erase(2);
        REQUIRE(tree.empty());
    }
}

TEST_CASE("AVLTree size function returns correct value", "[AVLTree]")
{
    SECTION("Default-constructed tree has size of zero")
    {
        AVLTree<int, int> tree {};
        REQUIRE(tree.size() == 0);
    }

    SECTION("Tree with elements added has correct size")
    {
        num_added = 0;
        auto tree = fill_tree_with_random_values();
        REQUIRE(tree.size() == num_added);
    }

/*    SECTION("Tree with elements removed has correct size")
    {
        num_added = 0;
        auto tree = fill_tree_with_random_values();
        int i = 0;
        for (auto it = tree.begin(); i != num_added / 2; ++i) {
            it = tree.erase(it);
        }
        REQUIRE(tree.size() == num_added / 2);
    }*/

    SECTION("Cleared tree has the correct size")
    {
        auto tree = fill_tree_with_random_values();
        tree.clear();
        REQUIRE(tree.size() == 0);
    }
}

TEST_CASE("AVLTree emplace function works as expected", "[AVLTree]")
{
    SECTION("Emplace function works correctly using pair's move constructor")
    {
        AVLTree<std::string, std::string> tree {};
        tree.emplace(std::make_pair(std::string("a"), std::string("a")));
        REQUIRE(tree["a"] == "a");
    }

    SECTION("Emplace function works correctly using pair's converting move constructor")
    {
        AVLTree<std::string, std::string> tree {};
        tree.emplace(std::make_pair("b", "abcd"));
        REQUIRE(tree["b"] == "abcd");
    }

    SECTION("Emplace function works correctly using pair's template constructor")
    {
        AVLTree<std::string, std::string> tree {};
        tree.emplace("d", "ddd");
        REQUIRE(tree["d"] == "ddd");
    }
}

TEST_CASE("AVLTree insert function works as expected", "[AVLTree]")
{
    SECTION("Insertion by const lvalue reference")
    {
        SECTION("Insert function returns the correct iterator")
        {
            AVLTree<int, int> tree = {{1,1}, {2,2}, {3,3}, {7,7}, {8,8}};
            auto kv_pair = std::make_pair(6,6);
            auto return_it = tree.insert(kv_pair).first;
            REQUIRE(return_it->first == 6);
            REQUIRE(return_it->second == 6);
        }

        SECTION("Insert function returns the correct boolean value")
        {
            SECTION("Boolean value is correct when insertion succeeds")
            {
                AVLTree<int, int> tree = {{1,1}, {2,2}, {3,3}, {7,7}, {8,8}};
                auto kv_pair = std::make_pair(6,6);
                auto return_bool = tree.insert(kv_pair).second;
                REQUIRE(return_bool);
            }

            SECTION("Boolean value is correct when insertion fails")
            {
                AVLTree<int, int> tree = {{1,1}, {2,2}, {3,3}, {6,6}, {8,8}};
                auto kv_pair = std::make_pair(6,6);
                auto return_bool = tree.insert(kv_pair).second;
                REQUIRE_FALSE(return_bool);
            }
        }
    }

    SECTION("Insertion by rvalue reference")
    {
        SECTION("Insert function returns the correct iterator")
        {
            AVLTree<int, int> tree = {{1,1}, {2,2}, {3,3}, {7,7}, {8,8}};
            auto kv_pair = std::make_pair(6,6);
            auto return_it = tree.insert(std::move(kv_pair)).first;
            REQUIRE(return_it->first == 6);
            REQUIRE(return_it->second == 6);
        }

        SECTION("Insert function returns the correct boolean value")
        {
            SECTION("Boolean value is correct when insertion succeeds")
            {
                AVLTree<int, int> tree = {{1,1}, {2,2}, {3,3}, {7,7}, {8,8}};
                auto kv_pair = std::make_pair(6,6);
                auto return_bool = tree.insert(std::move(kv_pair)).second;
                REQUIRE(return_bool);
            }

            SECTION("Boolean value is correct when insertion fails")
            {
                AVLTree<int, int> tree = {{1,1}, {2,2}, {3,3}, {6,6}, {8,8}};
                auto kv_pair = std::make_pair(6,6);
                auto return_bool = tree.insert(std::move(kv_pair)).second;
                REQUIRE_FALSE(return_bool);
            }
        }
    }
}

TEST_CASE("AVLTree set_default function works as expected", "[AVLTree]")
{
    SECTION("set_default function works as expected if key already exists")
    {
        AVLTree<int, int> tree {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
        int default_val = 12;
        auto ret_val = tree.set_default(1, default_val);
        REQUIRE_FALSE(ret_val == default_val);
        REQUIRE_FALSE(tree[1] == default_val);
    }

    SECTION("set_default function works as expected if key doesn't exist")
    {
        AVLTree<int, int> tree {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
        int default_val = 12;
        auto ret_val = tree.set_default(6, default_val);
        REQUIRE(ret_val == default_val);
        REQUIRE(tree[6] == default_val);
    }
}

TEST_CASE("AVLTree find function works as expected", "[AVLTree]")
{
    SECTION("find function returns an iterator to the element if it is found")
    {
        AVLTree<int, int> tree = {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
        auto it = tree.find(2);
        REQUIRE(it->first == 2);
        REQUIRE(it->second == 7);
    }

    SECTION("find function returns an end iterator if an element is not found")
    {
        AVLTree<int, int> tree = {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
        auto it = tree.find(6);
        REQUIRE(it == tree.end());
    }
}

TEST_CASE("Subscripting works correctly for an AVLTree", "[AVLTree]")
{
    SECTION("Subscripting returns the correct value for a key in the tree")
    {
        AVLTree<int, int> tree {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
        REQUIRE(tree[3] == 1);
        REQUIRE(tree[4] == 8);
    }

    SECTION("Assignment with subscripting updates an existing key")
    {
        AVLTree<int, int> tree {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
        tree[3] = 142;
        REQUIRE(tree[3] == 142);
    }

    SECTION("Assignment with subscripting inserts a key that doesn't already exist")
    {
        AVLTree<int, int> tree {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
        tree[6] = 142;
        REQUIRE(tree[6] == 142);
    }

    SECTION("Subscripting inserts a default value if key does not exist")
    {
        AVLTree<int, int> tree {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
        tree[6];
        REQUIRE(tree[6] == 0);
    }
}

TEST_CASE("AVLTree erase function works as expected", "[AVLTree]")
{
    SECTION("Element can be erased by key")
    {
        AVLTree<int, int> tree = fill_tree_with_random_values();
        std::vector<int> keys_in_tree {};
        std::for_each(tree.begin(), tree.end(), [&keys_in_tree](auto elem){ keys_in_tree.push_back(elem.first); });
        std::vector<int> even_keys_in_tree {};
        std::transform(keys_in_tree.begin(), keys_in_tree.end(), std::back_inserter(even_keys_in_tree), [](int key) { return key % 2 == 0; });
        std::for_each(even_keys_in_tree.begin(), even_keys_in_tree.end(), [&tree](int key){ tree.erase(key); });
        std::vector<int> odd_keys_in_tree {};
        std::set_difference(keys_in_tree.begin(), keys_in_tree.end(), even_keys_in_tree.begin(),
                            even_keys_in_tree.end(), std::back_inserter(odd_keys_in_tree));
        std::for_each(odd_keys_in_tree.begin(), odd_keys_in_tree.end(), [&tree](int key){ REQUIRE(tree.find(key) != tree.end()); });
    }

    SECTION("Element can be erased by iterator")
    {
        auto either_is_odd = [](const std::pair<int, int>& keyvalue) {
            return std::abs(keyvalue.first % 2) == 1 || std::abs(keyvalue.second % 2) == 1;
        };
        auto both_are_even = [](const std::pair<int, int>& keyvalue) {
            return keyvalue.first % 2 == 0 && keyvalue.second % 2 == 0;
        };
        AVLTree<int, int> tree = fill_tree_with_random_values();
        for (auto it = tree.begin(); it != tree.end(); ) {
            if (either_is_odd(*it)) {
                it = tree.erase(it);
            } else {
                ++it;
            }
        }
        std::for_each(tree.begin(), tree.end(), [&](auto elem){ REQUIRE(both_are_even(elem)); });
    }
}