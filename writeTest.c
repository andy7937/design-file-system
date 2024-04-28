#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileSystem.h"

int main(void) {
    int result;
    char data[256] = "Sample data to append.";
    int dataLength = strlen(data);

    // Initialize and format the filesystem
    format("TestFS");

    // Create a test file
    create("/testFolder/testFile.txt");
    write_to_file("/testFolder/testFile.txt", "Initial data.", 13);

    // Test 1: Append data to an existing file
    result = a2write("/testFolder/testFile.txt", data, dataLength);
    if (result == 0) {
        printf("Test 1 Passed: Data appended successfully.\n");
    } else {
        printf("Test 1 Failed: Could not append data.\n");
    }

    // Test 2: Try writing to a non-existent file
    result = a2write("/testFolder/nonexistent.txt", data, dataLength);
    if (result == -1) {
        printf("Test 2 Passed: Correctly handled non-existent file.\n");
    } else {
        printf("Test 2 Failed: Did not handle non-existent file correctly.\n");
    }

    // Optional: Read back the data to verify correctness
    char readBackData[512] = {0};
    a2read("/testFolder/testFile.txt", readBackData, 512);
    printf("Data in file: %s\n", readBackData);

    return EXIT_SUCCESS;
}
