//
// Created by su on 2022/3/19.
//

#include "FixSizePool.h"
// ---------------------MemTagger---------------------------
template<class Tag, class Obj>
void *MemTagger<Tag, Obj>::operator new(size_t n) {
    LOG_PRINT(LOG_DEBUG, "New MemTagger: [Tag:%s], [Ojb:%s], Allocate %lu bytes", typeid(Tag).name(), typeid(Obj).name(), n + sizeof(Tag));
    return ::operator new(n + sizeof(Tag));
}

template<class Tag, class Obj>
void *MemTagger<Tag, Obj>::operator new(size_t n, void * mem) {
    memasref<Tag>(mem) = Tag();
    return static_cast<void*>(pointer_cast<char*>(mem) + sizeof(Tag));
}

// ---------------------FixChunk-----------------------------
template<class T>
FixChunk<T>::FixChunk(): prev(nullptr), next(nullptr) {}

// ---------------------FixSizePool--------------------------
template<class T>
FixSizePool<T>::FixSizePool(size_t nchunk): nchunk(nchunk) {
    auto chunks = new FixChunk<T>[nchunk];
    this->mem = pointer_cast<char*>(chunks);
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