//
// Created by su on 2022/3/17.
//

#include "Pool.h"

template<class T>
T* Pool::alloc(size_t bytes) {
    if(0 == bytes) {
        LOG_PRINT(LOG_ERROR, "Attempt to allocate 0 byte memory");
        throw -1;
    }
    size_t order = size2order(bytes + sizeof(MemTag));
    size_t scanorder = order;
    auto chunk = this->freearea[scanorder];
    for(; order <= MAX_ORDER; ++scanorder) {
        chunk = this->freearea[scanorder];
        if(chunk) {
            break;
        }
    }
    if (chunk) {
        // split chunk, until it's order match
        if(scanorder != order) {
            auto split = [this](Chunk * chunk) -> Chunk* {
                auto neworder = chunk->order - 1;
                auto newchunk = new Chunk();
                auto newchunksize = order2size(neworder);
                newchunk->mem = chunk->mem + newchunksize;
                MemTag::tag(newchunk);
                newchunk->idx = chunk->idx + newchunksize;
                newchunk->order = neworder;
                chunk->order = neworder;
                // remove from upper area
                if(chunk->next) {
                    this->freearea[neworder + 1] = chunk->next;
                    this->freearea[neworder + 1]->prev = nullptr;
                } else {
                    this->freearea[neworder + 1] = nullptr;
                }
                // add to lower area
                chunk->prev = nullptr;
                chunk->next = newchunk;
                if(!this->freearea[neworder]) {
                    this->freearea[neworder] = chunk;
                } else {
                    auto head = this->freearea[neworder];
                    newchunk->next = head;
                    head->prev = chunk;
                    this->freearea[neworder] = chunk;
                }
                return chunk;
            };
            while(order != chunk->order) {
                chunk = split(chunk);
            }
        }
        // to allocate this chunk, remove from freearea
        if(chunk->next) {
            freearea[scanorder] = chunk->next;
            freearea[scanorder]->prev = nullptr;
        } else {
            freearea[scanorder] = nullptr;
        }
    } else {
        LOG_PRINT(LOG_ERROR, "Can not find suitable chunk to allocate");
        throw -1;
    }
    assert(chunk);
    LOG_PRINT(LOG_DEBUG, "Found a chunk: [addr: %p], [idx: %lu], [next: %p], [mem: %p], [order: %lu]", chunk, chunk->idx, chunk->next, chunk->mem, chunk->order);
    // add to usedarea
    chunk->prev = nullptr;
    if(!this->usedarea) {
        this->usedarea = chunk;
        chunk->next = nullptr;
    } else {
        chunk->next = usedarea;
        usedarea->prev = chunk;
        usedarea = chunk;
    }
    return static_cast<T*>(chunk->mem + sizeof(MemTag));
}

template<class T>
void Pool::free(T* mem) {
    MemTag memtag = MemTag::astaginfo(mem);
    auto chunk = memtag.chunk;
    auto buddy = Chunk::findbuddy(chunk);
    if(chunk->inuse) {
        while(!buddy->inuse) {
            LOG_PRINT(LOG_DEBUG, "About to merge chunk: ");
            LOG_PRINT(LOG_DEBUG, "chunk1: [order:%lu], [mem:%p], [idx:%lu]", chunk->order, chunk->mem, chunk->idx);
            LOG_PRINT(LOG_DEBUG, "chunk1: [order:%lu], [mem:%p], [idx:%lu]", buddy->order, buddy->mem, buddy->idx);
            // remove buddy from freearea
            auto prev = buddy->prev;
            auto next = buddy->next;
            if(prev && next) {
                prev->next = next;
            } else if(!prev && next) {
                this->freearea[buddy->order] = buddy->next;
            } else if(prev && !next) {
                prev->next = nullptr;
            } else {
                this->freearea[buddy->order] = nullptr;
            }
            // merge chunk and buddy
            auto bytes = order2size(chunk->order);
            chunk->order += 1;
            chunk->mem += bytes;
            chunk->idx += bytes;
            chunk->inuse = false;
            chunk->prev = nullptr;
            chunk->next = nullptr;
            delete buddy;
            LOG_PRINT(LOG_DEBUG, "merged: [order:%lu], [mem:%p], [idx:%lu]", chunk->order, chunk->mem, chunk->idx);
        }
        // add merged chunk to freearea
        auto insertarea = this->freearea[chunk->order];
        if(insertarea) {
            chunk->next = insertarea;
            insertarea->prev = chunk;
            insertarea = chunk;
        } else {
            insertarea = chunk;
        }
    } else {
        LOG_PRINT(LOG_ERROR, "Free memory wrong: %p", mem);
        throw -1;
    }
}

