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
#define DEBUG                         0

//Errors
#define F_NOPEN_FOR_WRITING_ERROR     -1
#define F_NOPEN_FOR_READING_ERROR     -1
#define F_ALREADY_CLOSED_ERROR        -1
#define F_CLOSED_SUCCESS               0
#define F_READ_OUT_OF_BOUNDS          -1

/*Structure to represent an entry in the directory table. Each time a file is 
created, an instance of this structure will be created.*/
typedef struct{
  char fileName[32];
  int fileSize;
  time_t dateModified;
  int fatIndex; //Data block location of the given file.
}directory_entry_t;

/*Structure to represent an entry in the file descriptor table. Each time a file
is opened, an instance of this structure will be created.*/
typedef struct{
  int fatIndex;
  int writeIndex;
  int readIndex;
}fdt_t;

/*Structure to represent an entry in the file access table. Each file will have
at least on entry in the file access table.*/
typedef struct{
  int blockIndex;
  int nextFatIndex; //Next data block entry for a given file. Multiple data block entries are used to represent a file.
}fat_entry_t;

/*The size of each individual block in the FS.*/
static int blockSize;

/*The number of blocks that are present in the FS.*/
static int noBlocks;

/*Pointer to the directory table.*/
directory_entry_t* directory;

/*Pointer to the file descriptor table.*/
fdt_t* fd;

/*Pointer to the file access table.*/
fat_entry_t* fat;

/*Free list which keeps a track of all the free blocks.*/
List* freeList;

/*Dummy list.*/
list_release_t* destroyFreeList;

/*Used in loops.*/
int i;

/*
Depending on the value of the free variable, this function either initializes
a new file system or reuses an existing one.
*/
int mksfs(int fresh)
{ 
  initialize();
  if(fresh){
    //initialise a new FS called SFS.
    init_fresh_disk("SFS",blockSize,noBlocks);
    
    //Reset all the structures
    for(i = 0; i < MAX_FILES; i++)
    {
      //Reset directory entries.
      #if DEBUG == 1
        printf("mksfs(): Initialising new file system with fresh variable set to 1.\n");
        printf("mksfs(): Resetting the directory entries.\n");
      #endif
      strcpy(directory[i].fileName, "");
      directory[i].fileSize = 0;
      time(&directory[i].dateModified);
      directory[i].fatIndex = 0;
      
      //Reset fd entries
      #if DEBUG == 1
        printf("mksfs(): Resetting the fd entries.\n");
      #endif
      fd[i].fatIndex = 0;
      fd[i].writeIndex = 0;
      fd[i].readIndex = 0;
      
      //Reset the fat entries.
      #if DEBUG == 1
        printf("mksfs(): Resetting the fat entries.\n");
      #endif
      fat[i].blockIndex = 0;
      fat[i].nextFatIndex = 0;
      
      //We store the blocks contain the disk data structures
      //(i.e. root directory, fd, and fat). The rest of the blocks are free.
      #if DEBUG == 1
        printf("mksfs(): Storing the free blocks in a list.\n");
      #endif
      for(i=3;i<noBlocks;i++)
      {
        list_append_int(freeList,i);
      }
      
      //Write the blocks to the FS.
      #if DEBUG == 1
        printf("mksfs(): About to write to disk.\n");
      #endif
      updateSFS();
      #if DEBUG == 1
        printf("mksfs(): Successfully wrote to disk.\n");
      #endif
    }
  }
  
  //Load from the FS.
  else
  {
    //Initialise the disk.
     #if DEBUG == 1
        printf("mksfs(): Initialising new file system with fresh variable set to 0.\n");
      #endif
    init_disk("SFS",blockSize,noBlocks);
    
    //Read the 1st block (directory table) into the directory pointer.
    read_blocks(DIRECTORY_INDEX,1,directory);
    
    //Read the 2nd block (file access table) into the fat pointer.
    read_blocks(FAT_INDEX,1,fat);
    
    //Store the block number of the free blocks inside the freeList.
    int* tmp = (int *)malloc(blockSize);
    list_empty(freeList);
    read_blocks(FREE_INDEX,1,tmp);
    for(i=0; i<noBlocks; i++)
    {
      if(tmp[i] == FREE)
      {
        list_append_int(freeList,i);
      }
    }
    #if DEBUG == 1
      printf("mksfs(): Successfully read from disk.\n");
    #endif
  }
  
  //Return 0 as the SFS is not expected to fail.
  return 0;
}

