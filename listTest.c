#include <stdio.h>
#include <stdlib.h>
#include "fileSystem.h"

int main(void) {

    format("TestFS");
    create("/dir1/file1.txt");
    create("/dir1/file2.txt");
    // file2 remains empty
    char result[1024] = {0}; // Buffer to hold the result from the list function

    // Test 1: Listing a directory with files
    list(result, "/dir1");
    printf("Test 1 Output:\n%s\n", result);
    if (strstr(result, "/dir1:") && strstr(result, "file1.txt:\t13") && strstr(result, "file2.txt:\t0")) {
        printf("Test 1 Passed: Directory listing with files is correct.\n");
    } else {
        printf("Test 1 Failed: Directory listing with files is incorrect.\n");
    }

    return EXIT_SUCCESS;
}
