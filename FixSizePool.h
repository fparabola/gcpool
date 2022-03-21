//
// Created by su on 2022/3/19.
//

#ifndef MYPOOL_FIXSIZEPOOL_H
#define MYPOOL_FIXSIZEPOOL_H

#include "common.h"

template<class T>
struct FixChunk {
//    FixChunk* prev;
    FixChunk* next;
    FixChunk* tag;
    T obj;

    FixChunk();
    FixChunk(const FixChunk<T>&) = delete;
    static size_t offset();
};

template<class T>
class FixSizePool {

public:
    FixSizePool(size_t);
    ~FixSizePool();
    T* allocone();
    T* free(T*);

private:
    FixChunk<T>* freelist;
    FixChunk<T>* mem;
    size_t nchunk;
};

#endif //MYPOOL_FIXSIZEPOOL_H
