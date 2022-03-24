//
// Created by su on 2022/3/17.
//

#ifndef MYPOOL_TEST_HPP
#define MYPOOL_TEST_HPP

#include "Pool.h"
#include "FixSizePool.hpp"

class Test {

public:
//    void testUpround() {
//        assert(2 == pool.sizeupround(2));
//        assert(8 == pool.sizeupround(8));
//        assert(16 == pool.sizeupround(9));
//        assert(256 == pool.sizeupround(255));
//    }

    void testFixSizePool() {
        delete new Chunk;
    }

    void testAll() {
//        testUpround();
        testFixSizePool();
    }

private:
//    Pool pool;
};

#endif //MYPOOL_TEST_HPP
