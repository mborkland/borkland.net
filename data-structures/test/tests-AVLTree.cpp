#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include "../../catch/catch.hpp"
#include "../src/AVLTree.hpp"

using bork_lib::AVLTree;

std::random_device rd;
constexpr int max_key_value = 100000;
constexpr int max_val_value = 25000;
constexpr int num_insertions = 50000;

AVLTree<int, int> func_that_copies_tree(AVLTree<int, int> tree)
{
    return tree;
}

void copy_constructor_test()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};

    AVLTree<int, int> orig_tree;
    for (int i = 0; i < num_insertions; ++i) {
        orig_tree.insert({key_dist(mt), val_dist(mt)});
    }

    AVLTree<int, int> copied_tree{orig_tree};
    std::string orig_tree_string {};
    std::string copied_tree_string {};

    for (const auto& x : orig_tree) {
        orig_tree_string.append(std::to_string(x.first + x.second));
    }

    for (const auto& x : copied_tree) {
        copied_tree_string.append(std::to_string(x.first + x.second));
    }

    assert(orig_tree_string == copied_tree_string && "Copy construction failed.\n");
}

void copy_assignment_test()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};

    AVLTree<int, int> orig_tree;
    for (int i = 0; i < num_insertions; ++i) {
        orig_tree.insert({key_dist(mt), val_dist(mt)});
    }

    AVLTree<int, int> copied_tree = {{1,4}, {5,38}, {-45, 106}, {98, 2}, {-1024, 1398}};
    copied_tree = orig_tree;
    std::string orig_tree_string {};
    std::string copied_tree_string {};

    for (const auto& x : orig_tree) {
        orig_tree_string.append(std::to_string(x.first + x.second));
    }

    for (const auto& x : copied_tree) {
        copied_tree_string.append(std::to_string(x.first + x.second));
    }

    assert(orig_tree_string == copied_tree_string && "Copy assignment failed.\n");
}

void construct_from_iterator_range_test()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};

    AVLTree<int, int> orig_tree;
    for (int i = 0; i < num_insertions; ++i) {
        orig_tree.insert({key_dist(mt), val_dist(mt)});
    }

    AVLTree<int, int> copied_tree(orig_tree.begin(), orig_tree.end());
    std::string orig_tree_string {};
    std::string copied_tree_string {};

    for (const auto& x : orig_tree) {
        orig_tree_string.append(std::to_string(x.first + x.second));
    }

    for (const auto& x : copied_tree) {
        copied_tree_string.append(std::to_string(x.first + x.second));
    }

    assert(orig_tree_string == copied_tree_string && "Copy construction failed.\n");
}

void move_constructor_test()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};

    AVLTree<int, int> orig_tree;
    for (int i = 0; i < num_insertions; ++i) {
        orig_tree.insert({key_dist(mt), val_dist(mt)});
    }

    AVLTree<int, int> copied_tree{std::move(func_that_copies_tree(orig_tree))};
    std::string orig_tree_string {};
    std::string copied_tree_string {};

    for (const auto& x : orig_tree) {
        orig_tree_string.append(std::to_string(x.first + x.second));
    }

    for (const auto& x : copied_tree) {
        copied_tree_string.append(std::to_string(x.first + x.second));
    }

    assert(orig_tree_string == copied_tree_string && "Move construction failed.\n");
}

void move_assignment_test()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};

    AVLTree<int, int> orig_tree;
    for (int i = 0; i < num_insertions; ++i) {
        orig_tree.insert({key_dist(mt), val_dist(mt)});
    }

    AVLTree<int, int> copied_tree = {{1,4}, {5,38}, {-45, 106}, {98, 2}, {-1024, 1398}};
    copied_tree = std::move(func_that_copies_tree(orig_tree));
    std::string orig_tree_string {};
    std::string copied_tree_string {};

    for (const auto& x : orig_tree) {
        orig_tree_string.append(std::to_string(x.first + x.second));
    }

    for (const auto& x : copied_tree) {
        copied_tree_string.append(std::to_string(x.first + x.second));
    }

    assert(orig_tree_string == copied_tree_string && "Move assignment failed.\n");
}

void empty_clear_and_size_test()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};

    AVLTree<int, int> orig_tree;
    int num_actual_insertions {};
    for (int i = 0; i < num_insertions; ++i) {
        if (orig_tree.insert({key_dist(mt), val_dist(mt)}).second) {
            ++num_actual_insertions;
        }
    }

    assert(orig_tree.size() == num_actual_insertions && "Size test failed.\n");

    orig_tree.clear();
    assert(orig_tree.size() == 0 && "Clear test failed.\n");
    assert(orig_tree.empty() && "Empty test failed.\n");
}

