//
// Created by su on 2022/3/17.
//

#ifndef MYPOOL_CHUNK_H
#define MYPOOL_CHUNK_H

#include "common.h"

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
    Chunk(const Chunk&) = delete;
};

struct MemTag {
    MemTag(Chunk* chunk) : chunk(chunk) {};
    Chunk* chunk;

    static void tag(Chunk*);
    template<class T>
    static MemTag& astaginfo(T*);
};

#endif //MYPOOL_CHUNK_H
