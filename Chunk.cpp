//
// Created by su on 2022/3/19.
//

#include "Chunk.h"

FixSizePool<Chunk> Chunk::fixpool(nchunks);

Chunk::Chunk()
: inuse(false), prev(nullptr), next(nullptr), mem(nullptr) {}

MemTag& Chunk::taginfo() {
    return *static_cast<MemTag*>(static_cast<void*>(this->mem));
}

Chunk* Chunk::findbuddy(Chunk* chunk) {
    auto idx = chunk->idx ^ (1 << chunk->order);
    auto mem = chunk->mem + chunk->idx - idx;
    return MemTag::astaginfo(mem).chunk;
}

void *Chunk::operator new(size_t) {
    return pointer_cast<void*>(fixpool.allocone());
}

void Chunk::operator delete(void * p) {
    fixpool.free(pointer_cast<Chunk*>(p));
}

template<class T>
MemTag& MemTag::astaginfo(T * p) {
    return memasref<MemTag>(p);
}

void MemTag::tag(Chunk* chunk) {
    chunk->taginfo() = MemTag(chunk);
}