void insert_test()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};

    std::unordered_set<int> unique_keys {};
    for (int i = 0; i < num_insertions; ++i) {
        unique_keys.insert(key_dist(mt));
    }

    std::vector<std::pair<int, int>> pairs_to_insert {};
    for (const auto& x : unique_keys) {
        pairs_to_insert.emplace_back(x, val_dist(mt));
    }

    int total_vec_sum {};
    std::for_each(pairs_to_insert.begin(), pairs_to_insert.end(), [&total_vec_sum](auto elem){ total_vec_sum += elem.first + elem.second; });

    AVLTree<int, int> tree1 {};
    for (const auto& x : pairs_to_insert) {
        tree1.insert(x);
    }

    int total_tree_sum {};
    std::for_each(tree1.begin(), tree1.end(), [&total_tree_sum](auto elem){ total_tree_sum += elem.first + elem.second; });

    assert(total_tree_sum == total_vec_sum && "Insertion of items failed.\n");

    AVLTree<int, int> tree2 = {{1,1}, {2,2}, {3,3}, {4,4}, {5,5}};
    bool did_insert_first_attempt = tree2.insert({1,3}).second;
    bool did_insert_second_attempt = tree2.insert({3,5}).second;

    assert(!did_insert_first_attempt && "First duplicate insertion check failed.\n");
    assert(!did_insert_second_attempt && "Second duplicate insertion check failed.\n");
    assert(tree2.size() == 5 && "Duplicate insertion size check failed.\n");

    AVLTree<int, int> tree3 {};
    int n = 21;
    for (int i = 0; i < n; ++i) {
        if (i % 2 == 0) {
            tree3.insert({i,i});
        }
    }
    for (auto it = tree3.begin(); it != tree3.end(); ++it) {
        int odd = it->first + 1;
        it = tree3.insert({odd, odd}).first;
    }
    int key_sum {};
    std::for_each(tree3.begin(), tree3.end(), [&key_sum](auto elem) { key_sum += elem.first; });
    assert(key_sum == (n*(n+1))/2 && "Insertion return iterator check failed.\n");
}

void set_default_test()
{
    AVLTree<int, int> tree {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
    int default_val = 12;
    int first_check = tree.set_default(1, default_val);
    int second_check = tree.set_default(6, default_val);
    assert(first_check != default_val && "First set_default return value test failed.\n");
    assert(tree[1] != default_val && "First set_default insertion test failed.\n");
    assert(second_check == default_val && "Second set_default return value test failed.\n");
    assert(tree[6] == default_val && "Second set_default insertion test failed.\n");
}

void find_test()
{
    AVLTree<int, int> tree {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
    auto first_check = tree.find(2);
    auto second_check = tree.find(-17);
    assert(first_check->first == 2 && "Find with element found test failed.\n");
    assert(first_check->second == 7 && "Find with element found test failed.\n");
    assert(second_check == tree.end() && "Find with element not found test failed.\n");
}

void subscript_test()
{
    AVLTree<int, int> tree {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
    assert(tree[3] == 1 && "Subscript without modification test failed.\n");
    tree[3] = 5;
    assert(tree[3] == 5 && "Subscript with modification test failed.\n");
}

void const_subscript_test()
{
    const AVLTree<int, int> tree {{1,4}, {2,7}, {3,1}, {4,8}, {5,2}};
    assert(tree[3] == 1 && "Const subscript without modification test failed.\n");
    //tree[3] = 5;   // if uncommented, compile-time error
}

void erase_by_key_test()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};

    std::set<int> unique_keys {};
    for (int i = 0; i < num_insertions; ++i) {
        unique_keys.insert(key_dist(mt));
    }

    AVLTree<int, int> tree {};
    for (const auto& x : unique_keys) {
        tree.insert({x, val_dist(mt)});
    }

    std::vector<int> keys_inserted {unique_keys.begin(), unique_keys.end()};
    std::vector<int> keys_deleted {};
    std::uniform_int_distribution<> deleted_key_dist{-max_key_value, max_key_value};
    for (int i = 0; i < num_insertions / 2; ++i) {
        int key_to_delete = deleted_key_dist(mt);
        if (tree.erase(key_to_delete)) {
            keys_deleted.emplace_back(key_to_delete);
        }
    }

    std::vector<int> keys_remaining {};
    for (const auto& x : tree) {
        keys_remaining.emplace_back(x.first);
    }

    std::sort(keys_inserted.begin(), keys_inserted.end());
    std::sort(keys_deleted.begin(), keys_deleted.end());
    std::vector<int> diff {};
    std::set_difference(keys_inserted.begin(), keys_inserted.end(),
                        keys_deleted.begin(), keys_deleted.end(), std::back_inserter(diff));

    assert(keys_remaining == diff && "Erase by key test failed.\n");
}

bool either_is_odd(std::pair<int, int> keyvalue)
{
    return std::abs(keyvalue.first % 2) == 1 || std::abs(keyvalue.second % 2) == 1;
}

bool both_are_even(std::pair<int, int> keyvalue)
{
    return keyvalue.first % 2 == 0 && keyvalue.second % 2 == 0;
}

void erase_by_iter_test()
{
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};

    AVLTree<int, int> tree {};
    for (int i = 0; i < 20; ++i) {
        tree.insert({key_dist(mt), val_dist(mt)});
    }

    for (auto it = tree.begin(); it != tree.end(); ) {
        if (either_is_odd(*it)) {
            it = tree.erase(it);
        } else {
            ++it;
        }
    }

    for (const auto& x : tree) {
        assert(both_are_even(x) && "Erase by iterator test failed.\n");
    }
}

int main()
{
    copy_constructor_test();
    copy_assignment_test();
    construct_from_iterator_range_test();
    move_constructor_test();
    move_assignment_test();
    empty_clear_and_size_test();
    insert_test();
    set_default_test();
    find_test();
    subscript_test();
    const_subscript_test();
    erase_by_key_test();
    erase_by_iter_test();
    std::cout << "All tests passed.\n";
}