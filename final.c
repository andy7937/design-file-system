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
    char listResult[1024];
	char data[128];
    int result;

    // testing format
	if (format("Data Storage") == 0) {
        printf("test success 1\n");
    }else{
        printf("test fail 1\n");
    }


    if (format("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") == -1) {
        printf("test success 2\n");
    }else{
        printf("test fail 2\n");
    }
    
    // testing volumeName
    volumeName(data);
    printf("Volume name should be Data Storage, it is: %s\n", data);

    // Test 1: Create a file in the root directory
    result = create("/fil.txt");
    list(listResult, "/");
    printf("After Test 1 - Create /fil.txt:\n%s\n", listResult);
    if (result == 0) {
        printf("Test 1 Passed: File created successfully.\n");
    } else {
        printf("Test 1 Failed: Could not create file.\n");
    }

    // Test 2: Create a file in a new subdirectory
    result = create("/dir/subdir1/subdir2/fi.txt");
    list(listResult, "/dir");
    printf("After Test 2 - show dir1:\n%s\n", listResult);
    list(listResult, "/dir/subdir1");
    printf("After Test 2 - show subdir1\n%s\n", listResult);
    list(listResult, "/dir/subdir1/subdir2");
    printf("After Test 2 - show subdir2\n%s\n", listResult);
    list(listResult, "subdir2");
    if (result == 0) {+
        printf("Test 2 Passed: Nested file created successfully.\n");
    } else {
        printf("Test 2 Failed: Could not create nested file.\n");
    }

    // Test 3: Try creating a file that already exists
    result = create("/fil.txt");
    list(listResult, "/");
    printf("After Test 3 - Try to create existing /fil.txt again:\n%s\n", listResult);
    if (result == -1) {
        printf("Test 3 Passed: Correctly handled attempt to create an existing file.\n");
    } else {
        printf("Test 3 Failed: Did not detect existing file.\n");
    }

    // Test 4: Create a file with an invalid name
    result = create("/my©.txt");
    list(listResult, "/");
    printf("After Test 4 - Try to create /my©.txt:\n%s\n", listResult);
    if (result == -1) {
        printf("Test 4 Passed: Correctly handled invalid filename.\n");
    } else {
        printf("Test 4 Failed: Did not detect invalid filename.\n");
    }

    // Test 5: Create file longer than 7 bytes
    result = create("/di1/dir2/directory/la.txt");
    list(listResult, "/");
    printf("After Test 5 - Try to create /di1/dir2/directory/la.txt:\n%s\n", listResult);
    if (result == -1) {
        printf("Test 5 Passed: Correctly handled attempt to create 7 byte file.\n");
    } else {
        printf("Test 5 Failed: Did not detect 7 byte name.\n");
    }

    // Test 6: Create file shorter than 1 byte
    result = create("/di1/dir2//da.txt");
    list(listResult, "/");
    printf("After Test 6 - Try to create /di1/dir2//da.txt:\n%s\n", listResult);
    if (result == -1) {
        printf("Test 6 Passed: Correctly handled attempt to create 0 byte file.\n");
    } else {
        printf("Test 6 Failed: Did not detect 0 byte name.\n");
    }

    //testing before test
    result =  create("/dir1/fileA");
    list(listResult, "/dir1");
    printf("After Test 7 - testing before\n%s\n", listResult);
    if (result == -1) {
        printf("Test 7 fail\n");
    } else {
        printf("Test 7 pass\n");
    }

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

    format("NEWFILE");

    if (a2read("/fileA", data, 12) == -1) {
        printf("test success 8\n");
	}

    volumeName(data);
    printf("Volume name should be Data Storage 2, it is: %s\n", data);

    create("/second");
    a2write("/second", "second", 7);
    printf("should read second \n");
    a2read("/second", data, 7);
    printf("Data from second should be second it is: %s\n", data);

    a2write("/second", "123456", 7);
    printf("should read 123456 \n");
    a2read("/second", data, 7);
    printf("Data from second should be 123456 it is: %s\n", data);



	return EXIT_SUCCESS;
}

