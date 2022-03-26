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
        std::srand(std::time(nullptr));
        for(auto i=0; i<1500; ++i) {
            v.push_back(1 + std::rand() % MAX_ALLOC);
            std::cout << v[i] << " ";
        }
        std::cout << std::endl;

        Pool pool;
        auto it = v.begin();
        auto begin = std::chrono::system_clock::now();
        while(it != v.end()) {
            pm.push_back(pool.alloc(*it));
            it++;
        }
        auto it2 = pm.begin();
        while(it2 != pm.end()) {
            pool.free(*it2);
            it2++;
        }
        auto end = std::chrono::system_clock::now();
        std::cout << "Pool: \t\t" << end.time_since_epoch().count() - begin.time_since_epoch().count() << std::endl;

        it = v.begin();
        begin = std::chrono::system_clock::now();
        while(it != v.end()) {
            mm.push_back(malloc(*it));
            it++;
        }
        it2 = mm.begin();
        while(it2 != mm.end()) {
            free(*it2);
            it2++;
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
