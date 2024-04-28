#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileSystem.h"
#include "file.h"

int main(void) {
    int result;
    char data[256] = "Sample data to append.";
    int dataLength = strlen(data);

    // Initialize and format the filesystem
    if (format("TestFS") != 0) {
        printf("Failed to format filesystem.\n");
        return EXIT_FAILURE;
    }

    // Create a test file
    if (create("/dir1/subdir1/fi.txt") != 0) {
        printf("Failed to create file '/dir1/subdir1/fi.txt'.\n");
        return EXIT_FAILURE;
    }

    // Test 1: Append data to an existing file
    result = a2write("/dir1/subdir1/fi.txt", data, dataLength);
    printf(result == 0 ? "Test 1 Passed: Data appended successfully.\n" : "Test 1 Failed: Could not append data.\n");

    // Test 2: Try writing to a non-existent file
    result = a2write("/dir1/subdir1/nonexistent.txt", data, dataLength);
    printf(result == -1 ? "Test 2 Passed: Correctly handled non-existent file.\n" : "Test 2 Failed: Did not handle non-existent file correctly.\n");

    // Read back the data to verify correctness
    finfoData fData;
    if (traverseFiles(&fData, 0, "/dir1/subdir1") != -1) {
        int arrSize = fData.curDir.size / DIRCONTENTSIZE;
        finfo files[arrSize];
        getDirContent(&fData.curDir, files);
        int fileIndex = containsFile(files, arrSize, "fi.txt", ISFILE);
        if (fileIndex != -1) {
            char* readBackData = malloc(files[fileIndex].size + 1);
            if (dataRead(&files[fileIndex], readBackData) == 0) {
                readBackData[files[fileIndex].size] = '\0';  // Null-terminate the read data
                printf("Data in file: %s\n", readBackData);
            } else {
                printf("Failed to read back data.\n");
            }
            free(readBackData);
        } else {
            printf("File 'fi.txt' not found in directory.\n");
        }
    } else {
        printf("Failed to navigate to directory '/dir1/subdir1'.\n");
    }

    return EXIT_SUCCESS;
}
