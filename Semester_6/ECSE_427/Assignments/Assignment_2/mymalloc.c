#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include "mymalloc.h"

#define FREE       	    99
#define ALLOCATED  	    100
#define EXTRA_MEMORY 	  128*1024

/*Allways points to the beginning of the free list. Was used for debuggin purposes.*/
static block_info* fixedHead;
/*Points to the first free block in the free list.*/
static block_info* head;
/*Points to the last free block in the free list.*/
static block_info* tail;
/*Flag to keep a track of whether a block was initialized for the first time.*/
static int initialized = 0;
/*Policy to use while allocating memory.*/
static int policyToUse = FIRST_FIT;
/*Flag to keep track of the total number of bytes allocated thus far (includes meta blocks).*/
static int totalBytesAllocated = 0;
/*Flag to keep track of the total number of free bytes in the free list (not counting meta blocks).*/
static int totalFreeBytes = 0;
/*Flag to keep track of the largest free block in the free list.*/
static int largestFreeSpace = 0;

/*
This function returns a pointer to a memory block that is at least as large as 
the requested size. It uses either the First-Fit or Best-Fit policy to
allocated a block of memory. If there is not enough memory in the free list it creates
more memory using the sbrk call.
*/
void* my_malloc(int requestedSize)
{
  block_info* block;

  if(!initialized)
  {
    initialized = 1;

    //Setup the top meta block.
    block_info* topBlock = (block_info *)sbrk(sizeof(block_info));
    topBlock->address = (void *)sbrk(requestedSize + EXTRA_MEMORY);
    topBlock->availability = FREE;
    topBlock->size = requestedSize + EXTRA_MEMORY;

    //Setup the bottom meta block.
    block_info* bottomBlock = (block_info *)sbrk(sizeof(block_info));
    bottomBlock -> address = topBlock ->address;
    bottomBlock -> availability = FREE;
    bottomBlock -> size = requestedSize + EXTRA_MEMORY;

    //Setup the initial pointers to the free list.
    fixedHead = topBlock;
    head = topBlock;
    tail = topBlock;
    
    //Store information for later printing.
    totalBytesAllocated += sizeof(block_info) + requestedSize + EXTRA_MEMORY + sizeof(block_info);
    totalFreeBytes += requestedSize + EXTRA_MEMORY;
    largestFreeSpace = requestedSize + EXTRA_MEMORY;
  }

  block = head;
  if(policyToUse == FIRST_FIT)
  {
    while(block)
    {
      if(block->size > requestedSize && block->availability == FREE)
      {
        //Case 1: It is not possible to split the free block into a smaller portion
        //as there won't be enough memory to store the information about the free
        //block. Thus, we return the block with excess memory.
        if(block->size - requestedSize <= 2*sizeof(block_info))
        {
          return allocateExtraMemory(block,requestedSize);
        }

       //Case 2: We can allocated the requested memory and there is still enough memory
       //left to store information about the segmented block size. Thus, we partition
       //the available memory.
        else
        {
          return allocateMemoryAndStoreRemaining(block,requestedSize);
        }
      }

      else
      {
        if(block->next == 0)
        {
          //Case 3: We don't have enough memory in the free list. Thus, we have to create
          //more memory.
          return createMoreMemory(block,requestedSize);
        }
        else
        {
          block = block->next;
        }
      }
    }
  }

  else
  {
    block_info* bestFit = block;
    int foundAFit = 0;

    while(block){
      if(block->size >= requestedSize && block->availability==FREE)
      {
        //Determine if the free block is the best fit so far.
        if(block -> size < bestFit -> size || bestFit -> size < requestedSize)
        {
          bestFit = block;
          foundAFit = 1;
        }
      }
      block = block -> next;
    }

    if(foundAFit)
    {
      //Case 1: It is not possible to split the free block into a smaller portion
      //as there won't be enough memory to store the information about the free
      //block. Thus, we return the block with excess memory.
      if(bestFit->size - requestedSize <= 2*sizeof(block_info))
      {
        return allocateExtraMemory(bestFit,requestedSize);
      }

      //Case 2: We can allocated the requested memory and there is still enough memory
      //left to store information about the segmented block size. Thus, we partition
      //the available memory.
      else
      {
        return allocateMemoryAndStoreRemaining(bestFit,requestedSize);
      }
    }
    else
    {
      //Case 3: We don't have enough memory in the free list. Thus, we have to create
      //more memory.
      return createMoreMemory(tail,requestedSize);
    }
  }
}

