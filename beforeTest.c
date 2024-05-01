/*
 * beforeTest.c
 *
 *  Modified on: 24/03/2023
 *      Author: Robert Sheehan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileSystem.h"

int main(void) {
	char listResult[1024];

	format("Permanent Data");

	create("/fileA");	
	a2write("/fileA", "aaaaa", 6);
	a2write("/fileA", "ccccc", 6);

	create("/fileB");
	a2write("/fileB", "bbbbb", 6);
	
	create("/dir1/fileA");
	a2write("/dir1/fileA", "1a1a1a", 7);
	a2write("/dir1/fileA", "1a1a1a1a", 9);

	create("/dir1/fileB");
	a2write("/dir1/fileB", "1b1b1b1b", 9);

	create("/dir1/dir2/fileB");
	a2write("/dir1/dir2/fileB", "2b2b2b2b2b", 11);

	// Data from /fileA: aaaaaccccc - 12
	// Data from /fileB: bbbbb - 6
	// Data from /dir1/fileA: 1a1a1a1a1a1a1a - 16
	// Data from /dir1/fileB: 1b1b1b1b - 9
	// Data from /dir1/dir2/fileB: 2b2b2b2b2b - 11




	printf("Test 1: List root\n");
	list(listResult, "/");
	printf("%s\n", listResult);

	printf("Test 2: List /dir1/\n");
	list(listResult, "/dir1");
	printf("%s\n", listResult);

	printf("Test 3: List /dir1/dir2/\n");
	list(listResult, "/dir1/dir2");
	printf("%s\n", listResult);
	return EXIT_SUCCESS;
}