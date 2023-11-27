#include<stdio.h>
#include <stdlib.h>

int main() {
    char* datablock = (char *) malloc(100 * sizeof(char));
    char* datablock2 = (char *) malloc(100 * sizeof(char));
    char* datablock3 = (char *) malloc(100 * sizeof(char));
    for(int i = 0; i < 100; i++) {
        printf("%d\n", datablock3[i]);
    }
}