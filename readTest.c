#include <stdio.h>
#include <stdlib.h>
#include "fileSystem.h"

int main(void) {

    char data[128];

    format("NewTest");
    a2read("/fileA", data, 6);
    printf("Data read from /fileA: %s\n", data);
    return EXIT_SUCCESS;
}
