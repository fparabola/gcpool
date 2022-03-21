//
// Created by su on 2022/3/19.
//

#include "FixSizePool.h"

// ---------------------FixChunk-----------------------------
template<class T>
FixChunk<T>::FixChunk(): next(nullptr) {}

template<class T>
size_t FixChunk<T>::offset() {
    static auto fixchunk = new FixChunk<T>();
    return pointer_cast<char*>(&fixchunk->obj) - pointer_cast<char*>(&fixchunk->tag);
}

// ---------------------FixSizePool--------------------------
template<class T>
FixSizePool<T>::FixSizePool(size_t nchunk): nchunk(nchunk) {
    auto chunks = new FixChunk<T>[nchunk];
//    this->mem = pointer_cast<char*>(chunks);
    for(auto i = 0; i < nchunk; ++i) {
        if(0 == i) {
            freelist = chunks[i];
            continue;
        }
        if(nchunk - 1 == i) {
            chunks[i].prev = chunks[i-1];
            continue;
        }
        chunks[i].prev = chunks[i-1];
        chunks[i].next = chunks[i+1];
    }
}

template<class T>
FixSizePool<T>::~FixSizePool() {
    delete mem;
}

template<class T>
T *FixSizePool<T>::allocone() {
    auto head = this->freelist;
    if(head) {
        if(head->next) {
            this->freelist = head.next;
        } else {
            this->freelist = nullptr;
        }
        return &head->obj;
    }
    LOG_PRINT(LOG_ERROR, "Allocate fix chunk fail: no free chunk");
}

template<class T>
T *FixSizePool<T>::free(T * obj) {
    auto tagaddr = pointer_cast<char*>(obj) - FixChunk<T>::offset();
    auto fixchunk = pointer_cast<FixChunk<T>*>(tagaddr);
    auto head = this->freelist;
    if(head) {
        fixchunk->next = head;
    }
    this->freelist = fixchunk;
    return nullptr;
}
