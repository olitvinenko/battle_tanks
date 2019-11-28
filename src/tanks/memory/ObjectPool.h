#pragma once

#include "Header.h"

#include <cassert>
#include <cstring> // memset
#include <new>
#include <typeinfo>
#ifndef NDEBUG
#include <cstdio>  // printf
#endif

template
<
    typename T,
    typename THeader,
    size_t block_size = 128
>
class ObjectPool
{
    struct Block;
    struct BlankObject
    {
        char data[sizeof(T) + sizeof(THeader)];

        BlankObject* next;
        Block* block;
#ifndef NDEBUG
        bool dbgBusy;
#endif
    };

    struct Block
    {
        Block* prevFree;
        Block* nextFree;

        BlankObject blanks[block_size];
        BlankObject* firstFreeBlank;
        
        size_t used;
        size_t thisBlockIdx;

        Block(size_t idx)
            : prevFree(nullptr)
            , nextFree(nullptr)
            , firstFreeBlank(blanks)
            , used(0)
            , thisBlockIdx(idx)
        {
#ifndef NDEBUG
            memset(blanks, 0xdb, sizeof(blanks));
#endif

            // link together empty object blanks
            BlankObject* tmp(blanks);
            BlankObject* end(blanks + block_size - 1);

            while (tmp != end)
            {
                tmp->block = this;
                tmp->next = tmp + 1;

#ifndef NDEBUG
                tmp->dbgBusy = false;
#endif
                ++tmp;
            }
            end->block = this;
            end->next = nullptr;
#ifndef NDEBUG
            end->dbgBusy = false;
#endif
    }

        ~Block()
        {
            assert(used == 0);
        }

        BlankObject* Alloc()
        {
            assert(firstFreeBlank);

            ++used;

            BlankObject* tmp = firstFreeBlank;
#ifndef NDEBUG
            assert(!tmp->dbgBusy);
            tmp->dbgBusy = true;
#endif
            firstFreeBlank = firstFreeBlank->next;
            return tmp;
        }

        void Free(BlankObject* p)
        {
            assert(this == p->block);
            assert(used > 0);
#ifndef NDEBUG
            memset(p, 0xdb, sizeof(T) + sizeof(THeader));
            assert(p->dbgBusy);
            p->dbgBusy = false;
#endif
            p->next = firstFreeBlank;
            firstFreeBlank = p;
            --used;
        }

    private:
        Block() = delete;
        Block(Block&) = delete;
        Block& operator =(Block&) = delete;
  }; // struct block

    struct BlockPtr
    {
        Block* block;
        size_t nextEmptyIdx; // not a pointer to enable realloc
    };
    
public:
    ObjectPool()
        : m_blocks((BlockPtr*)malloc(sizeof(BlockPtr)))
        , m_freeBlock(nullptr)
        , m_blockCount(1)
        , m_firstEmptyIdx(0)
#ifndef NDEBUG
        , m_allocatedCount(0)
        , m_allocatedPeak(0)
#endif
    {
        if (!m_blocks)
            throw std::bad_alloc();
        
        m_blocks->block = nullptr;
        m_blocks->nextEmptyIdx = 1; // out of range
    }

    ~ObjectPool()
    {
#ifndef NDEBUG
        assert(0 == m_allocatedCount);
        for (size_t i = 0; i < m_blockCount; ++i)
            assert(!m_blocks[i].block);
        printf("MemoryPool<%s>: peak allocation is %zu\n", typeid(T).name(), m_allocatedPeak);
#endif
        free(m_blocks);
    }

    void* Alloc()
    {
#ifndef NDEBUG
        if (++m_allocatedCount > m_allocatedPeak)
            m_allocatedPeak = m_allocatedCount;
#endif

        if (!m_freeBlock)
        {
            // grow if no empty blocks available
            if (m_firstEmptyIdx == m_blockCount)
            {
                m_blocks = (BlockPtr*)realloc(m_blocks, sizeof(BlockPtr) * m_blockCount * 2);
                if (!m_blocks)
                    throw std::bad_alloc();
                for (size_t i = m_blockCount; i < m_blockCount * 2; ++i)
                {
                    m_blocks[i].block = nullptr;
                    m_blocks[i].nextEmptyIdx = i + 1; // last is out of range
                }
                m_blockCount *= 2;
            }

            m_freeBlock = new Block(m_firstEmptyIdx);

            size_t tmp = m_firstEmptyIdx;
            m_firstEmptyIdx = m_blocks[tmp].nextEmptyIdx;

            assert(!m_blocks[tmp].block);
            m_blocks[tmp].block = m_freeBlock;
#ifndef NDEBUG
            m_blocks[tmp].nextEmptyIdx = -1;
#endif
        }

        BlankObject* result = m_freeBlock->Alloc();
        assert(m_freeBlock == result->block);
        // no more free blanks in this block; remove block from free list
        if (!m_freeBlock->firstFreeBlank)
        {
            assert(!m_freeBlock->prevFree);
            Block* tmp = m_freeBlock;
            m_freeBlock = tmp->nextFree;
            if (m_freeBlock)
                m_freeBlock->prevFree = nullptr;
            tmp->nextFree = nullptr;
        }

        return result->data;
    }

    void Free(void* p)
    {
        assert(m_allocatedCount--);

        Block* block = ((BlankObject*)p)->block;
        if (!block->firstFreeBlank)
        {
            // block just became free
            assert(!block->prevFree);
            assert(!block->nextFree);

            if (m_freeBlock)
            {
                assert(!m_freeBlock->prevFree);
                m_freeBlock->prevFree = block;
                block->nextFree = m_freeBlock;
            }
            m_freeBlock = block;
        }

        block->Free((BlankObject*)p);

        if (block->used == 0)
        {
            if (block == m_freeBlock)
                m_freeBlock = m_freeBlock->nextFree;
            if (block->prevFree)
                block->prevFree->nextFree = block->nextFree;
            if (block->nextFree)
                block->nextFree->prevFree = block->prevFree;

            m_blocks[block->thisBlockIdx].block = nullptr;
            m_blocks[block->thisBlockIdx].nextEmptyIdx = m_firstEmptyIdx;
            m_firstEmptyIdx = block->thisBlockIdx;

            delete block;
        }
    }
    
private:
    BlockPtr* m_blocks;
    Block* m_freeBlock;

    size_t m_blockCount;
    size_t m_firstEmptyIdx;

#ifndef NDEBUG
    size_t m_allocatedCount;
    size_t m_allocatedPeak;
#endif
};