/*
This function frees the block of memory pointed to by pointer. Furthermore, if there
are neighboring blocks that are free, the given block is coalesced with those.
*/
void my_free(void* ptr)
{
  //Get the metablocks for the neighbors to determine if they're available or not.
  block_info* currentSegmentTopMeta = (block_info *)(ptr - sizeof(block_info));
  block_info* currentSegmentBottomMeta = (block_info *)(ptr + currentSegmentTopMeta->size);
  block_info* topSegmentBottomMeta = (block_info *)ptr<head?NULL:(block_info*)(ptr - 2*sizeof(block_info));
  block_info* topSegmentTopMeta = (block_info *)ptr<head?NULL:(block_info *)(ptr - 2*sizeof(block_info) - topSegmentBottomMeta->size - sizeof(block_info));
  block_info* bottomSegmentTopMeta = (block_info *)ptr>tail?NULL:(block_info *)(ptr + currentSegmentTopMeta->size + sizeof(block_info));
  block_info* bottomSegmentBottomMeta = (block_info *)ptr>tail?NULL:(block_info *)(ptr + currentSegmentTopMeta->size + 2*sizeof(block_info) + bottomSegmentTopMeta->size);
  int topBlockAvailabilityFlag = (block_info *)ptr<head?ALLOCATED:topSegmentBottomMeta -> availability;
  int bottomBlockAvailabilityFlag = (block_info *)ptr>tail?ALLOCATED:bottomSegmentTopMeta -> availability;
  
  //Case 1: The segment being freed is below an already free segment.
  if(topBlockAvailabilityFlag == FREE && bottomBlockAvailabilityFlag == ALLOCATED)
  {
    int topBlockFreeSize = topSegmentBottomMeta -> size;
    int newSize = topBlockFreeSize + currentSegmentTopMeta->size + 2*sizeof(block_info); 
    
    //Update the values of the top meta block for the coalesced segment.
    topSegmentTopMeta -> availability = FREE;
    topSegmentTopMeta -> size = newSize;
   
    //Update the values of the bottom meta block for the coalesced segment.
    currentSegmentBottomMeta -> availability = FREE;
    currentSegmentBottomMeta -> address = topSegmentTopMeta -> address;
    currentSegmentBottomMeta -> size = newSize;
    
    //If the memory being freed comes before the previous memory that is 
    //free, make this the head.
    if(head > topSegmentTopMeta){
      head = topSegmentTopMeta;
    }
    
    //Clear the memory that is between the top and bottom meta blocks.
    memset(topSegmentTopMeta -> address, 0, newSize);
    
    //For printing the stats later
    totalFreeBytes += newSize - topBlockFreeSize;
    largestFreeSpace = getLargestFreeSpace();
  }
  
  //Case 2: The segment being freed is above an already free segment.
  else if(topBlockAvailabilityFlag == ALLOCATED && bottomBlockAvailabilityFlag == FREE)
  {
    int bottomSegmentFreeSize = bottomSegmentTopMeta -> size;
    int newSize = currentSegmentTopMeta->size + 2*sizeof(block_info) + bottomSegmentFreeSize;
    
    //Update the values of the top meta block for the coalesced segment.
    currentSegmentTopMeta -> availability = FREE;
    currentSegmentTopMeta -> size = newSize;
   
    //Update the values of the bottom meta block for the coalesced segment.
    bottomSegmentBottomMeta -> availability = FREE;
    bottomSegmentBottomMeta -> address = currentSegmentTopMeta -> address;
    bottomSegmentBottomMeta -> size = newSize;
    
    //Update the pointers
    currentSegmentTopMeta -> prev = bottomSegmentTopMeta -> prev;
    currentSegmentTopMeta -> next = bottomSegmentTopMeta -> next;
    if(currentSegmentTopMeta -> prev){
      block_info* tmp = currentSegmentTopMeta -> prev;
      tmp -> next = currentSegmentTopMeta;
    }
    if(currentSegmentTopMeta -> next){
      block_info* tmp = bottomSegmentTopMeta -> next;
      tmp -> prev = currentSegmentTopMeta;
    }
    
    //If the memory being freed comes before the previous memory that is 
    //free, make this the head.
    if(head > topSegmentTopMeta){
      head = currentSegmentTopMeta;
    }
    //If the block being coalesced with is the tail, then, we move the tail up.
    if(bottomSegmentTopMeta == tail){
      tail = currentSegmentTopMeta;
    }
    //Clear the memory that is between the top and bottom meta blocks.
    memset(currentSegmentTopMeta -> address, 0, newSize);   
    
    //For printing the stats later
    totalFreeBytes += newSize - bottomSegmentFreeSize;
    largestFreeSpace = getLargestFreeSpace();
  }
  
  //Case 3: The segment being freed is between two segments that were already freed.
  else if(topBlockAvailabilityFlag == FREE && bottomBlockAvailabilityFlag == FREE)
  {
    int topBlockFreeSize = topSegmentBottomMeta -> size;
    int bottomSegmentFreeSize = bottomSegmentTopMeta -> size;
    int newSize = topBlockFreeSize + currentSegmentTopMeta->size + bottomSegmentFreeSize + 4*sizeof(block_info);
    
    //Update the values of the top meta block for the coalesced segment.
    topSegmentTopMeta -> availability = FREE;
    topSegmentTopMeta -> size = newSize;
    
    //Update the values of the bottom meta block for the coalesced segment.
    bottomSegmentBottomMeta -> availability = FREE;
    bottomSegmentBottomMeta -> address = topSegmentTopMeta -> address;
    bottomSegmentBottomMeta -> size = newSize;
    
    //Update the pointers
    topSegmentTopMeta->next = bottomSegmentTopMeta->next;
    if(topSegmentTopMeta -> next){
      block_info* tmp = topSegmentTopMeta -> next;
      tmp->prev = topSegmentTopMeta;
    }
    
    //If the memory being freed comes before the previous memory that is 
    //free, make this the head.
    if(head > topSegmentTopMeta){
      head = topSegmentTopMeta;
    }
    
    //Clear the memory that is between the top and bottom meta blocks.
    memset(topSegmentTopMeta -> address, 0, newSize);
    
    //For printing the stats later
    totalFreeBytes += newSize - topBlockFreeSize - bottomSegmentFreeSize;
    largestFreeSpace = getLargestFreeSpace();
  }
  
  //Case 4: There are no segments directly above or below the current segment that is being freed.
  else
  {
    currentSegmentTopMeta -> availability = FREE;
    currentSegmentBottomMeta -> availability = FREE;
    
    if(currentSegmentTopMeta < head)
    { //There are no free elements before this one.
      currentSegmentTopMeta -> next = head;
      head = currentSegmentTopMeta;
    }
    else
    { //There are free elements before this one. Thus, we have to set the next
      //pointer of the free element preceding the current element. We also have
      //to set the previous pointer of the current element to that same block.
      block_info* tmp1 = head;
      block_info* tmp2 = head;
      while(tmp1 < currentSegmentTopMeta)
      {
        tmp2 = tmp1;
        tmp1 = tmp1->next;
      }
      tmp2->next = currentSegmentTopMeta;
      currentSegmentTopMeta->prev = tmp2;
    }
    
    if(currentSegmentTopMeta > tail)
    { //There are no free elements after this one.
      currentSegmentTopMeta -> prev = tail;
      tail = currentSegmentTopMeta;
    }
    else
    { //There are free elements after this one. Thus, we have to set the prev 
      //pointer of the free element following the current element. We also have
      //to set the next pointer of the current element to that same block.
      block_info* tmp1 = tail;
      block_info* tmp2 = tail;
      while(tmp1 > currentSegmentTopMeta)
      {
        tmp2 = tmp1;
        tmp1 = tmp1->prev;
      }
      tmp2->prev = currentSegmentTopMeta;
      currentSegmentTopMeta->next = tmp2;
    }
    
    //For printing the stats later
    totalFreeBytes += currentSegmentTopMeta->size;
    largestFreeSpace = getLargestFreeSpace();
  }
}

