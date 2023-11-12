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

mblock_t * findLastMemlistBlock();
mblock_t * findFreeBlockOfSize(size_t size);
void splitBlockAtSize(mblock_t*, size_t);
void coallesceBlockPrev(mblock_t*);
void coallesceBlockNext(mblock_t*);
mblock_t * growHeapBySize(size_t);

int main(int argc, char* argv[]) {
    printf("Hello world\n");
    return 0;
}

void * mymalloc(size_t size) {
    void *payload;
    // put the implementation here
    return payload;
}

void myfree(void * ptr) {
    // return nothing
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
