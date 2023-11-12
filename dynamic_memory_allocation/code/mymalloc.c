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
mblock_t * findLastMemlistBlock();
mblock_t * findFreeBlockOfSize(size_t);
void splitBlockAtSize(mblock_t*, size_t);
void coallesceBlockPrev(mblock_t*);
void coallesceBlockNext(mblock_t*);
mblock_t * growHeapBySize(size_t);

int main(int argc, char* argv[]) {
    init_list();
    
}

void init_list() {
    mlist.head = NULL;
}

mblock_t* add_block(mlist_t* list, size_t size, int status, void* payload) {
    mblock_t* new_block = (mblock_t*)malloc(sizeof(mblock_t));
    if (new_block == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    new_block->size = size;
    new_block->status = status;
    new_block->payload = payload;

    // Insert at the front of the list
    new_block->next = list->head;
    new_block->prev = NULL;

    if (list->head != NULL) {
        list->head->prev = new_block;
    }

    list->head = new_block;

    return new_block;
}

void remove_block(mlist_t* list, mblock_t* block) {
    if (block->prev != NULL) {
        block->prev->next = block->next;
    } else {
        list->head = block->next;
    }

    if (block->next != NULL) {
        block->next->prev = block->prev;
    }

    free(block);
}

void print_list(mlist_t* list) {
    mblock_t* current = list->head;

    while (current != NULL) {
        printf("Size: %zu, Status: %d, Payload: %p\n", current->size, current->status, current->payload);
        current = current->next;
    }
}

void clear_list(mlist_t* list) {
    mblock_t* current = list->head;
    mblock_t* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    list->head = NULL;
}

mblock_t * findLastMemlistBlock() {
    return NULL;
}

mblock_t * findFreeBlockOfSize(size_t size) {
    return NULL;
}
void splitBlockAtSize(mblock_t * block, size_t newSize) {
    
}
void coallesceBlockPrev(mblock_t * freedBlock) {

}
void coallesceBlockNext(mblock_t * freedBlock) {

}
mblock_t * growHeapBySize(size_t size) {
    return NULL;
}

void * mymalloc(size_t size) {
    void *payload;
    // put the implementation here
    return payload;
}

void myfree(void * ptr) {
    // return nothing
}
