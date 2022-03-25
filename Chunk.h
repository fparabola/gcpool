//
// Created by su on 2022/3/17.
//

#ifndef MYPOOL_CHUNK_H
#define MYPOOL_CHUNK_H

#include <vector>
#include "common.h"
#include "FixSizePool.hpp"

struct MemTag;
struct Chunk {
    size_t idx;
    Chunk* prev;
    Chunk* next;
    size_t order;
    bool inuse;
    char* mem;

    static Chunk* findbuddy(Chunk*);
    MemTag& taginfo();
    Chunk();
    ~Chunk();
    Chunk(const Chunk&) = delete;
    void* operator new(size_t);
    void operator delete(void*);
    void* operator new[](size_t) = delete;
    void operator delete[](void*) = delete;

private:
    static const size_t nchunks = 1 << 20;
    static FixSizePool<Chunk> fixpool;
};


struct MemTag {
    MemTag(Chunk* chunk) : chunk(chunk) {};
    Chunk* chunk;

    static void tag(Chunk*);
    static MemTag& astaginfo(void*);
};

#endif //MYPOOL_CHUNK_H
