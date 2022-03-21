//
// Created by su on 2022/3/17.
//

#ifndef MYPOOL_POOL_H
#define MYPOOL_POOL_H

#include "common.h"
#include "Chunk.h"

const size_t POOL_SIZE = 1024 * 1024 * 1024;  //1GB
const size_t MAX_ORDER = 22;
const size_t MAX_ALLOC = 1 << MAX_ORDER;
const size_t N_AREA = MAX_ORDER + 1;

class Pool {

    friend class Test;

public:
    template<class T>
    T* alloc(size_t);

    template<class T>
    void free(T*);

public:
    Pool();
    ~Pool();

private:
    Pool(const Pool&) = delete;
    Pool(const Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;

private:
    Chunk* usedarea;
    Chunk* freearea[N_AREA];

private:
    char* mem;

private:
    size_t size2order(size_t);
    size_t order2size(size_t);
    size_t sizeupround(size_t);
};

#endif //MYPOOL_POOL_H