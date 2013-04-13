#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <slack/std.h>
#include <slack/list.h>
#include "sfs_api.h"
#include "disk_emu.h"

//Constants
#define MAX_FILES                     100
#define DIRECTORY_INDEX               0
#define FAT_INDEX                     1
#define FREE_INDEX                    2
#define DESCRIPTOR_OFFSET             3
#define BUSY                          99
#define FREE                          100

//Errors
#define F_NOPEN_FOR_WRITING_ERROR     -1
#define F_NOPEN_FOR_READING_ERROR     -1
#define F_ALREADY_CLOSED_ERROR        -1
#define F_CLOSED_SUCCESS               0

typedef struct{
  char fileName[32];
  int fileSize;
  time_t dateModified;
  int fatIndex; //Data block location of the given file.
}directory_entry_t;

typedef struct{
  int fatIndex;
  int writeIndex;
  int readIndex;
}fdt_t;

typedef struct{
  int blockIndex;
  int nextFatIndex; //Next data block entry for a given file. Multiple data block entries are
                    //used to represent a file.
}fat_entry_t;

static int blockSize;
static int noBlocks;
directory_entry_t* directory;
fdt_t* fd;
fat_entry_t* fat;
List* freeList;
list_release_t* destroyFreeList;
int initialise = 1;
int i;
static int bytesWrittenDuringCurrentOpen;

int mksfs(int fresh)
{ 
  initialize();
  if(fresh){
    init_fresh_disk("SFS",blockSize,noBlocks);
    
    //Reset all the structures
    for(i = 0; i < MAX_FILES; i++)
    {
      //Reset directory entries.
      strcpy(directory[i].fileName, "");
      directory[i].fileSize = 0;
      time(&directory[i].dateModified);
      directory[i].fatIndex = 0;
      
      //Reset fdt entries
      fd[i].fatIndex = 0;
      fd[i].writeIndex = 0;
      fd[i].readIndex = 0;
      
      //Reset the fat entries.
      fat[i].blockIndex = 0;
      fat[i].nextFatIndex = 0;
      
      //We store the blocks contain the disk data structures
      //(i.e. root directory, fd, and fat). The rest of the blocks are free.
      for(i=3;i<noBlocks;i++)
      {
        list_append_int(freeList,i);
      }
      
      //Write the directory structure to the 1st block in the FS.
      write_blocks(DIRECTORY_INDEX, 1, directory);
      
      //Write the fat to the 2nd block in the FS.
      write_blocks(FAT_INDEX,1,fat);
      
      //Write the free list to the FS.
      int* tmp = (int *)malloc(blockSize);
      int block;
      for(i=0; i<noBlocks; i++)
      {
        tmp[i] = BUSY;
      }
      for(i=0; i<list_length(freeList);i++)
      {
        block = list_item_int(freeList,i);
        tmp[block] = FREE;
      }
      write_blocks(FREE_INDEX,1,tmp);
      free(tmp);
    }
  }
  return 0;
}

void sfs_ls(void)
{
  int currFileIndex = 0;
  while((strcmp(directory[currFileIndex].fileName,""))){
    printf("%s\n",directory[currFileIndex].fileName);
    currFileIndex++;
  }
}

int sfs_open(char *name)
{
  int found=0;
  int currFileIndex = 0;
  int openValue = -1;
  
  while(!found && currFileIndex < MAX_FILES){
    if(!strcmp(directory[currFileIndex].fileName, name))
    {
      found=1;
      break;
    }
    currFileIndex++;
  }  
  
  //The file was found so we set up the appropriate entry in
  //the fd table.
  if(found)
  {
    fd[currFileIndex].fatIndex = directory[currFileIndex].fatIndex;
    fd[currFileIndex].writeIndex = directory[currFileIndex].fileSize;
    fd[currFileIndex].readIndex = 0;
    openValue = currFileIndex;
    bytesWrittenDuringCurrentOpen = 0;
  }
  
  //The file was not found so we create one.
  else
  {
     openValue = createFile(name);
  }
  
  return openValue;
}

int sfs_close(int fileID)
{
  //If file is already closed return an error.
  if(fd[fileID].fatIndex == 0)
  {
    return F_ALREADY_CLOSED_ERROR;
  }
  
  //Close the file and return 0.
  else
  {
    fd[fileID].fatIndex = 0;
    return F_CLOSED_SUCCESS;
  }
}

