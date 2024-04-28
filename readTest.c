#include <stdio.h>
#include <stdlib.h>
#include "fileSystem.h"

int main(void) {

    int result;
    char data[1024]; // Buffer for reading files

    format("TestFS");

    // Test 1: Successfully read from a newly created file
    create("/test.txt");
    write_to_file("/test.txt", "Hello, world!", 13); // Assuming function to write data exists
    result = a2read("/test.txt", data, 13);
    if (result == 0 && strcmp(data, "Hello, world!") == 0) {
        printf("Test 1 Passed: Read data successfully.\n");
    } else {
        printf("Test 1 Failed: Data mismatch or read error.\n");
    }

    // Test 2: Read from a non-existent file
    result = a2read("/nonexistent.txt", data, 10);
    if (result == -1) {
        printf("Test 2 Passed: Correctly handled non-existent file.\n");
    } else {
        printf("Test 2 Failed: Failed to detect non-existent file.\n");
    }

    // Test 3: Read with a zero length
    result = a2read("/test.txt", data, 0);
    if (result == -1) {
        printf("Test 3 Passed: Correctly handled zero length read.\n");
    } else {
        printf("Test 3 Failed: Did not properly handle zero length read.\n");
    }

    // Test 4: Sequential reads to test cursor management
    result = a2read("/test.txt", data, 7); // Read "Hello, "
    result += a2read("/test.txt", data + 7, 6); // Continue to read "world!"
    if (result == 0 && strcmp(data, "Hello, world!") == 0) {
        printf("Test 4 Passed: Sequential reads handled correctly.\n");
    } else {
        printf("Test 4 Failed: Error in sequential reads.\n");
    }

    // Test 5: Read beyond the end of the file
    result = a2read("/test.txt", data, 100); // Trying to read more than written
    if (result == 0 && strcmp(data, "Hello, world!") == 0) {
        printf("Test 5 Passed: Read beyond EOF handled correctly.\n");
    } else {
        printf("Test 5 Failed: Error reading beyond EOF.\n");
    }

    // Displaying the final content of the file system for verification
    list(data, "/");
    printf("Current FileSystem Structure:\n%s\n", data);

    return EXIT_SUCCESS;
}
