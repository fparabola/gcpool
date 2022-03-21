//
// Created by su on 2022/3/17.
//

#ifndef MYPOOL_TEST_HPP
#define MYPOOL_TEST_HPP

#include "common.h"
#include "Pool.h"

class Test {

public:
    void testUpround() {
        assert(2 == pool.sizeupround(2));
        assert(8 == pool.sizeupround(8));
        assert(16 == pool.sizeupround(9));
        assert(256 == pool.sizeupround(255));
    }

private:
    Pool pool;
};


#endif //MYPOOL_TEST_HPP
