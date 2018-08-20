#include <iostream>
#include <stdexcept>
#include "DLinkedList.hpp"

using bork_lib::DLinkedList;

int main()
{
    DLinkedList<int> dlist;
    for (int i = 0; i < 10; ++i) {
        try {
            dlist.push_back(i);
        } catch (std::invalid_argument& e) {
            std::cout << e.what() << '\n' << "Failed on insertion of " << i << '\n';
            throw;
        }
    }
    for (auto it = dlist.begin(); it != dlist.end(); ++it) {
        std::cout << *it << ' ';
    }
}