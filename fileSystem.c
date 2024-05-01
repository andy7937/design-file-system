/* fileSystem.c
 *
 *  Modified on: 
 *  Author: 
 *  Version 1.1
 *  Changes:
 * - Fixed a bug where `fileCachePos` was incorrectly incremented regardless of whether a file was already cached. 
 *   This issue mainly resulted in unnecessary cache space usage but was unlikely to cause errors during low to moderate read operations.
 *   This version ensures that `fileCachePos` is only incremented when a new file is added to the cache, thereby optimizing cache usage and management.
 *
 * Complete this file.
 */

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"
#include "file.h"
#include "fileSystem.h"

// Initialise - format(initialise the device for use by this file system)
//              volumename on first block
//              create root directory and write information in second block
//              reset global variables


// Operations - create(create new file in filesystem automatically handling creation of any necessary directories along the path)

//              a2read(reading data from a file from the last read position)

//              a2write(writing data to a file at the end of the file)

//              seek(repositioning the file pointer for the file at the specified location)
//              for example, if you a2write at the end of the file, then the pointer will be at the
//              end of the file, so when you use a2read at a specific point, you would need to use seek
//              first to get to the correct position before you start reading, and if you use a2write again
//              after this, you would need to move the pointer again to the end of the file.


// Information - list(lists all files in the named directory, with file names and sizes)

/* The file system error number. */
int file_errno = 0;

typedef struct {
  // pointer to file name
  char *fileName;
  // cursor pos for reading/writing
  int cursorpos;
} filecache;

// access to file and position data for files 
filecache fileCache[1024];
int fileCachePos = 0;

// COMPLETE
/*
 * Formats the device for use by this file system.
 * The volume name must be < 64 bytes long.
 * All information previously on the device is lost.
 * Also creates the root directory "/".
 * Returns 0 if no problem or -1 if the call failed.
 */
int format(char *volumeName) {
  // set all memory to 0 except for first block
  CHECK_TRY(strlen(volumeName) >= BLOCK_SIZE || strlen(volumeName) <= 0,
            EBADVOLNAME)
  // setting block volName with array 0 (empty)
  block volName = {0};

  // setting all blocks to volName currently empty, block 1 will be used for root directory
  for (int i = 2; i < numBlocks(); i++) {
    CHECK_ERR(blockWrite(i, (unsigned char *)volName), EBADDEV)
  }

  // copy block name into an array of BLOCK_SIZE long requires block name to be
  // smaller than BLOCK_SIZE
  strncpy((char *)volName, volumeName, BLOCK_SIZE);

  // writing volName into BLOCK 0
  CHECK_ERR(blockWrite(0, volName), EBADDEV)

  // setting rootDir array as (0, 1), presumably to mark it as root dir? not sure yet
  block rootDir = {0, 1};
  finfo root = finfoNew("/", ISDIR, 12, 1);
  finfoWrite(&root, (char *)&rootDir[2]);

  // reset global variables
  fileCachePos = 0;
  nextEmptyBlock = 1;
  file_errno = 0;

  // writing rootDir into BLOCK 1
  CHECK_ERR(blockWrite(1, rootDir), EBADDEV)
  return 0;
}


// COMPLETE
/*
 * Returns the volume's name in the result.
 * Returns 0 if no problem or -1 if the call failed.
 */
int volumeName(char *result) {

  // read first block and write data into result (first block is volume name)
  if (blockRead(0, (unsigned char*)result) < 0){
    return -1;
  }

  // make sure string is null terminated
  result[BLOCK_SIZE - 1] = '\0';
  return 0;
}

// COMPLETE
/*
 * Makes a file with a fully qualified pathname starting with "/".
 * It automatically creates all intervening directories.
 * Pathnames can consist of any printable ASCII characters (0x20 - 0x7e)
 * including the space character.
 * The occurrence of "/" is interpreted as starting a new directory
 * (or the file name).
 * Each section of the pathname must be between 1 and 7 bytes long (not
 * counting the "/"s).
 * The pathname cannot finish with a "/" so the only way to create a
 * directory is to create a file in that directory. This is not true for the
 * root directory "/" as this needs to be created when format is called. The
 * total length of a pathname is limited only by the size of the device.
 * Returns 0 if no problem or -1 if the call failed.
 */
