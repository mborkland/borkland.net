#include <cassert>
#include <iostream>
#include <random>
#include <unordered_set>
#include <string>
#include <vector>
#include "../AVLTree.hpp"

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
    std::mt19937 mt(rd());
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
    std::mt19937 mt(rd());
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

void move_constructor_test()
{
    std::mt19937 mt(rd());
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
    std::mt19937 mt(rd());
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
    std::mt19937 mt(rd());
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
    std::mt19937 mt(rd());
    std::uniform_int_distribution<> key_dist{-max_key_value, max_key_value};
    std::uniform_int_distribution<> val_dist{-max_val_value, max_val_value};

    std::unordered_set<int> unique_values {};
    for (int i = 0; i < num_insertions; ++i) {
        unique_values.insert(key_dist(mt));
    }

    std::vector<std::pair<int, int>> pairs_to_insert {};
    for (const auto& x : unique_values) {
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

    AVLTree<int, int> tree3 {};
    for (int i = 0; i <= 20; ++i) {
        if (i % 2 == 0) {
            tree3.insert({i,i});
        }
    }

    for (auto it = tree3.begin(); it != tree3.end(); ++it) {
        // TODO: insertion iterator test;
    }
}

void set_default_test()
{

}

void find_test()
{

}

void subscript_test()
{

}

void const_subscript_test()
{

}

void erase_by_key_test()
{

}

void erase_by_iter_test()
{

}

int main()
{
    copy_constructor_test();
    copy_assignment_test();
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