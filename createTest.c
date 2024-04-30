#include <stdio.h>
#include <stdlib.h>
#include "fileSystem.h"

int main(void) {
    int result;
    char listResult[1024];  // Buffer for storing listing results

    format("TestFS");
    printf("File System Initialized.\n");

    // Test 1: Create a file in the root directory
    result = create("/fil.txt");
    list(listResult, "/");
    printf("After Test 1 - Create /fil.txt:\n%s\n", listResult);
    if (result == 0) {
        printf("Test 1 Passed: File created successfully.\n");
    } else {
        printf("Test 1 Failed: Could not create file.\n");
    }

    // Test 2: Create a file in a new subdirectory
    result = create("/dir1/subdir1/subdir2/fi.txt");
    list(listResult, "/dir1");
    printf("After Test 2 - show dir1:\n%s\n", listResult);
    list(listResult, "/dir1/subdir1");
    printf("After Test 2 - show subdir1\n%s\n", listResult);
    list(listResult, "/dir1/subdir1/subdir2");
    printf("After Test 2 - show subdir2\n%s\n", listResult);
    list(listResult, "subdir2");
    if (result == 0) {
        printf("Test 2 Passed: Nested file created successfully.\n");
    } else {
        printf("Test 2 Failed: Could not create nested file.\n");
    }

    // Test 3: Try creating a file that already exists
    result = create("/fil.txt");
    list(listResult, "/");
    printf("After Test 3 - Try to create existing /fil.txt again:\n%s\n", listResult);
    if (result == -1) {
        printf("Test 3 Passed: Correctly handled attempt to create an existing file.\n");
    } else {
        printf("Test 3 Failed: Did not detect existing file.\n");
    }

    // Test 4: Create a file with an invalid name
    result = create("/my©.txt");
    list(listResult, "/");
    printf("After Test 4 - Try to create /my©.txt:\n%s\n", listResult);
    if (result == -1) {
        printf("Test 4 Passed: Correctly handled invalid filename.\n");
    } else {
        printf("Test 4 Failed: Did not detect invalid filename.\n");
    }

    // Test 5: Create file longer than 7 bytes
    result = create("/di1/dir2/directory/la.txt");
    list(listResult, "/");
    printf("After Test 5 - Try to create /di1/dir2/directory/la.txt:\n%s\n", listResult);
    if (result == -1) {
        printf("Test 5 Passed: Correctly handled attempt to create 7 byte file.\n");
    } else {
        printf("Test 5 Failed: Did not detect 7 byte name.\n");
    }

    // Test 6: Create file shorter than 1 byte
    result = create("/di1/dir2//da.txt");
    list(listResult, "/");
    printf("After Test 6 - Try to create /di1/dir2//da.txt:\n%s\n", listResult);
    if (result == -1) {
        printf("Test 6 Passed: Correctly handled attempt to create 0 byte file.\n");
    } else {
        printf("Test 6 Failed: Did not detect 0 byte name.\n");
    }

    return EXIT_SUCCESS;
}
