//
// Created by su on 2022/3/17.
//

#ifndef MYPOOL_POOL_H
#define MYPOOL_POOL_H

#include "common.h"
#include "Chunk.h"

class Pool {

    friend class Test;

public:
    void* alloc(size_t);

    void free(void*);

public:
    Pool(size_t size, size_t maxorder);
    ~Pool();

private:
    Pool(const Pool&) = delete;
    Pool(const Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;

private:
    Chunk* usedarea;
    Chunk** freearea;

private:
    char* mem;

private:
    size_t size2order(size_t);
    size_t order2size(size_t);
    size_t sizeupround(size_t);
private:
    size_t POOL_SIZE;
    size_t MAX_ORDER;
    size_t MAX_ALLOC;
    size_t N_AREA;
};

#endif //MYPOOL_POOL_H