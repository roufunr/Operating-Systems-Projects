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
void init_list();
void print_mlist();
mblock_t * findLastMemlistBlock();
mblock_t * findFreeBlockOfSize(size_t);
void splitBlockAtSize(mblock_t*, size_t);
void coallesceBlockPrev(mblock_t*);
void coallesceBlockNext(mblock_t*);
mblock_t * growHeapBySize(size_t);


int main(int argc, char* argv[]) {
    init_list();
    
    //void * p4 = mymalloc(500);
    void * p1 = mymalloc(10); 
    void * p2 = mymalloc(100); 
    // print_mlist();
    void * p3 = mymalloc(200); 
    print_mlist();
    
    void * p4 = mymalloc(20); 
    print_mlist();
    void * p5 = mymalloc(54); 
    print_mlist();
    
}

void init_list() {
    mlist.head = NULL;
}

void print_mlist() {
    printf("=================================================================\n");
    mblock_t* current = mlist.head;
    while (current != NULL) {
        printf("mblock_address: %p, Size: %zu, Status: %d, Payload: %p\n",current, current->size, current->status, current->payload);
        current = current->next;
    }
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

        block->next = remianing_block;
        block->status = 1;
        block->payload = (void *)block + MBLOCK_HEADER_SZ;
    } else {
        block->status = 1;
        block->payload = (void *)block + MBLOCK_HEADER_SZ;
    }
    
}

void coallesceBlockPrev(mblock_t * freedBlock) {

}

void coallesceBlockNext(mblock_t * freedBlock) {

}

void* getNewBlockInitialAddress(mblock_t* lastMemBlockAddr, size_t size) {
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
mblock_t * growHeapBySize(size_t size) { 
    mblock_t* lastMemBlockAddr = findLastMemlistBlock();
    void* newBlockInitAddr = getNewBlockInitialAddress(lastMemBlockAddr, size);
    mblock_t* newMblock = (mblock_t *) newBlockInitAddr;
    printf("# Program break incremented at size %ld\n", size);
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
    if(freeBlock == NULL) {
        freeBlock = growHeapBySize(256);
    }
    splitBlockAtSize(freeBlock, size);
    return (void*)freeBlock + MBLOCK_HEADER_SZ;
}

void myfree(void * ptr) {
    // return nothing
}
