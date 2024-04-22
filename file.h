/*
 * file.h
 *
 *  Modified on: 08/05/2023
 *  Author: anonymous
 */

#define ISDIR 'd'
#define ISFILE 'f'
#define DIRCONTENTSIZE 12

extern int nextEmptyBlock;

#define CHECK_ERR(retVal, errno)                                               \
  if (retVal == -1) {                                                          \
    file_errno = errno;                                                        \
    return -1;                                                                 \
  }

#define CHECK_RET(retVal)                                                      \
  if (retVal == -1) {                                                          \
    return -1;                                                                 \
  }

#define CHECK_TRY(retVal, errno)                                               \
  if (retVal) {                                                                \
    file_errno = errno;                                                        \
    return -1;                                                                 \
  }

typedef struct {
  char name[8];
  int type;
  int size;
  int blockLocation;
} finfo;

/* Get the size of the file excluding the block pointers */
int getDataSize(finfo *file);

/* Create a new struct for file info */
finfo finfoNew(char *name, int type, int size, int blockLocation);

finfo finfoNewDir(char *name);

finfo finfoNewFile(char *name);

void finfoWrite(finfo *file, char *data);

/* get the number of blocks a file takes up from its size */
// int sizeGetNumBlocks(int size); // doesn't have any uses

/* gets the last block for a given file */
int fileGetLastBlock(finfo *file, int *lastBlock);

int remainingSize(int *size);

/* read data from file excluding block pointers */
int dataRead(finfo *file, char *data);

/*
 * Rewrites everything to a specified block
 * If it overflows, then it will select a new block to write into
 */
int dataWrite(int blockNumber, char *data, int size);

/*
 * Appends to a file
 * If it overflows, then it will select a new block to write into
 */
int dataAppend(finfo *file, char *data, int size);

int dataAppendDir(finfo *curDir, finfo *nextDir);

int updateDirEntry(finfo *file, finfo *fileEntry, int fileIndex);

int getRoot(finfo *rootInfo);

void debugFinfo(finfo file);

int getDirContent(finfo *fileInfo, finfo *fileArray);

int containsFile(finfo *files, int arrsize, char *filename, int fileType);

typedef struct finfoData {
  finfo prevDir;
  int prevDirIndex;
  finfo curDir;
  int curDirIndex;
  finfo nextDir;
  int nextDirIndex;
} finfoData;

// typedef struct finfoData finfoData;

int traverseFiles(finfoData *finfoData, int isCreateDir, char *dirPath);
