/*
 * afterTest.c
 *
 *  Modified on: 24/03/2023
 *      Author: Robert Sheehan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileSystem.h"

int main(void) {
	char data[128];
	a2read("/fileA", data, 12);
	printf("Data from /fileA: %s\n", data);

	a2read("/dir1/fileA", data, 7);
	printf("Data from /dir1/fileA: %s\n", data);

	a2read("/dir1/fileB", data, 9);
	printf("Data from /dir1/fileB: %s\n", data);

	a2read("/dir1/dir2/fileB", data, 11);
	printf("Data from /dir1/dir2/fileB: %s\n", data);

	return EXIT_SUCCESS;
}
