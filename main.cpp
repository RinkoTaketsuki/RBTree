#include "RBTree.h"
#include <iostream>
#include <random>

using Type = int;
constexpr Type MIN_VALUE = -100;
constexpr Type MAX_VALUE = 100;
constexpr size_t SIZE = 10;

RBTree<Type> getRandomRBTree() {
    std::default_random_engine e(time(nullptr));
    std::uniform_int_distribution<Type> d(MIN_VALUE, MAX_VALUE);
    RBTree<Type> t;
    for (size_t i = 0; i < SIZE; ++i) {
        t.insert(d(e));
    }
    return t;
}

int main() {
    RBTree<Type> t = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    std::cout << t.toString() << std::endl;
    t.erase(10);
    t.erase(7);
    std::cout << t.toString() << std::endl;
    return 0;
}
