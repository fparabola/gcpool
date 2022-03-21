//
// Created by su on 2022/3/19.
//

#include "Chunk.h"

Chunk::Chunk(): inuse(false), prev(nullptr), next(nullptr), mem(nullptr) {}

MemTag& Chunk::taginfo() {
    return *static_cast<MemTag*>(static_cast<void*>(this->mem));
}

Chunk* Chunk::findbuddy(Chunk* chunk) {
    auto idx = chunk->idx ^ (1 << chunk->order);
    auto mem = chunk->mem + chunk->idx - idx;
    return MemTag::astaginfo(mem).chunk;
}

template<class T>
MemTag& MemTag::astaginfo(T * p) {
//    return *static_cast<MemTag*>(static_cast<void*>(p));
    return memasref<MemTag>(p);
}

void MemTag::tag(Chunk* chunk) {
    chunk->taginfo() = MemTag(chunk);
}