#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define MBLOCK_HEADER_SZ offsetof(mblock_t, payload)

typedef struct _mblock_t {
    struct _mblock_t * prev;
    struct _mblock_t * next;
    size_t size;
    int status;
    void * payload;
} mblock_t;

typedef struct _mlist_t {
    mblock_t * head;
} mlist_t;

mlist_t mlist;

void * mymalloc(size_t);
void myfree(void*);
void printMemList(const mblock_t*);
void init_list();
mblock_t * findLastMemlistBlock();
mblock_t * findFreeBlockOfSize(size_t);
void splitBlockAtSize(mblock_t*, size_t);
void coallesceBlockPrev(mblock_t*);
void coallesceBlockNext(mblock_t*);
mblock_t * growHeapBySize(size_t);
void* getNewBlockInitialAddressV1(mblock_t*, size_t);
void* getNewBlockInitialAddressV2(size_t);



int main(int argc, char* argv[]) {
    init_list();

    void * p1 = mymalloc(10); 
    void * p2 = mymalloc(100); 
    void * p3 = mymalloc(200); 
    void * p4 = mymalloc(500); 
    myfree(p3); p3 = NULL; 
    myfree(p2); p2 = NULL; 
    void * p5 = mymalloc(150); 
    void * p6 = mymalloc(500); 
    myfree(p4); p4 = NULL; 
    myfree(p5); p5 = NULL; 
    myfree(p6); p6 = NULL; 
    myfree(p1); p1 = NULL;

    printMemList(mlist.head);
}

void init_list() {
    mlist.head = NULL;
}

void printMemList(const mblock_t* head) {
    const mblock_t* p = head;
    size_t i = 0;
    while(p != NULL) {
        printf("[%ld] p: %p\n", i, p);
        printf("[%ld] p->size: %ld\n", i, p->size);
        printf("[%ld] p->status: %s\n", i, p->status > 0 ? "allocated" : "free");
        printf("[%ld] p->prev: %p\n", i, p->prev);
        printf("[%ld] p->next: %p\n", i, p->next);
        printf("___________________________\n");
        ++i;
        p = p->next;
    }
    printf("===========================\n");
}

mblock_t * findLastMemlistBlock() {
    if(mlist.head == NULL) {
        return NULL;
    }
    mblock_t* current = mlist.head;
    while(current->next != NULL) {
        current = current->next;
    }
    return current;
}

mblock_t * findFreeBlockOfSize(size_t size) {
    if(mlist.head == NULL) {
        return NULL;
    } 
    mblock_t* current = mlist.head;
    while(current != NULL) {
        if(current->status == 0 && size <= current->size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void splitBlockAtSize(mblock_t * block, size_t newSize) {
    if((block->size - newSize) > MBLOCK_HEADER_SZ) {
        size_t remaining_block_size = block->size - newSize - MBLOCK_HEADER_SZ;
        mblock_t* remianing_block = (mblock_t *)((void *)block + MBLOCK_HEADER_SZ + newSize); // addr of remaining block
        remianing_block->status = 0;
        remianing_block->payload = (void*)remianing_block + MBLOCK_HEADER_SZ;
        remianing_block->size = remaining_block_size;

        block->size = newSize;
        remianing_block->next = block->next;
        remianing_block->prev = block;
        if(block->next != NULL) block->next->prev = remianing_block;
        block->next = remianing_block;
        block->status = 1;
        block->payload = (void *)block + MBLOCK_HEADER_SZ;
    } else {
        block->status = 1;
        block->payload = (void *)block + MBLOCK_HEADER_SZ;
    }
    

}

void coallesceBlockPrev(mblock_t * freedBlock) {
    mblock_t* prevBlock = freedBlock->prev;
    prevBlock->size = prevBlock->size + freedBlock->size + MBLOCK_HEADER_SZ;
    prevBlock->next = freedBlock->next;
    if(freedBlock->next != NULL) {
        freedBlock->next->prev = prevBlock;
    }
    freedBlock = prevBlock;
}

void coallesceBlockNext(mblock_t * freedBlock) {
    mblock_t* nextBlock = freedBlock->next;
    freedBlock->size = freedBlock->size + nextBlock->size + MBLOCK_HEADER_SZ;
    freedBlock->next = nextBlock->next;
    if(nextBlock->next != NULL) {
        nextBlock->next->prev = freedBlock;
    }
}

void* getNewBlockInitialAddressV1(mblock_t* lastMemBlockAddr, size_t size) {
    void* newBlockInitAddr;
    if (lastMemBlockAddr == NULL) {
        newBlockInitAddr = sbrk(size);
    } else {
        void* tentativeNewBreakAddr = ((void*) lastMemBlockAddr) + lastMemBlockAddr->size + MBLOCK_HEADER_SZ + size;
        if (brk(tentativeNewBreakAddr) == 0) {
            newBlockInitAddr = ((void*) lastMemBlockAddr) + lastMemBlockAddr->size + MBLOCK_HEADER_SZ;
        } else {
            perror("brk failed!");
            return NULL;
        }
    }
    return newBlockInitAddr;
}

void* getNewBlockInitialAddressV2(size_t size) {
    static void* lastBlockEnd = NULL; 
    if (lastBlockEnd == NULL) {
        lastBlockEnd = sbrk(0); 
    }
    void* newBlockInitAddr = sbrk(size); 
    if (newBlockInitAddr == (void*)-1) {
        perror("sbrk failed!");
        return NULL;
    }
    return newBlockInitAddr;
}

mblock_t * growHeapBySize(size_t size) { 
    mblock_t* lastMemBlockAddr = findLastMemlistBlock();
    //void* newBlockInitAddr = getNewBlockInitialAddress(lastMemBlockAddr, size);
    void* newBlockInitAddr = getNewBlockInitialAddressV2(size);
    mblock_t* newMblock = (mblock_t *) newBlockInitAddr;
    // printf("# Program break incremented at size %ld\n", size);
    newMblock->next = NULL;
    newMblock->prev = NULL;
    newMblock->size = size - MBLOCK_HEADER_SZ;
    newMblock->status = 0;
    if(lastMemBlockAddr == NULL) {
        mlist.head = newMblock;
    } else {
        lastMemBlockAddr->next = newMblock;
        newMblock->prev = lastMemBlockAddr;
    }
    return newMblock;
}

void * mymalloc(size_t size) {
    mblock_t* freeBlock = findFreeBlockOfSize(size);
    size_t default_mblock_chunk_size = 1024;
    if(freeBlock == NULL) {
        size_t increment_size = default_mblock_chunk_size < (size + MBLOCK_HEADER_SZ) ? size + MBLOCK_HEADER_SZ: default_mblock_chunk_size;
        freeBlock = growHeapBySize(increment_size);
    }
    splitBlockAtSize(freeBlock, size);
    return (void*)freeBlock + MBLOCK_HEADER_SZ;
}

void myfree(void * ptr) {
    if(ptr == NULL) {
        printf("Invalid Operation!\n");
    }
    mblock_t* memBlock = (mblock_t*) (ptr - MBLOCK_HEADER_SZ);
    memBlock->status = 0;

    mblock_t* currentMBlock = memBlock;
    while((currentMBlock->next != NULL && currentMBlock->next->status == 0)) {
        coallesceBlockNext(currentMBlock);
    }
    
    currentMBlock = memBlock;
    while(currentMBlock->prev != NULL && currentMBlock->prev->status == 0) {
        coallesceBlockPrev(currentMBlock);
        currentMBlock = currentMBlock->prev;
    }
}
