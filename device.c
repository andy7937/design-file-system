/*
 * device.c
 *
 *  Modified on: 11/04/2024
 *      Author: Sean Ma
 *
 *  You are not allowed to modify this file
 */

#include "device.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#define NUMBLOCKS	16	    //The number of blocks in the device

#define EBADBLOCK		1
#define EOPENINGDEVICE	2
#define ECREATINGMMAP	3
#define EINSTALLATEXIT	4

/* The device is really just an array of blocks. */
block *deviceBlocks;

/* The device error number. */
int dev_errno = 0;

/*
 * Prints the message and then information about the most recent error.
 */
void printDevError(char *message) {
	fprintf(stderr, "%s ERROR: ", message);
	switch (dev_errno) {
	case EBADBLOCK:
		fprintf(stderr, "bad block number\n");
		break;
	case EOPENINGDEVICE:
		fprintf(stderr, "unable to open device\n");
		break;
	case ECREATINGMMAP:
		fprintf(stderr, "unable to memory map device\n");
		break;
	case EINSTALLATEXIT:
		fprintf(stderr, "unable to install atexit handler\n");
		break;
	default:
		fprintf(stderr, "unknown error\n");
	}
}

/*
 * Ensures the data is flushed back to disk when the program exits.
 */
void removeDevice() {
	if (munmap(deviceBlocks, NUMBLOCKS * BLOCK_SIZE) == -1) {
		perror("ERROR removing mmap");
	}
}

/*
 * Connects an area of memory with the file representing the device.
 * Only called once each time the program is run.
 */
int connectDevice() {
	int dev_fd;
	struct stat fileStatus;

	if ((dev_fd = open("device_file", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
		dev_errno = EOPENINGDEVICE;
		return -1;
	}
	if (fstat(dev_fd, &fileStatus)) {
		dev_errno = EOPENINGDEVICE;
		return -1;
	}
	int size = fileStatus.st_size;
	unsigned char data[BLOCK_SIZE]; // could be garbage until formatted
	while (size < NUMBLOCKS * BLOCK_SIZE) {
		if (write(dev_fd, data, BLOCK_SIZE) == -1) {
			dev_errno = EOPENINGDEVICE;
			return -1;
		}
		size += BLOCK_SIZE;
	}
	deviceBlocks = (block *)mmap(NULL, NUMBLOCKS * BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, 0);
	if ((long)deviceBlocks == -1) {
		dev_errno = ECREATINGMMAP;
		return -1;
	}
	if (atexit(removeDevice)) {
		dev_errno = EINSTALLATEXIT;
		return -1;
	}
	return 0;
}

/*
 * Checks to see if the device is attached.
 * If not it tries to connect it.
 * Returns -1 if there is a problem, 0 otherwise.
 */
int badDevice() {
	static int unconnected = 1;
	if (unconnected) {
		if (connectDevice()) {
			return -1;
		}
		unconnected = 0;
	}
	return 0;
}

/**
 * Reads a specified block of data from the device and stores it in a given buffer.
 *
 * @param blockNumber The index of the block to be read, starting from 0.
 * @param data Pointer to the buffer where the read data should be stored.
 *        The buffer must have space for at least BLOCK_SIZE bytes.
 *
 * @return 0 on successful read operation, -1 on failure.
 *
 * On success, the data from the specified block is copied into the provided buffer.
 * On failure, the function returns -1, indicating an error occurred. Possible
 * error conditions include an invalid block number or a malfunctioning device.
 * If the block number is out of the valid range (0 to NUMBLOCKS - 1), the global
 * variable 'dev_errno' is set to EBADBLOCK to indicate the specific error.
 */
int blockRead(int blockNumber, unsigned char * const data) {
    if (badDevice()) {
        return -1; // Device is in a bad state or malfunctioning
    }
    if (blockNumber < 0 || blockNumber >= NUMBLOCKS) {
        dev_errno = EBADBLOCK; // Invalid block number specified
        return -1;
    }
    memcpy(data, deviceBlocks[blockNumber], BLOCK_SIZE); // Copy data from device to buffer
    return 0; // Successful read operation
}

/**
 * Writes a specified block of data to the device.
 *
 * @param blockNumber The index of the block to be written, starting from 0.
 * @param data Pointer to the buffer containing the data to be written.
 *        The buffer must contain exactly BLOCK_SIZE bytes of data.
 *
 * @return 0 on successful write operation, -1 on failure.
 *
 * On success, the data from the provided buffer is copied to the specified block
 * on the device. On failure, the function returns -1, indicating an error occurred.
 * Possible error conditions include an invalid block number or a malfunctioning device.
 * If the block number is out of the valid range (0 to NUMBLOCKS - 1), the global
 * variable 'dev_errno' is set to EBADBLOCK to indicate the specific error.
 */
int blockWrite(int blockNumber, const unsigned char * const data) {
    if (badDevice()) {
        return -1; // Device is in a bad state or malfunctioning
    }
    if (blockNumber < 0 || blockNumber >= NUMBLOCKS) {
        dev_errno = EBADBLOCK; // Invalid block number specified
        return -1;
    }
    memcpy(deviceBlocks[blockNumber], data, BLOCK_SIZE); // Copy data to the device from buffer
    return 0; // Successful write operation
}


/*
 * Reports the number of blocks in the device.
 */
int numBlocks() {
	return NUMBLOCKS;
}

/*
 * Displays the contents of a block on stdout.
 * Not really a device function, just for debugging and marking.
 */
 
 /*
  * The sample of what displayBlock(int blockNumber) will print is shown below:

Block 0
==========
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 	----------------
10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 	----------------
20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 	 !"#$%&'()*+,-./
30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 	0123456789:;<=>?  
 
 */
 
 
void displayBlock(int blockNumber) {
    block b; // Now using the typedef for block
    if (blockRead(blockNumber, b)) { // Correctly passing 'b' as it's already an unsigned char array
        printDevError("-- displayBlock --");
    } else {
        printf("\nBlock %d\n", blockNumber);
        printf("==========\n");
        const int number_per_row = 16;
        for (int row = 0; row < BLOCK_SIZE / number_per_row; ++row) {
            for (int col = 0; col < number_per_row; ++col) {
                // Directly increment the pointer in the print statement for brevity
                printf("%02x ", b[row * number_per_row + col]);
            }
            printf("\t");
            for (int col = 0; col < number_per_row; ++col) {
                unsigned char c = b[row * number_per_row + col];
                if (c < 32 || c > 126) c = '-';
                printf("%c", c);
            }
            printf("\n");
        }
    }
}
