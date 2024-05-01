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

	a2read("/fileB", data, 6);
	printf("Data from /fileB: %s\n", data);

	a2read("/dir1/fileA", data, 16);
	printf("Data from /dir1/fileA: %s\n", data);

	a2read("/dir1/fileB", data, 9);
	printf("Data from /dir1/fileB: %s\n", data);

	a2read("/dir1/dir2/fileB", data, 11);
	printf("Data from /dir1/dir2/fileB: %s\n", data);

	// Data from /fileA: aaaaaccccc - 12
	// Data from /fileB: bbbbb - 6
	// Data from /dir1/fileA: 1a1a1a1a1a1a1a - 16
	// Data from /dir1/fileB: 1b1b1b1b - 9
	// Data from /dir1/dir2/fileB: 2b2b2b2b2b - 11

	return EXIT_SUCCESS;
}