int sfs_write(int fileID, char *buf, int length)
{
  char *buffNew;
  int currentBlock = 0;
  int noBytesToWrite = 0;
  int noBytesWritten = 0;
  int currentOffset = 0;
  
  //Check if file is open before anything.
  if(fd[fileID].fatIndex != 0)
  {
    //Get the current block to start writing to as well as the offset within
    //the block.
    currentBlock = fd[fileID].fatIndex;
    currentOffset = fd[fileID].writeIndex;
    while(currentOffset > blockSize){
      currentBlock = fat[currentBlock].nextFatIndex;
      currentOffset -= blockSize;
    }
    
    //Start writting the bytes.
    while(noBytesWritten != length)
    {
      //Buffer to hold the data to write.
      buffNew = (char *)malloc(blockSize*sizeof(char));
      
      //Read the current block to prevent data from being overwritten.
      read_blocks(currentBlock,1,buffNew);
      
      //Get the number of bytes to write to the given block.
      noBytesToWrite = (length - noBytesWritten>blockSize - currentOffset) ? blockSize - currentOffset: length - noBytesWritten;
      
      //Copy the data to write in the block in a buffer.
      memcpy(buffNew + currentOffset, buf + noBytesWritten, noBytesToWrite);
     
      //Write data to the block.
      write_blocks(currentBlock, 1, buffNew);
      
      //Free the buffer that holds the data.
      free(buffNew);
      
      //Increment the number of bytes written.
      noBytesWritten += noBytesToWrite;
      bytesWrittenDuringCurrentOpen += noBytesWritten;
      
      //Update the directory table and file descriptor tables.
      fd[fileID].writeIndex += noBytesToWrite;
      directory[fileID].fileSize += noBytesToWrite;
      
      //For the next block, the offset is 0.
      currentOffset = 0;
      
      //Check whether we require another block to write.
      if(noBytesWritten != length)
      {
        //We need to create a new block in this case.
        if(fat[currentBlock].nextFatIndex == 0)
        {
          //First we check if there are free blocks left in the list.
          if(list_length(freeList) != 0)
          {
            fat[currentBlock].nextFatIndex = list_shift_int(freeList);
            currentBlock = fat[currentBlock].nextFatIndex;
          }
          
          //There are no more blocks left. Thus, we return the number
          //of bytes written thus far.
          else
          {
            updateSFS();
            return noBytesWritten;
          }
        }
        
        //We need to allocate another block.
        else
        {
          currentBlock = fat[currentBlock].nextFatIndex;
        }
      }
    }
    
    updateSFS();
  }
  
  //File does not exist.
  else
  {
    return F_NOPEN_FOR_WRITING_ERROR;
  }
 
  return noBytesWritten;
}

int sfs_read(int fileID, char *buf, int length)
{
  // char *buffNew;
  // int currentBlock=0;
  // int currentOffset=0;
  // int noBytesToRead;
  // int noBytesRead = 0;
  
  // //Check if file is open before anything.
  // if(fd[fileID].fatIndex != 0)
  // {
    // //Determine which block to start reading from
    // currentBlock = fd[fileID].fatIndex;
    
  // }
  
  // //The file hasn't been opened as there is no entry in the fd table. Thus,
  // //we return an error.
  // else
  // {
    // return F_NOPEN_FOR_READING_ERROR;
  // }
  
  return 0;
}

void initialize()
{
  noBlocks = MAX_FILES + 3;
  freeList = list_create(destroyFreeList);
  blockSize = MAX_FILES * sizeof(directory_entry_t);
  if(MAX_FILES * sizeof(fdt_t) > blockSize)
  {
    blockSize = MAX_FILES * sizeof(fdt_t);
  }
  else if(noBlocks * sizeof(fat_entry_t) > blockSize)
  {
    blockSize = noBlocks * sizeof(fat_entry_t);
  }
  
  directory = (directory_entry_t*)malloc(blockSize);
  fat = (fat_entry_t*)malloc(blockSize);
  fd = (fdt_t*)malloc(MAX_FILES * sizeof(fdt_t));
  fat[1].blockIndex = 1;
  fat[2].blockIndex = 2;
}

int createFile(char *name)
{
  int newFileIndex = 0;
  
  //First we verify if there is space available to create this new file.
  if(list_length(freeList) == 0)
  {
    return -1;
  }
  
  //There is space so we update the directory table, fat, and fd table.
  else
  {
    //Check where the next entry in the directory table goes.
    while(directory[newFileIndex].fatIndex != 0)
    {
      newFileIndex++;
    }
    
    //Update the directory table by putting an entry for the newly created file.
    strcpy(directory[newFileIndex].fileName,name);
    directory[newFileIndex].fileSize = 0;
    time(&directory[newFileIndex].dateModified);
    directory[newFileIndex].fatIndex = list_shift_int(freeList);
    
    //Update the fat by putting an entry for the newly created file.
    fat[newFileIndex].blockIndex = directory[newFileIndex].fatIndex;
    fat[newFileIndex].nextFatIndex = 0;
    
    //Update the fd table by putting an entry for the newly created file.
    fd[newFileIndex].fatIndex = directory[newFileIndex].fatIndex;
    fd[newFileIndex].readIndex = 0;
    fd[newFileIndex].writeIndex = 0;
  }
  
  return newFileIndex;
}

void updateSFS()
{
  //Write the directory structure to the 1st block in the FS.
  write_blocks(DIRECTORY_INDEX, 1, directory);
      
  //Write the fat to the 2nd block in the FS.
  write_blocks(FAT_INDEX,1,fat);
    
  //Write the free list to the FS.
  int* tmp = (int *)malloc(blockSize);
  int block;
  for(i=0; i<noBlocks; i++)
  {
    tmp[i] = BUSY;
  }
  for(i=0; i<list_length(freeList);i++)
  {
    block = list_item_int(freeList,i);
    tmp[block] = FREE;
  }
  write_blocks(FREE_INDEX,1,tmp);
  free(tmp);
}