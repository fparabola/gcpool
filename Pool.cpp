//
// Created by su on 2022/3/17.
//

#include "Pool.h"

void* Pool::alloc(size_t bytes) {
    if(0 == bytes) {
        LOG_PRINT(LOG_ERROR, "Attempt to allocate 0 byte memory");
        throw -1;
    }
    size_t order = size2order(bytes + sizeof(MemTag));
    size_t scanorder = order;
    auto chunk = this->freearea[scanorder];
    for(; scanorder <= MAX_ORDER; ++scanorder) {
        chunk = this->freearea[scanorder];
        if(chunk) {
            break;
        }
    }
    if (chunk) {
        // split chunk, until it's order is matched
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
                newchunk->prev = chunk;
                newchunk->next = nullptr;
                auto head = this->freearea[neworder];
                if(!head) {
                    this->freearea[neworder] = chunk;
                } else {
                    newchunk->next = head;
                    head->prev = newchunk;
                    this->freearea[neworder] = chunk;
                }
                return chunk;
            };
            while(order != chunk->order) {
                chunk = split(chunk);
            }
        }
        // to allocate this chunk, remove from freearea
        auto head = this->freearea[order];
        if(head->next) {
            this->freearea[order] = head->next;
            head->prev = nullptr;
        } else {
            this->freearea[order] = nullptr;
        }
    } else {
        LOG_PRINT(LOG_ERROR, "Can not find suitable chunk to allocate");
        throw -1;
    }
    assert(chunk);
    LOG_PRINT(LOG_DEBUG, "Found a chunk: [addr: %p], [idx: %lu], [next: %p], [mem: %p], [order: %lu]", chunk, chunk->idx, chunk->next, chunk->mem, chunk->order);
    assert(MemTag::astaginfo(chunk->mem).chunk == chunk);
    // add to usedarea
    chunk->prev = nullptr;
    auto head = this->usedarea;
    if(!head) {
        head = chunk;
        head->next = nullptr;
        this->usedarea = head;
    } else {
        chunk->next = head;
        head->prev = chunk;
        this->usedarea = chunk;
    }
    chunk->inuse = true;
#ifdef DEBUG_ALLOC
    for(auto i=0; i<=MAX_ORDER; ++i) {
        auto count = 0;
        auto current = this->freearea[i];
        while(current) {
            ++count;
            current = current->next;
        }
        LOG_PRINT(LOG_DEBUG, "(count) freearea[%lu]: %lu", i, count);
    }
    auto used = this->usedarea;
    auto count = 0;
    while(used) {
        ++count;
        used = used->next;
    }
    LOG_PRINT(LOG_DEBUG, "(count) userarea: %lu", count);
#endif
    return pointer_cast<void*>(chunk->mem + sizeof(MemTag));
}

void Pool::free(void* mem) {
    MemTag memtag = MemTag::astaginfo(static_cast<char*>(mem) - sizeof(MemTag));
    auto chunk = memtag.chunk;
    if(chunk->inuse) {
        // remove chunk from usedarea
        if(chunk->prev && chunk->next) {
            chunk->prev->next = chunk->next;
        }
        if(chunk->prev && !chunk->next) {
            chunk->prev->next = nullptr;
        }
        if(!chunk->prev && chunk->next) {
            this->usedarea = chunk->next;
        }
        // find buddy to be merged with
        while(chunk->order < MAX_ORDER) {
            auto buddy = Chunk::findbuddy(chunk);
            if(buddy->order != chunk->order) {
                break;
            }
            if(buddy->inuse) {
                break;
            }
#ifdef DEBUG
            auto minidx = std::min(chunk->idx, buddy->idx);
            auto maxidx = std::max(chunk->idx, buddy->idx);
            auto minmem = std::min(chunk->mem, buddy->mem);
            auto maxmem= std::max(chunk->mem, buddy->mem);
            auto chunksize = 1 << chunk->order;
            assert(minmem + chunksize == maxmem);
            assert(minidx + chunksize == maxidx);
            assert(chunk->order == buddy->order);
#endif
            LOG_PRINT(LOG_DEBUG, "About to merge chunk: ");
            LOG_PRINT(LOG_DEBUG, "chunk: [order:%lu], [mem:%p], [idx:%lu]", chunk->order, chunk->mem, chunk->idx);
            LOG_PRINT(LOG_DEBUG, "chunk: [order:%lu], [mem:%p], [idx:%lu]", buddy->order, buddy->mem, buddy->idx);
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
            chunk->mem = std::min(chunk->mem, buddy->mem);
            chunk->idx = std::min(chunk->idx, buddy->idx);
            chunk->inuse = false;
            chunk->prev = nullptr;
            chunk->next = nullptr;
            delete buddy;
            LOG_PRINT(LOG_DEBUG, "merged: [order:%lu], [mem:%p], [idx:%lu]", chunk->order, chunk->mem, chunk->idx);
            assert((chunk->mem - this->mem) == chunk->idx);
        }
        // add merged chunk to freearea
        chunk->inuse = false;
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
            LOG_PRINT(LOG_DEBUG, "chunk: [%p], prev: [%p]", newchunk, prev);
            prev->next = newchunk;
            newchunk->prev = prev;
        } else {
            head = newchunk;
        }
#ifdef DEBUG
        if(i > 0 && i % 2 == 1) {
            assert(Chunk::findbuddy(newchunk) == prev);
        }
#endif
        prev = newchunk;
    }
    this->freearea[MAX_ORDER] = head;

#ifdef DEBUG
    LOG_PRINT(LOG_DEBUG, "*********************************************");
    for(auto i=0; i<=MAX_ORDER; ++i) {
        auto count = 0;
        auto current = this->freearea[i];
        while(current) {
            ++count;
            current = current->next;
        }
        LOG_PRINT(LOG_DEBUG, "(count) freearea[%lu]: %lu", i, count);
    }
    LOG_PRINT(LOG_DEBUG, "*********************************************");
    for(auto i=0; i<N_AREA; ++i) {
        auto current = this->freearea[i];
        LOG_PRINT(LOG_DEBUG, "Pool free area[%lu]:", i);
        if(!current) {
            LOG_PRINT(LOG_DEBUG, "empty");
            continue;
        }
        auto count = 0;
        while(current) {
            LOG_PRINT(LOG_DEBUG, "Node(%lu): [addr: %p], [inuse: %d], [idx: %lu], [prev: %p] [next: %p], [mem: %p], [order: %lu]", ++count, current, current->inuse, current->idx, current->prev, current->next, current->mem, current->order);
            assert(MemTag::astaginfo(current->mem).chunk == current);
            current = current->next;
        }
        LOG_PRINT(LOG_DEBUG, "*********************************************");
    }
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