/*
This function sets the policy to use while allocating memory.
*/
void my_mallopt(int policy)
{
  policyToUse = policy;
}

/*
This function prints statistics.
*/
void my_mallinfo()
{
  printf("Total allocated bytes: %d (%d kB)\n",totalBytesAllocated,totalBytesAllocated/1024);
  printf("Total free bytes: %d (%d kB)\n",totalFreeBytes,totalFreeBytes/1024);
  printf("Largest free space: %d (%d kB)\n",largestFreeSpace,largestFreeSpace/1024);
}

/*
This function handles Case 1 - itt is not possible to split the free block into a smaller portion
as there won't be enough memory to store the information about the free
block. Thus, we return the block with excess memory.
*/
void* allocateExtraMemory(block_info* block, int requestedSize)
{
  block_info* bottomBlock; 
             
  //Change the availability flag of the top and bottom flags associated
  //with this block being allocated.
  block->availability = ALLOCATED;
  bottomBlock = (block_info*)((void *)block->address + block->size); 
  bottomBlock->availability = ALLOCATED;

  //Update the pointers.
  if(block->prev != 0){
    block->prev->next = block->next;
  }
  if(block->next != 0){
    block->next->prev = block->prev;
  }
  if(head == block && block->next != 0)
  {
    head = block->next;
  }
  if(tail == block && block->prev !=0)
  {
    tail = block->prev;
  }
  block->next = 0;
  block->prev = 0;

  //For printing the stats later
  totalFreeBytes -= block->size;
  largestFreeSpace = getLargestFreeSpace();
  
  //Allocate the free block by returning the address to the beginning of the block.
  return block->address;
}

