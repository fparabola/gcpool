//
// Created by su on 2022/3/17.
//

#ifndef MYPOOL_TEST_HPP
#define MYPOOL_TEST_HPP

#include "Pool.h"
#include "FixSizePool.hpp"
#include <chrono>

class Test {

public:

    void testPool() {
        std::vector<int> v;
        std::vector<void*> pm;
        std::vector<void*> mm;
        for(auto i=0; i<1000; ++i) {
            std::srand(std::time(nullptr));
            for(auto j=0; j<500; ++j) {
//                v.push_back(1 + std::rand() % MAX_ALLOC);
//                v.push_back(2500);
//                v.push_back(MAX_ALLOC / 2 - 100);
                v.push_back(MAX_ALLOC - 100);
            }
        }

        Pool pool;
        auto it = v.begin();
        auto begin = std::chrono::system_clock::now();
        auto count = 0;
        while(it != v.end()) {
            pm.push_back(pool.alloc(*it));
            it++;
            std::cout << ++count << std::endl;
        }
        auto end = std::chrono::system_clock::now();
        std::cout << "Pool: \t\t" << end.time_since_epoch().count() - begin.time_since_epoch().count() << std::endl;

        it = v.begin();
        begin = std::chrono::system_clock::now();
        while(it != v.end()) {
            mm.push_back(malloc(*it));
            it++;
        }
        end = std::chrono::system_clock::now();
        std::cout << "Malloc: \t" << end.time_since_epoch().count() - begin.time_since_epoch().count() << std::endl;
    }

    void testAll() {
        testPool();
    }

private:
//    Pool pool;
};

#endif //MYPOOL_TEST_HPP