/*
This function lists the files in the main directory.
*/
void sfs_ls(void)
{
  int currFileIndex = 0;
  
  //Goes through all the entries in the directory table and 
  //prints the file name.
  while((strcmp(directory[currFileIndex].fileName,""))){
    printf("%s\n",directory[currFileIndex].fileName);
    currFileIndex++;
  }
}

/*
This function returns the value of the file descriptor associated with
the given file name. A negative value is returned if an error occurs.
*/
int sfs_open(char *name)
{
  int found=0;
  int currFileIndex = 0;
  int openValue = -1;
  
  //We attempt to find the file that matches the name variable. If a 
  //match is found we break and proceed with the code below. If no
  //match is found and we have search all the files, then an we
  //create the file.
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
     #if DEBUG == 1
        printf("sfs_open(): File was found. Going to add an entry in the fd table.\n");
      #endif
    fd[currFileIndex].fatIndex = directory[currFileIndex].fatIndex;
    fd[currFileIndex].writeIndex = directory[currFileIndex].fileSize;
    fd[currFileIndex].readIndex = 0;
    openValue = currFileIndex;
  }
  
  //The file was not found so we create one and open it (it's opened when it's created).
  else
  {
    #if DEBUG == 1
      printf("sfs_open():File was not found so creating it.\n");
    #endif
    openValue = createFile(name);
    #if DEBUG == 1
      printf("sfs_open():File was successfully created.\n");
    #endif
  }
  
  //Return a value greater than 0 if the operation was successfull.
  #if DEBUG == 1
    printf("sfs_open():Returning file descriptor %d.\n",openValue);
  #endif
  return openValue;
}

/*
The function closes a file. If the file does exist in the file descriptor, it is closed. Otherwise,
the file is already closed or does not exist so an error is returned.
*/
int sfs_close(int fileID)
{
  //If file is already closed return an error.
  if(fd[fileID].fatIndex == 0)
  {
    #if DEBUG == 1
      printf("sfs_close(): The file was already closed. Returning an error.\n");
    #endif
    return F_ALREADY_CLOSED_ERROR;
  }
  
  //Close the file and return 0.
  else
  {
    fd[fileID].fatIndex = 0;
    #if DEBUG == 1
      printf("sfs_close(): File was successfully closed.\n");
    #endif
    return F_CLOSED_SUCCESS;
  }
}

/*
This function returns the number of bytes actually written to the disk. In most
cases, it should be the same as the number of bytes asked to write - length.
*/
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
    
    //Find the offset.
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
      #if DEBUG == 1
        printf("sfs_write(): Bytes to write is %d. Bytes written thus far is %d.\n",length,noBytesWritten);
      #endif
      
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
            #if DEBUG == 1
              printf("sfs_write(): No more blocks. Added a new fat block with index %d.\n",currentBlock);
            #endif
          }
          
          //There are no more blocks left. Thus, we return the number
          //of bytes written thus far.
          else
          {
            //Write the changes to the FS before returning.
            updateSFS();
            return noBytesWritten;
          }
        }
        
        //A block already exists.
        else
        {
          currentBlock = fat[currentBlock].nextFatIndex;
        }
      }
    }
    
    //Write the changes to the FS.
    updateSFS();
  }
  
  //File does not exist.
  else
  {
    return F_NOPEN_FOR_WRITING_ERROR;
  }
 
  //Returns the number of bytes written.
  return noBytesWritten;
}

