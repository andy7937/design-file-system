#include <stdio.h>
#include <stdlib.h>
#include "fileSystem.h"

int main(void) {

    format("TestFS");
    create("/dir1/fd.txt");
    create("/dir1/fi.txt");
    create("/fi.txt");
    create("/dir2");
    // file2 remains empty
    char result[1024] = {0}; // Buffer to hold the result from the list function

    // Test 1: Listing a directory with files
    list(result, "/dir1");
    printf("Test 1: List dir1\n%s\n", result);
    
    list(result, "/dir2");
    printf("Test 2: List dir2\n%s\n", result);

    list(result, "/");
    printf("Test 3: List root\n%s\n", result);

    return EXIT_SUCCESS;
}
