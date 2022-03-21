//
// Created by su on 2022/3/19.
//

#ifndef MYPOOL_FIXSIZEPOOL_H
#define MYPOOL_FIXSIZEPOOL_H

#include "common.h"

template<class Tag, class Obj>
class MemTagger {
public:
    void* operator new(size_t);
    void* operator new(size_t, void*);

private:
    Tag t;
};

template<class T>
struct FixChunk {
    FixChunk* prev;
    FixChunk* next;
    T t;

    FixChunk();
    FixChunk(const FixChunk<T>&) = delete;
};

template<class T>
class FixSizePool {

public:
    FixSizePool(size_t);
    T* allocone();
    T* free(T*);

private:
    FixChunk<T>* freelist;
    size_t nchunk;
    char* mem;
};
void fuck() {
    int* pi = new int;
}
#endif //MYPOOL_FIXSIZEPOOL_H