/*
This function handles Case 2 - We can allocated the requested memory and there is still enough memory
left to store information about the segmented block size. Thus, we partition
the available memory.
*/
void* allocateMemoryAndStoreRemaining(block_info* block, int requestedSize)
{
  block_info* newSegmentedBlockInfo;
  int prevSize =  block->size;

  //Update the availability and size fiels of the top and bottom blocks
  //associated with the segment being allocated.
  block->availability = ALLOCATED;
  block->size = requestedSize;
  memcpy((void *)block->address + requestedSize, block, sizeof(block_info));

  //Create a new meta block for the free segment that was partitioned.
  newSegmentedBlockInfo = (block_info *)memcpy((void *)block->address + requestedSize + sizeof(block_info),block,sizeof(block_info));
  newSegmentedBlockInfo -> address = (void *)block->address + requestedSize + 2*sizeof(block_info);
  newSegmentedBlockInfo -> availability = FREE;
  newSegmentedBlockInfo -> size = prevSize - requestedSize - 2*sizeof(block_info);
  memcpy((void *)block->address + requestedSize + 2*sizeof(block_info) + newSegmentedBlockInfo -> size, newSegmentedBlockInfo,sizeof(block_info));
 
  //Update the pointers.
  if(block->prev != 0)
  {
    block->prev->next = newSegmentedBlockInfo;
  }
  if(block->next != 0)
  {
    block->next->prev = newSegmentedBlockInfo;
  }
  if(head == block)
  {
    head = newSegmentedBlockInfo;
  }
  if(tail == block)
  {
    tail = newSegmentedBlockInfo;
  }
  newSegmentedBlockInfo -> next = block->next;
  newSegmentedBlockInfo -> prev = block->prev;
  block->next = 0;
  block->prev = 0;
 
  //For printing the stats later
  totalFreeBytes -= requestedSize;
  largestFreeSpace = getLargestFreeSpace();
  
  //Allocate the free block by returning the address to the beginning of the block.
  return block->address;
}

/*
This function handles Case 3 - We don't have enough memory in the free list. Thus, we have to create
more memory.
*/
void* createMoreMemory(block_info* block, int requestedSize)
{
  //Setup the top meta block.
  block_info* topBlock = (block_info *)sbrk(sizeof(block_info));
  topBlock->address = (void *)sbrk(requestedSize + EXTRA_MEMORY);
  topBlock->availability = FREE;
  topBlock->size = requestedSize + EXTRA_MEMORY;

  //Setup the bottom meta block.
  block_info* bottomBlock = (block_info *)sbrk(sizeof(block_info));
  bottomBlock -> address = topBlock ->address;
  bottomBlock -> availability = FREE;
  bottomBlock -> size = requestedSize + EXTRA_MEMORY;
  
  //Adjust pointers.
  if(head -> availability == ALLOCATED){
    head = topBlock;
  }
  if(tail-> availability == FREE){
    tail->next = topBlock;
    topBlock -> prev = tail;
  }
  tail = topBlock;
  block = topBlock;
 
  //For printing the stats later
  totalBytesAllocated += sizeof(block_info) + requestedSize + EXTRA_MEMORY + sizeof(block_info);
  totalFreeBytes += requestedSize + EXTRA_MEMORY;
  largestFreeSpace = getLargestFreeSpace();
  
  //Allocate from the memory created.
  if(block->size - requestedSize <= 2*sizeof(block_info))
  {
    return allocateExtraMemory(block,requestedSize);
  }
  else
  {
    return allocateMemoryAndStoreRemaining(block,requestedSize);
  }
}

/*
This function goes through the free list to find largest free block.
*/
int getLargestFreeSpace(){
  block_info* tmp = head;
  int largest = 0;
  while(tmp){
    if(tmp->size > largest){
      largest = tmp->size;
    }
    tmp = tmp->next;
  }
  return largest;
}