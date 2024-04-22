/*
 * file.c
 *
 *  Modified on: 19/04/2024
 *  Author: anonymous
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"
#include "file.h"
#include "fileSystem.h"

#define BP_SIZE 2                      // BLOCK_POINTER_SIZE
#define BD_SIZE (BLOCK_SIZE - BP_SIZE) // BLOCK_DATA_SIZE

int ceilDiv(int numer, int denom) { return (numer + denom - 1) / denom; }

//data[1] and data[0] are used to store block pointers (BP)
int shortRead(unsigned char *data) {
  unsigned int tens = data[0];
  unsigned int ones = data[1];
  return tens * 256 + ones;
}

void shortWrite(short number, char *data) {
  data[0] = number / 256;
  data[1] = number % 256;
}

finfo finfoNew(char *name, int type, int size, int blockLocation) {
  finfo file;
  strncpy(file.name, name, 7);
  file.type = type;
  file.size = size;
  file.blockLocation = blockLocation;
  return file;
}

// Function: finfoWrite
// Description: Serializes finfo structure data into a character array (named "data") for storage.
// Parameters: 
// - file: the finfo structure to be written.
// - data: the character array where the finfo data is to be written.
void finfoWrite(finfo *file, char *data) {
  strncpy(data, file->name, 7);
  data[7] = file->type;
  shortWrite(file->size, &data[8]);
  shortWrite(file->blockLocation, &data[10]);
}

// Function: finfoRead
// Description: Deserializes character array data back into an finfo structure.
// Parameters: 
// - data: the character array containing the serialized finfo data.
// Returns: A finfo structure filled with the data from the character array.
finfo finfoRead(char *data) {
  finfo file;
  strncpy(file.name, data, 7);
  file.type = data[7];
  file.size = shortRead((unsigned char *)&data[8]);
  file.blockLocation = shortRead((unsigned char *)&data[10]);
  return file;
}

// Function: finfoNewDir
// Description: Creates a new finfo structure for a directory.
//              When size is 0 and blockLocation is 0, it means no block is 
//              allocated for this file or dir.       
// Parameters:
// - name: the name of the new directory.
// Returns: An finfo structure for a new directory with the specified name.
finfo finfoNewDir(char *name) { return finfoNew(name, ISDIR, 0, 0); }

// Function: finfoNewFile
// Description: Creates a new finfo structure for a file.
// Parameters:
// - name: the name of the new file.
// Returns: An finfo structure for a new file with the specified name.
finfo finfoNewFile(char *name) { return finfoNew(name, ISFILE, 0, 0); }

// Function: sizeGetNumBlocks
// Description: Calculates the number of blocks needed to store a file of given size.
//              In this case, each block has 62(BD_SIZE) bytes available to store file data
// Parameters:
// - fileSize: the size of the file in bytes.
// Returns: The number of blocks needed to store the file.
int sizeGetNumBlocks(int fileSize) {
  // return ceiling division
  return ceilDiv(fileSize, BD_SIZE);
}

// Function: remainingSize
// Description: Calculates the size that can be written to the current block or returns
//              the remaining file size. If the datasize is more than BD_SIZE, return 
//              BD_SIZE and datasize minus BD_SIZE. Otherwise, return the dataSize and 
//              then zero dataSize.
// Parameters:
// - dataSize: pointer to the size of the data to be written.
// Returns: The size to be written to the current block or the remaining file size.
int remainingSize(int *dataSize) {
  if (*dataSize > BD_SIZE) {
    *dataSize -= BD_SIZE;
    return BD_SIZE;
  } else {
    int remainingSize = *dataSize;
    *dataSize = 0;
    return remainingSize;
  }
}

// caches the lowest empty block
int nextEmptyBlock = 1;

// Function: getNextEmptyBlock
// Description: Finds the next empty block in the file system that can be used to store data.
// Parameters:
// - nextBlock: pointer to store the block number of the found empty block.
// Returns: 0 on success or -1 if no empty block is found.
int getNextEmptyBlock(int *nextBlock) {
  block blockData = {0};

  // starts the for loop from the cached global blockPointer
  for (int i = nextEmptyBlock; i < numBlocks(); i++) {
    // don't write to the current block
    if (i == *nextBlock) {
      continue;  //skip one time of blockRead
    }
	//Try to read block pointers (BP). If BP equals 0, the block pointed by this BP
	//is available.
    CHECK_ERR(blockRead(i, blockData), EBADDEV)
    if (shortRead(blockData) == 0) {
      nextEmptyBlock = i;
      *nextBlock = i;
      return 0;
    }
  }
  file_errno = ENOROOM;
  return -1;
}

// Function: fileGetLastBlock
// Description: Finds the last block associated with a given file.
// Parameters:
// - file: pointer to the finfo structure of the file.
// - lastBlock: pointer to store the block number of the last block of the file.
// Returns: 0 on success or -1 on failure.
int fileGetLastBlock(finfo *file, int *lastBlock) {
  *lastBlock = file->blockLocation;
  block blockData = {0};
  for (int i = 0; i < sizeGetNumBlocks(file->size); i++) {
    CHECK_ERR(blockRead(*lastBlock, blockData), EBADDEV)
    *lastBlock = shortRead(blockData);
  }
  return 0;
}

// Function: fileGetNthBlock
// Description: Finds the N-th block in the chain of blocks associated with a file.
// Parameters:
// - file: pointer to the finfo structure of the file.
// - lastBlock: pointer to store the block number after N iterations.
// - n: the N-th block to find in the sequence.
// Returns: 0 on success or -1 on failure.
int fileGetNthBlock(finfo *file, int *lastBlock, int n) {
  *lastBlock = file->blockLocation;
  block blockData = {0};
  for (int i = 0; i < n; i++) {
    CHECK_ERR(blockRead(*lastBlock, blockData), EBADDEV)
    *lastBlock = shortRead(blockData);
  }
  return 0;
}

// Function: fileGetLastBlockDataSize
// Description: Calculates the data size within the last block of a file.
// Parameters:
// - file: pointer to the finfo structure of the file.
// Returns: The size of the data in the last block of the file.
int fileGetLastBlockDataSize(finfo *file) {
  int ret = file->size % BD_SIZE;
  if (ret == 0 && file->size != 0) {
    ret = BD_SIZE;
  }
  return ret;
}

// Function: dataRead
// Description: Reads the content of a file into a data buffer.
// Parameters:
// - file: pointer to the finfo structure of the file to be read.
// - data: the buffer where the file's data will be stored.
// Returns: 0 on success or -1 on failure.
int dataRead(finfo *file, char *data) {
  int blockNumber = file->blockLocation;
  int dataSize = file->size;
  block blockData = {0};
  for (int i = 0; dataSize != 0; i++) {
    CHECK_ERR(blockRead(blockNumber, blockData), EBADDEV)

    int readSize = remainingSize(&dataSize);
	//memcpy excludes the BP_SIZE of data.
    memcpy(data + i * (BD_SIZE), blockData + BP_SIZE, readSize);
    blockNumber = shortRead(blockData);
  }
  return 0;
}

// Function: dataWrite
// Description: Writes data to a sequence of blocks starting from a specified block number.
// Parameters:
// - blockNumber: the starting block number to write data to.
// - data: the buffer containing the data to write.
// - size: the size of the data to write.
// Returns: 0 on success or -1 on failure.
int dataWrite(int blockNumber, char *data, int size) {
  for (int i = 0; size != 0; i++) {
    // initialise empty block to write everything to
    block blockData = {0};
    int readSize = remainingSize(&size);
    memcpy(blockData + BP_SIZE, data + i * (BD_SIZE), readSize);

    // get the next block to write to
    int nextBlock = blockNumber;
    if (size != 0) {
      CHECK_RET(getNextEmptyBlock(&nextBlock))
    }

    // assign the block pointer to the start of the block
    char nextBlockPointer[2];
    shortWrite(nextBlock, nextBlockPointer);
    memcpy(blockData, nextBlockPointer, BP_SIZE);

    // write data to the block
    CHECK_ERR(blockWrite(blockNumber, blockData), EBADDEV)
    blockNumber = nextBlock;
  }
  return 0;
}

// Function: dataAppend
// Description: Appends data to the end of a file, expanding the file as necessary.
// Parameters:
// - file: pointer to the finfo structure of the file to append data to.
// - data: the buffer containing the data to append.
// - size: the size of the data to append.
// Returns: 0 on success or -1 on failure.
int dataAppend(finfo *file, char *data, int size) {
  int lastBlockLocation;
  if (file->blockLocation == 0) {		//New file
    CHECK_RET(getNextEmptyBlock(&lastBlockLocation))
    file->blockLocation = lastBlockLocation;
  } else {								//Existed file
    CHECK_RET(fileGetLastBlock(file, &lastBlockLocation))
  }

  // check if there is enough room to write the data
  int dataSize = fileGetLastBlockDataSize(file);
  int localNextEmptyBlock;
  if (getNextEmptyBlock(&localNextEmptyBlock) == -1) {
    localNextEmptyBlock = numBlocks(); 		//Only the last block is free.
  }
  int freeBlockCount = numBlocks() - localNextEmptyBlock;
  int spaceLeft = freeBlockCount * BD_SIZE + (BD_SIZE - dataSize) % BD_SIZE;
  CHECK_TRY(size > spaceLeft, ENOROOM)
  // initialise char array to write everything
  char writeData[size + dataSize];	//The data in the last block need to be read and re-writed into the same block
  block blockData = {0};
  CHECK_ERR(blockRead(lastBlockLocation, blockData), EBADDEV)

  // copy all data into writeData
  memcpy(writeData, blockData + BP_SIZE, dataSize); // Except BP pointer.
  memcpy(writeData + dataSize, data, size);

  CHECK_RET(dataWrite(lastBlockLocation, writeData, dataSize + size))
  file->size += size;
  return 0;
}

// Function: dataAppendDir
// Description: Appends directory information to the current directory structure in the filesystem.
//              This function serializes the directory information of 'nextFile' into a formatted string
//              and appends it to the 'curDir' directory. It is typically used when a new directory
//              needs to be added to the existing directory structure.
// Parameters:
//   - curDir: Pointer to the finfo structure representing the current directory to which the new directory info will be appended.
//   - nextFile: Pointer to the finfo structure of the directory to be appended to 'curDir'.
// Returns: 0 on success, or -1 on failure, with an appropriate error set to indicate the issue (e.g., no room to append the data).
int dataAppendDir(finfo *curDir, finfo *nextFile) {
  char data[DIRCONTENTSIZE] = {0};
  finfoWrite(nextFile, data);
  CHECK_RET(dataAppend(curDir, data, DIRCONTENTSIZE))
  return 0;
}

// Function: updateBlock
// Description: Updates a specific block in the filesystem with new data starting from a specified position.
//              This function reads the existing block data, modifies part of it starting from 'startPos' with 'len' bytes
//              from 'finfoData', and writes the updated block back to the filesystem. It is useful for updating
//              metadata or file contents that are stored in blocks without needing to rewrite the entire block.
// Parameters:
//   - blockNum: The block number in the filesystem that needs to be updated.
//   - finfoData: The data buffer containing new data to be written into the block.
//   - startPos: The starting position within the block where new data will be written.
//   - len: The length of the data in bytes to be written to the block.
// Returns: 0 on success or -1 on failure, indicating errors during block read or write operations.
int updateBlock(int blockNum, char *finfoData, int startPos, int len) {
  block data = {0};
  CHECK_ERR(blockRead(blockNum, data), EBADDEV)
  memcpy(&data[startPos] + BP_SIZE, finfoData, len); 
  CHECK_ERR(blockWrite(blockNum, data), EBADDEV)
  return 0;
}

// Function: updateDirEntry
// Description: Updates a directory entry within a specific block. If the directory entry spans multiple blocks,
//              this function handles the spillover and ensures that the entry is correctly split across the blocks.
//              This is particularly used when modifying directory entries that might not fit within a single block
//              due to their size exceeding the remaining space in the block.
// Parameters:
//   - file: Pointer to the finfo structure of the parent directory containing the directory entry to be updated.
//   - fileEntry: Pointer to the finfo structure that contains the new data for the directory entry.
//   - fileIndex: The index of the directory entry within the parent directory. This is used to calculate the
//                block and position within the block where the entry needs to be updated.
// Returns: 0 on success or -1 on failure, typically indicating problems with block updates or invalid directory indices.
int updateDirEntry(finfo *file, finfo *fileEntry, int fileIndex) {
  int startIndex = fileIndex * DIRCONTENTSIZE;
  int nthBlock = startIndex / BD_SIZE;
  int startBlock;
  int startNextBlock;
  fileGetNthBlock(file, &startBlock, nthBlock);
  fileGetNthBlock(file, &startNextBlock, nthBlock + 1);
  int startPos = startIndex % BD_SIZE;
  char finfoData[DIRCONTENTSIZE];
  finfoWrite(fileEntry, finfoData);
  if (startPos + DIRCONTENTSIZE > BD_SIZE) { //the remaining of current block is not enough
    int firstLen = BD_SIZE - startPos;
    int nextLen = startPos + DIRCONTENTSIZE - BD_SIZE;
    CHECK_RET(updateBlock(startBlock, finfoData, startPos, firstLen) == -1 ||
              updateBlock(startNextBlock, &finfoData[firstLen], 0, nextLen))
//comparing with -1 to avoid that the 2nd updateBlock is not executed when the 1st updateBlock return Ture.               
  } else {
    CHECK_RET(updateBlock(startBlock, finfoData, startPos, DIRCONTENTSIZE))
  }
  return 0;
}

int getRoot(finfo *rootInfo) {
  block data = {0};
  CHECK_ERR(blockRead(1, data), EBADDEV)
  *rootInfo = finfoRead((char *)&data[2]);
  return 0;
}

int getDirContent(finfo *fileInfo, finfo *fileArray) {
  char data[fileInfo->size];
  CHECK_RET(dataRead(fileInfo, data))

  for (int i = 0; i < fileInfo->size; i += DIRCONTENTSIZE) {
    fileArray[i / DIRCONTENTSIZE] = finfoRead(&data[i]);
  }
  return 0;
}

int containsFile(finfo *files, int arrsize, char *filename, int fileType) {
  for (int i = 0; i < arrsize; i++) {
    if (strcmp(files[i].name, filename) == 0 && files[i].type == fileType) {
      return i;
    }
  }
  return -1;
}

int traverseFiles(finfoData *fData, int isCreateDir, char *dirPath) {
  CHECK_RET(getRoot(&fData->prevDir))//set root info as the prevDir
  fData->curDir = fData->prevDir;    //prevDir is also the curDir
  fData->prevDirIndex = fData->curDirIndex = 0;  //pre and cur' indices are 0.
  char *token = strtok(dirPath, "/");
  while (token != NULL) {
    int arrSize = fData->curDir.size / DIRCONTENTSIZE; 
    finfo files[arrSize];
    CHECK_RET(getDirContent(&fData->curDir, files))
    fData->nextDirIndex = containsFile(files, arrSize, token, ISDIR);
    if (fData->nextDirIndex == -1) {
      CHECK_TRY(!isCreateDir, ENOSUCHFILE)
      fData->nextDir = finfoNewDir(token);
      CHECK_RET(dataAppendDir(&fData->curDir, &fData->nextDir))
      CHECK_RET(
          updateDirEntry(&fData->prevDir, &fData->curDir, fData->curDirIndex))
      fData->nextDirIndex = arrSize;
    } else {
      fData->nextDir = files[fData->nextDirIndex];
    }
    fData->prevDir = fData->curDir;
    fData->prevDirIndex = fData->curDirIndex;
    fData->curDir = fData->nextDir;
    fData->curDirIndex = fData->nextDirIndex;
    token = strtok(NULL, "/");
  }
  return 0;
}

void debugFinfo(finfo file) {
  printf("\n");
  printf("name: %s\n", file.name);
  printf("type: %c\n", file.type);
  printf("size: %d\n", file.size);
  printf("location: %d\n", file.blockLocation);
}
