#include <stdio.h>
#include <stdlib.h>
#include "fileSystem.h"



int main(void) {

    int result;
    char listResult[1024];

    format("TestFS");

    // Test 1: Create a file in the root directory
    result = create("/fil.txt");
    if (result == 0) {
        printf("Test 1 Passed: File created successfully.\n");
    } else {
        printf("Test 1 Failed: Could not create file.\n");
    }

    // Test 2: Create a file in a new subdirectory
    result = create("/dir1/subdir1/fi.txt");
    if (result == 0) {
        printf("Test 2 Passed: Nested file created successfully.\n");
    } else {
        printf("Test 2 Failed: Could not create nested file.\n");
    }

    // Test 3: Try creating a file that already exists
    result = create("/fil.txt");
    if (result == -1) {
        printf("Test 3 Passed: Correctly handled attempt to create an existing file.\n");
    } else {
        printf("Test 3 Failed: Did not detect existing file.\n");
    }

    // Test 4: Create a file with an invalid name
    result = create("/my©.txt");
    if (result == -1) {
        printf("Test 4 Passed: Correctly handled invalid filename.\n");
    } else {
        printf("Test 4 Failed: Did not detect invalid filename.\n");
    }

    // Test 5: Create file longer than 7 bytes
    result = create("/di1/dir2/directory/la.txt");
    if (result == -1) {
        printf("Test 5 Passed: Correctly handled attempt to create 7 byte file.\n");
    } else {
        printf("Test 5 Failed: Did not detect 7 byte name.\n");
    }

    // Test 6: Create file shorter than 1 byte
    result = create("/di1/dir2//da.txt");
    if (result == -1) {
        printf("Test 6 Passed: Correctly handled attempt to create 0 byte file.\n");
    } else {
        printf("Test 6 Failed: Did not detect 0 byte name.\n");
    }

    list(listResult, "/");
    printf("Current FileSystem Structure:\n%s\n", listResult);

    return EXIT_SUCCESS;
}
