#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define FREE       	    99
#define ALLOCATED  	    100
#define EXTRA_MEMORY 	  128*1024

typedef struct block_info{
  struct block_info* next; 
  struct block_info* prev;
  void* address;
  int availability;
  int size;
}block_info;

static block_info* fixedHead;
static block_info* head;
static block_info* tail;
static int initialized = 0;

void* my_malloc(int requestedSize)
{
    block_info* block;

    if(!initialized)
    {
      initialized = 1;

      block_info* topBlock = (block_info *)sbrk(sizeof(block_info));
      topBlock->address = (void *)sbrk(requestedSize + EXTRA_MEMORY);
      topBlock->availability = FREE;
      topBlock->size = requestedSize + EXTRA_MEMORY;

      block_info* bottomBlock = (block_info *)sbrk(sizeof(block_info));
      bottomBlock -> address = topBlock ->address;
      bottomBlock -> availability = FREE;
      bottomBlock -> size = requestedSize + EXTRA_MEMORY;
      
      fixedHead = topBlock;
      head = topBlock;
      tail = topBlock;
    }

    block = head;
    while(block)
    {
       if(block->size > requestedSize && block->availability == FREE)
       {
           //Case 1: It is not possible to split the free block into a smaller portion
           //as there won't be enough memory to store the information about the free
           //block. Thus, we return the block with excess memory.
           if(block->size - requestedSize <= 2*sizeof(block_info))
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
             
             //Allocate the free block by returning the address to the beginning of the block.
             return block->address;
           }

           //Case 2: We can allocated the requested memory and there is still enough memory
           //left to store information about the segmented block size. Thus, we partition
           //the available memory.
           else
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
             if(block->prev != 0){
                block->prev->next = newSegmentedBlockInfo;
             }
             if(block->next != 0){
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
             
             //Allocate the free block by returning the address to the beginning of the block.
             return block->address;
           }
       }
       
       else
       {
         if(block->next == 0)
         {
           block_info* topBlock = (block_info *)sbrk(sizeof(block_info));
           topBlock->address = (void *)sbrk(requestedSize + EXTRA_MEMORY);
           topBlock->availability = FREE;
           topBlock->size = requestedSize + EXTRA_MEMORY;

           block_info* bottomBlock = (block_info *)sbrk(sizeof(block_info));
           bottomBlock -> address = topBlock ->address;
           bottomBlock -> availability = FREE;
           bottomBlock -> size = requestedSize + EXTRA_MEMORY;
           if(head == block){
            head = topBlock;
           }
           tail = topBlock;
           block = topBlock;
         }
         else
         {
           block = block->next;
         }
         
       }
    }
}

void my_free(void* ptr)
{
  block_info* currentSegmentTopMeta = (block_info *)(ptr - sizeof(block_info));
  block_info* currentSegmentBottomMeta = (block_info *)(ptr + currentSegmentTopMeta->size);
  block_info* topSegmentBottomMeta = (block_info*)(ptr - 2*sizeof(block_info));
  block_info* topSegmentTopMeta = (block_info *)(ptr - 2*sizeof(block_info) - topSegmentBottomMeta->size - sizeof(block_info));
  block_info* bottomSegmentTopMeta = (block_info *)(ptr + currentSegmentTopMeta->size + sizeof(block_info));
  block_info* bottomSegmentBottomMeta = (block_info *)(ptr + currentSegmentTopMeta->size + 2*sizeof(block_info) + bottomSegmentTopMeta->size);
  int topBlockAvailabilityFlag = topSegmentBottomMeta -> availability;
  int bottomBlockAvailabilityFlag = bottomSegmentTopMeta -> availability;
  
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
    currentSegmentTopMeta -> prev = bottomSegmentBottomMeta -> prev;
    currentSegmentTopMeta -> next = bottomSegmentBottomMeta -> next;
    if(currentSegmentTopMeta -> prev){
      block_info* tmp = currentSegmentTopMeta -> prev;
      tmp -> next = currentSegmentTopMeta;
    }
    if(currentSegmentTopMeta -> next){
      block_info* tmp = bottomSegmentBottomMeta -> next;
      tmp -> prev = currentSegmentTopMeta;
    }
    
    //If the memory being freed comes before the previous memory that is 
    //free, make this the head.
    if(head > topSegmentTopMeta){
      head = currentSegmentTopMeta;
    }
    
    //Clear the memory that is between the top and bottom meta blocks.
    memset(currentSegmentTopMeta -> address, 0, newSize);   
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
    topSegmentTopMeta->next = bottomSegmentBottomMeta->next;
    if(topSegmentTopMeta -> next){
      block_info* tmp = topSegmentTopMeta -> next;
      tmp->prev = topSegmentTopMeta;
    }
    
    //Clear the memory that is between the top and bottom meta blocks.
    memset(topSegmentTopMeta -> address, 0, newSize);
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
      currentSegmentTopMeta->prev = tmp2;
    }
  }
}