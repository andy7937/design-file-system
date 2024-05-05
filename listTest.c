#include <stdio.h>
#include <stdlib.h>
#include "fileSystem.h"

int main(void) {

    char data_a[128];
    format("Data Storage");

    create("/dir1/fileA");
    a2write("/dir1/fileA", "aaaaaa", 7); // Includes a null terminator
    a2write("/dir1/fileA", "bbbbbb", 7); // Includes a null terminator

    a2read("/dir1/fileA", data_a, 3); // Reads the first 3 bytes
    printf("Data read: %s\n", data_a);
    a2read("/dir1/fileA", data_a, 4); // Reads the next 4 bytes
    printf("Data read: %s\n", data_a);
    a2read("/dir1/fileA", data_a, 7); // Reads the next 4 bytes
    printf("Data read: %s\n", data_a);






    return EXIT_SUCCESS;
}