int create(char *pathName) {
  // copy path name to stack
  // If pathname is /root/dir/file1/file2
  // dirName is /root/dir/file1
  // baseName is /file2
  char dirPathName[strlen(pathName) + 1];
  char basePathName[strlen(pathName) + 1];
  strcpy(dirPathName, pathName);
  strcpy(basePathName, pathName);
  char *dirName = dirname(dirPathName);
  char *baseName = basename(basePathName);

    // checking all file names are valid
  int length = strlen(pathName);
  int segmentBytes = 0; // Reset segment length counter
  
  for (int i = 1; i <= length; i++) { // Start from 1 to skip the initial '/'
      if (pathName[i] == '/' || i == length) {
          if (segmentBytes < 1 || segmentBytes > 7) {
              file_errno = EOTHER;
              return -1;
          }
          segmentBytes = 0; // Reset for the next segment
      } else {
          // Check for valid ASCII characters
          if ((unsigned char)pathName[i] < 0x20 || (unsigned char)pathName[i] > 0x7e) {
              file_errno = EOTHER;
              return -1;
          }
          segmentBytes++;
      }
  }

  CHECK_TRY(baseName[0] == '.' || dirName[0] == '.', EOTHER)
  finfoData fData;
  CHECK_RET(traverseFiles(&fData, 1, dirName))
  int arrSize = fData.curDir.size / DIRCONTENTSIZE;
  finfo files[arrSize];
  CHECK_RET(getDirContent(&fData.curDir, files))
  fData.nextDirIndex = containsFile(files, arrSize, baseName, ISFILE);
  //If there is no such file
  if (fData.nextDirIndex == -1) {
    fData.nextDir = finfoNewFile(baseName);
    CHECK_RET(dataAppendDir(&fData.curDir, &fData.nextDir))
    CHECK_RET(updateDirEntry(&fData.prevDir, &fData.curDir, fData.curDirIndex))
  } else {
  // a file with the same name has been created. Return an error.
    file_errno = EOTHER;
    return -1;
  }
  return 0;
}

// TODO: TEST
/*
 * Returns a list of all files in the named directory.
 * The "result" string is filled in with the output.
 * The result looks like this (assuming there are "file1" and "file 2" in the folder "dir1")

/dir1:
file1	42
file2	0

 * The fully qualified pathname of the directory followed by a colon and
 * then each file name followed by a tab "\t" and then its file size.
 * Each file on a separate line.
 * The directoryName is a full pathname.
 */
void list(char *result, char *directoryName) {
  char dirName[strlen(directoryName) + 1];
  strcpy(dirName, directoryName);
  strcpy(result, directoryName);
  strcat(result, ":\n");

  finfoData fData;

  if (traverseFiles(&fData, 0, dirName) == -1) {
    printf("traverse failed\n");
    return;
  }

  int arrSize = fData.curDir.size / DIRCONTENTSIZE;
  finfo files[arrSize];

  if (getDirContent(&fData.curDir, files) == -1) {
    printf("getDirContent failed\n");
    return;
  }
  for (int i = 0; i < arrSize; i++) {
    if (strcmp("/", files[i].name) == 0) {
      continue;
    }
    strcat(result, files[i].name);
    strcat(result, ":\t");
    char buf[10] = {0};
    sprintf(buf, "%d", files[i].size);
    strcat(result, buf);
    strcat(result, "\n");
  }
}

// TODO - writing into root files work but writing into subdirectories does not work
/*
 * Writes data onto the end of the file.
 * Copies "length" bytes from data and appends them to the file.
 * The filename is a full pathname.
 * The file must have been created before this call is made.
 * Returns 0 if no problem or -1 if the call failed.
 */
