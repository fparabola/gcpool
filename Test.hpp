//
// Created by su on 2022/3/17.
//

#ifndef MYPOOL_TEST_HPP
#define MYPOOL_TEST_HPP

#include "Pool.h"
#include "FixSizePool.hpp"

class Test {

public:

    void testFixSizePool() {
        delete new Chunk;
    }

    void testPool() {
        Pool pool;
        auto mem = pool.alloc(100);
        auto mem1 = pool.alloc(100);
        auto mem2 = pool.alloc(100);
        pool.free(mem);
        pool.free(mem1);
        pool.free(mem2);
    }

    void testAll() {
//        testFixSizePool();
        testPool();
    }

private:
//    Pool pool;
};

#endif //MYPOOL_TEST_HPP
