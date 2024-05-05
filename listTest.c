#include <stdio.h>
#include <stdlib.h>
#include "fileSystem.h"

int main(void) {

    char data[128];
    list(data, "/");
    printf("Data read from /: %s\n", data);
    list(data, "/Dir1");
    printf("Data read from /Dir1: %s\n", data);
    list(data, "/Dir1/Dir2");
    printf("Data read from /Dir1/Dir2: %s\n", data);


    return EXIT_SUCCESS;
}