int a2write(char *fileName, void *data, int length) {
    if (length <= 0) {
        file_errno = EOTHER; // Invalid length
        return -1;
    }


    char dirPathName[strlen(fileName) + 1];
    char basePathName[strlen(fileName) + 1];
    strcpy(dirPathName, fileName);
    strcpy(basePathName, fileName);
    char *dirName = dirname(dirPathName);
    char *baseName = basename(basePathName);

    finfoData fData;

    if (traverseFiles(&fData, 0, dirName) == -1) {
        return -1; // Directory traversal failed
    }

    int arrSize = fData.curDir.size / DIRCONTENTSIZE;
    finfo files[arrSize];
    
    if (getDirContent(&fData.curDir, files) == -1) {
        return -1; // Failed to read directory content
    }

    int fileIndex = containsFile(files, arrSize, baseName, ISFILE);
    if (fileIndex == -1) {
        file_errno = EOTHER; // File does not exist
        return -1;
    }

    length -= 1;
    
    // Use dataAppend to append data to the file
    if (dataAppend(&files[fileIndex], data, length) != 0) {
        return -1; // Append failed
    }


    if (updateDirEntry(&fData.curDir, &files[fileIndex], fileIndex) != 0) {
        return -1; // Failed to update directory entry
    }

    return 0;
}



// TODO: TEST
/*
 * Reads data from the start of the file.
 * Maintains a file position so that subsequent reads continue
 * from where the last read finished.
 * The filename is a full pathname.
 * The file must have been created before this call is made.
 * Returns 0 if no problem or -1 if the call failed.
 */
int a2read(char *fileName, void *data, int length) {
  CHECK_TRY(length <= 0, EOTHER)
  int cursorPos = 0;
  int isCached = 0;
  int cachePos = -1;
  for (int i = 0; i < fileCachePos; i++) {
    if (strcmp(fileName, fileCache[i].fileName) == 0) {
      cursorPos = fileCache[i].cursorpos;
      isCached = 1;
      cachePos = i;
      break;
    }
  }
  char dirPathName[strlen(fileName) + 1];
  char basePathName[strlen(fileName) + 1];
  strcpy(dirPathName, fileName);
  strcpy(basePathName, fileName);
  char *dirName = dirname(dirPathName);
  char *baseName = basename(basePathName);
  finfoData fData;
  CHECK_RET(traverseFiles(&fData, 0, dirName))
  int arrSize = fData.curDir.size / DIRCONTENTSIZE;
  finfo files[arrSize];
  CHECK_RET(getDirContent(&fData.curDir, files))
  fData.nextDirIndex = containsFile(files, arrSize, baseName, ISFILE);
  CHECK_ERR(fData.nextDirIndex, ENOSUCHFILE)
  fData.nextDir = files[fData.nextDirIndex];
  if (cursorPos >= fData.nextDir.size) {
    strncpy(data, "\0", length);
  } else {
    char readData[fData.nextDir.size + 1];
    memset(readData, 0, fData.nextDir.size + 1);
    CHECK_RET(dataRead(&fData.nextDir, readData))
    strncpy(data, &readData[cursorPos], length);
  }
  if (!isCached) {
    fileCache[fileCachePos].fileName = fileName;
    fileCache[fileCachePos].cursorpos = length;
    fileCachePos++;
  } else {
    fileCache[cachePos].cursorpos += length;
  }
  return 0;
}

// TODO: TEST
/*
 * Repositions the file pointer for the file at the specified location.
 * All positive integers are byte offsets from the start of the file.
 * 0 is the beginning of the file.
 * If the location is after the end of the file, move the location
 * pointer to the end of the file.
 * The filename is a full pathname.
 * The file must have been created before this call is made.
 * Returns 0 if no problem or -1 if the call failed.
 */
int seek(char *fileName, int location) {
  // check if the current file name has been cached already
  for (int i = 0; i < fileCachePos; i++) {
    if (strcmp(fileName, fileCache[i].fileName) == 0) {
      fileCache[i].cursorpos = location;
      return 0;
    }
  }

  char dirPathName[strlen(fileName) + 1];
  char basePathName[strlen(fileName) + 1];
  strcpy(dirPathName, fileName);
  strcpy(basePathName, fileName);
  char *dirName = dirname(dirPathName);
  char *baseName = basename(basePathName);

  finfoData fData;
  CHECK_RET(traverseFiles(&fData, 0, dirName))
  int arrSize = fData.curDir.size / DIRCONTENTSIZE;
  finfo files[arrSize];
  CHECK_RET(getDirContent(&fData.curDir, files))
  fData.nextDirIndex = containsFile(files, arrSize, baseName, ISFILE);
  CHECK_RET(fData.nextDirIndex)
  fileCache[fileCachePos].fileName = fileName;
  fileCache[fileCachePos].cursorpos = location;
  fileCachePos++;
  return 0;
}