/*
This function returns the number of bytes actually read from disk.
*/
int sfs_read(int fileID, char *buf, int length)
{
  char *buffNew;
  int currentBlock=0;
  int currentOffset=0;
  int noBytesToRead=0;
  int noBytesRead = 0;
  
  //Check if file is open before anything.
  if(fd[fileID].fatIndex != 0)
  {
    //Determine which block to start reading from.
    currentBlock = fd[fileID].fatIndex;
    currentOffset = fd[fileID].readIndex;
    
    //Go to the data block where we have to start reading from.
    while(currentOffset > blockSize)
    {
      currentBlock = fat[currentBlock].nextFatIndex;
      currentOffset -= blockSize;
    }
    
    //This loop executes until we have not read at least lenght bytes.
    while(noBytesRead != length)
    {
      //Will hold the data read from disk.
      buffNew = (char *)malloc(blockSize * sizeof(char));
      
      //Read the current block into the buffer.
      read_blocks(currentBlock, 1, buffNew);
      
      //Determine the number of bytes to read.
      noBytesToRead = (length - noBytesRead) > blockSize - currentOffset ? blockSize - currentOffset: length - noBytesRead;
      
      //Copy the bytes from the block to the buffer passed into the function.
      memcpy(buf + noBytesRead, buffNew + currentOffset, noBytesToRead);
      
      //Free the buffer which holds the data from the block.
      free(buffNew);
      
      //Update the number of bytes read thus far.
      noBytesRead += noBytesToRead;
      #if DEBUG == 1
        printf("sfs_read(): Bytes to read is %d. Bytes read thus far is %d.\n",length,noBytesRead);
      #endif
      
      //Update the readIndex and set the offset to 0.
      fd[fileID].readIndex += noBytesToRead;
      currentOffset = 0;
      
      //We're not done yet.
      if(noBytesRead != length)
      {  
        //We're not done reading so find the next block to read.
        currentBlock = fat[currentBlock].nextFatIndex;
        
        //If there is no next block, return an error.
        if(currentBlock == 0){
          return F_READ_OUT_OF_BOUNDS;
        }
      }else{
        //If we have read more than what we were supposed to, we return
        //the number of bytes we were supposed to have read.
        if(noBytesRead >= directory[fileID].fileSize)
        {
          return directory[fileID].fileSize;
        }
      }
    }
    
  }
  
  //The file hasn't been opened as there is no entry in the fd table. Thus,
  //we return an error.
  else
  {
    return F_NOPEN_FOR_READING_ERROR;
  }
  
  //Return the number of bytes read.
  return noBytesRead;
}

void initialize()
{
  //The number of blocks to include in the files is the 3 blocks + the total number of 
  //possible files.
  noBlocks = MAX_FILES + 3;
  
  //Create the free list.
  freeList = list_create(destroyFreeList);
  
  //Determine the size of the block on disk.
  blockSize = MAX_FILES * sizeof(directory_entry_t);
  if(MAX_FILES * sizeof(fdt_t) > blockSize)
  {
    blockSize = MAX_FILES * sizeof(fdt_t);
  }
  else if(noBlocks * sizeof(fat_entry_t) > blockSize)
  {
    blockSize = noBlocks * sizeof(fat_entry_t);
  }
  #if DEBUG == 1
    printf("initialize(): The size of each block will be %d.\n",blockSize);
  #endif
      
  //Create a pointer to the directory table.
  directory = (directory_entry_t*)malloc(blockSize);
  
  //Create a pointer to the file access table.
  fat = (fat_entry_t*)malloc(blockSize);
  
  //Create a pointer to the file descriptor table.
  fd = (fdt_t*)malloc(MAX_FILES * sizeof(fdt_t));
  
  //The block is for the directory table.
  fat[1].blockIndex = 1;
  
  //The second block is for the fa table.
  fat[2].blockIndex = 2;
}

/*
This function creates a new file and returns the index of the file in
the directory table.
*/
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
    #if DEBUG == 1
      printf("createFile(): Putting an entry in the directory table for file %s with index %d.\n",name,newFileIndex);
    #endif
    strcpy(directory[newFileIndex].fileName,name);
    directory[newFileIndex].fileSize = 0;
    time(&directory[newFileIndex].dateModified);
    directory[newFileIndex].fatIndex = list_shift_int(freeList);
    
    //Update the fat by putting an entry for the newly created file.
    #if DEBUG == 1
      printf("createFile(): Putting an entry in the fat table for file %s with index %d.\n",name,newFileIndex);
    #endif
    fat[newFileIndex].blockIndex = directory[newFileIndex].fatIndex;
    fat[newFileIndex].nextFatIndex = 0;
    
    //Update the fd table by putting an entry for the newly created file.
    #if DEBUG == 1
      printf("createFile(): Putting an entry in the fd table for file %s with index %d.\n",name,newFileIndex);
    #endif
    fd[newFileIndex].fatIndex = directory[newFileIndex].fatIndex;
    fd[newFileIndex].readIndex = 0;
    fd[newFileIndex].writeIndex = 0;
  }
  
  //Update the FS.
  updateSFS();
  
  //Return the index (position in the directory table) of the newly created file.
  return newFileIndex;
}

/*
This function writes content to disk.
*/
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