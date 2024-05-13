#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileSystem.h"

void test_format() {
    printf("Testing format():\n");

    // Test with a valid volume name within the character limit
    format("MyVolume");

    char volume[64];
    volumeName(volume);
    printf("Volume name: %s\n", volume);

    // Test with a volume name exceeding the character limit
    format("ThisIsAVeryLongVolumeNameThatExceedsTheCharacterLimit12341234aA");

    volumeName(volume);
    printf("Volume name: %s\n", volume);

    // Test for proper initialization of the root directory
    // (display content to verify)

    // Test for proper resetting of global variables
}

void test_create() {
    printf("\nTesting create():\n");

    // Test with a valid path name to create a file
    create("/file1");

    // Test with a valid path name to create a directory
    create("/dir1");

    // Test with an invalid path name (e.g., containing invalid characters)
    create("/dir&");

    // Test for creating a file with the same name in an existing directory
    create("/dir1/file1");

    // Test for creating a file in a nested directory
    create("/dir1/dir2/file2");
}

void test_list() {
    printf("\nTesting list():\n");

    // Test listing files in the root directory
    char result[1024];
    list(result, "/");
    printf("%s", result);

    // Test listing files in a nested directory
    list(result, "/dir1");
    printf("%s", result);

    // Test listing an empty directory
    list(result, "/dir3");
    printf("%s", result);

    // Test listing a directory that doesn't exist
    list(result, "/nonexistent");
    printf("%s", result);
}

void test_a2write_a2read() {
    printf("\nTesting a2write() and a2read():\n");

    // Test writing data to the end of a file
    a2write("/file1", "Hello", 5);

    // Test writing data to a non-existing file
    a2write("/nonexistent", "World", 5);

    // Test writing data with a length exceeding available space
    char longData[100];
    memset(longData, 'A', 100);
    a2write("/file1", longData, 100);

    // Test writing data to a read-only file
    a2write("/dir1", "CannotWrite", 11);

    // Test reading data from the start of a file
    char data[128];
    a2read("/file1", data, 5);
    printf("Data from /file1: %s\n", data);

    // Test reading data from the middle of a file
    a2read("/file1", data, 3);
    printf("Data from /file1: %s\n", data);

    // Test reading data from a file with multiple read operations
    a2read("/file1", data, 10);
    printf("Data from /file1: %s\n", data);
    a2read("/file1", data, 10);
    printf("Data from /file1: %s\n", data);

    // Test reading data from a non-existing file
    a2read("/nonexistent", data, 10);
    printf("Data from /nonexistent: %s\n", data);

    // Test reading data with a length exceeding available data
    a2read("/file1", data, 100);
    printf("Data from /file1: %s\n", data);
}

int main(void) {
    test_format();
    test_create();
    test_list();
    test_a2write_a2read();

    return EXIT_SUCCESS;
}
