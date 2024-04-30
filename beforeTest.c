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

	create("/fileB");
	a2write("/fileB", "bbbbb", 6);

	list(listResult, "/");
	printf("%s\n", listResult);

	create("/dir1/fileA");
	a2write("/dir1/fileA", "1a1a1a", 7);

	printf("Test 1: List root\n");
	list(listResult, "/");
	printf("%s\n", listResult);

	printf("Test 2: List /dir1\n");
	list(listResult, "/dir1");
	printf("%s\n", listResult);
	return EXIT_SUCCESS;
}