Pool::Pool(): usedarea(nullptr) {
    memset(this->freearea, 0, sizeof(Chunk*) * N_AREA);
    LOG_PRINT(LOG_DEBUG, "About to init pool: [POOL_SIZE: %lu], [MAX_ALLOC: %lu]", POOL_SIZE, MAX_ALLOC);
    if(POOL_SIZE < MAX_ALLOC) {
        LOG_PRINT(LOG_ERROR, "POOL_SIZE < MAX_ALLOC, init pool fail");
        throw -1;
    }
    if(MAX_ALLOC <= sizeof(MemTag)) {
        LOG_PRINT(LOG_ERROR, "Max allocating size is smaller than header tag size");
        throw -1;
    }
    this->mem = new char[POOL_SIZE];
    LOG_PRINT(LOG_DEBUG, "Init pool from %p to %p", mem, mem + POOL_SIZE);

    auto nmaxchunks = POOL_SIZE / MAX_ALLOC;
    LOG_PRINT(LOG_DEBUG, "About to prepare %lu max chuncks", nmaxchunks);
    int i = nmaxchunks;
    Chunk* head = nullptr;
    Chunk* prev = nullptr;
    for(auto i = 0; i < nmaxchunks; ++i) {
        auto newchunk = new Chunk();
        newchunk->order = MAX_ORDER;
        newchunk->mem = this->mem + i * MAX_ALLOC;
        newchunk->idx = i * MAX_ALLOC;
        MemTag::tag(newchunk);
        if(prev) {
            prev->next = newchunk;
            newchunk->prev = prev;
        } else {
            head = newchunk;
        }
        prev = newchunk;
    }
    this->freearea[MAX_ORDER] = head;

#ifdef DEBUG
    LOG_PRINT(LOG_DEBUG, "Init freelist on freearea[%u]:", MAX_ORDER);
    auto debugchunk = this->freearea[MAX_ORDER];
    auto count = 0;
    while(debugchunk) {
        LOG_PRINT(LOG_DEBUG, "freelist node(%lu): [addr: %p], [inuse: %d], [idx: %lu], [prev: %p] [next: %p], [mem: %p], [order: %lu]", count, debugchunk, debugchunk->inuse, debugchunk->idx, debugchunk->prev, debugchunk->next, debugchunk->mem, debugchunk->order);
        debugchunk = debugchunk->next;
        ++count;
    }
    assert(count == POOL_SIZE / MAX_ALLOC);
#endif
}

Pool::~Pool() {
    delete[] mem;
}

size_t Pool::size2order(size_t bytes) {
    size_t hibit = std::__lg(bytes);
    size_t lowround = 1 << hibit;
    return lowround == bytes ? hibit : hibit + 1;
}

size_t Pool::order2size(size_t order) {
    return 1 << order;
}

size_t Pool::sizeupround(size_t bytes) {
    size_t hibit = std::__lg(bytes);
    size_t lowround = 1 << hibit;
//    LOG_PRINT(LOG_DEBUG, "%lu left lowround to %lu", bytes, lowround == bytes ? lowround : lowround << 1);
    return lowround == bytes ? lowround : lowround << 1;
}
