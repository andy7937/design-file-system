/*
 * volumeNameTest.c
 *
 * Modified on: 26/04/2024
 * Author: Your Name
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileSystem.h"
#include "device.h"

// make volumeName
// ./volumeName

int main(void) {
    char volumeNameResult[BLOCK_SIZE]; 
    memset(volumeNameResult, 0, BLOCK_SIZE); 

    // Format the filesystem with a specific volume name
    if (format("MyTestVolume") < 0) {
        printf("Failed to format the filesystem.\n");
        return EXIT_FAILURE;
    }

    // Retrieve the volume name
    if (volumeName(volumeNameResult) < 0) {
        printf("Failed to retrieve volume name.\n");
        return EXIT_FAILURE;
    }

    // Print the volume name to verify it
    printf("Retrieved Volume Name: %s\n", volumeNameResult);

    return EXIT_SUCCESS;
}
