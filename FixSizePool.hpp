//
// Created by su on 2022/3/19.
//

#ifndef MYPOOL_FIXSIZEPOOL_HPP
#define MYPOOL_FIXSIZEPOOL_HPP

#include "common.h"

template<class T>
struct FixChunk {
    FixChunk* next;
    FixChunk* tag;
    T obj;

    FixChunk();
    FixChunk(const FixChunk<T>&) = delete;
    static size_t offset();
    static FixChunk<T>* getchunk(T*);
};

template<class T>
class FixSizePool {
public:
    FixSizePool(size_t);
    ~FixSizePool();
    T* allocone();
    void free(T*);

private:
    FixChunk<T>* freelist;
    FixChunk<T>* mem;
    size_t nchunk;
};

// ---------------------FixChunk-----------------------------
template<class T>
FixChunk<T>::FixChunk(): next(nullptr) {}

template<class T>
size_t FixChunk<T>::offset() {
    static auto fixchunk = new FixChunk<T>();
    return pointer_cast<char*>(&fixchunk->obj) - pointer_cast<char*>(&fixchunk->tag);
}

template<class T>
FixChunk<T> *FixChunk<T>::getchunk(T * p) {
    auto tagaddr = pointer_cast<char*>(p) - FixChunk<T>::offset();
    return *pointer_cast<FixChunk<T>**>(tagaddr);
}

// ---------------------FixSizePool--------------------------
template<class T>
T *FixSizePool<T>::allocone() {
    auto current = this->freelist;
    if(current) {
        if(current->next) {
            this->freelist = current->next;
        } else {
            this->freelist = nullptr;
        }
        LOG_PRINT(LOG_ERROR, "Fix chunk found: [addr: %p], [objaddr: %p]", current, &current->obj);
        return &current->obj;
    }
    LOG_PRINT(LOG_ERROR, "Allocate fix chunk fail: no free chunk");
    throw -1;
}

template<class T>
void FixSizePool<T>::free(T * obj) {
    auto fixchunk = FixChunk<T>::getchunk(obj);
    auto head = this->freelist;
    if(head) {
        fixchunk->next = head;
    }
    this->freelist = fixchunk;
}
template<class T>
FixSizePool<T>::FixSizePool(size_t nchunk): nchunk(nchunk) {
    LOG_PRINT(LOG_DEBUG, "FixChunk offset: %lu bytes", FixChunk<T>::offset());
    LOG_PRINT(LOG_DEBUG, "About to new %lu fix chunks", nchunk);
    auto chunks = new FixChunk<T>[nchunk];
    this->mem = chunks;
    FixChunk<T>* prev = nullptr;
    for(auto i = 0; i < nchunk; ++i) {
        chunks[i].tag = &chunks[i];
        if(prev) {
            prev->next = &chunks[i];
        }
        prev = &chunks[i];
    }
    this->freelist = chunks;
#ifdef DEBUG
    auto count = 0;
    auto current = this->freelist;
    while(current) {
        LOG_PRINT(LOG_DEBUG, "Node(%lu): [addr: %p], [next: %p], [objaddr: %p], [tag: %p], [getchunk: %p]", ++count, current, current->next, &current->obj, current->tag,
                  FixChunk<T>::getchunk(&current->obj));
        if(current->next) {
            assert(pointer_cast<char*>(current) + sizeof(T) <= pointer_cast<char*>(current+1));
            assert(FixChunk<T>::getchunk(&current->obj) == current);
        }
        current = current->next;
    }
#endif
}

template<class T>
FixSizePool<T>::~FixSizePool() {
    delete[] mem;
}

#endif //MYPOOL_FIXSIZEPOOL_HPP
