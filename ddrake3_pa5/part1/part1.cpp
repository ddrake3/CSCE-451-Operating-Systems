/****************************************************************
 *
 * Author: Derek Drake
 * Title: CSCE-451 PA5
 * Date: 04/20/2021
 * Description: a virtual memory manager with infinite memory space
 * 							i.e. no need for page replacement
 *							this solution uses FIFO replacement for TLB updating
 *
 ****************************************************************/

	#include<stdio.h>
	#include<stdlib.h>

/******************************************************
 * Declarations
 ******************************************************/
// #Define'd sizes
// we need to accommodate for all 256 pages from BACKING_STORE.bin
  #define FRAMES 256
  #define FRAME_SIZE 256

  #define TABLES_OF_PAGES 256
  #define TABLE_SIZE 16

  #define ADDRESS_SIZE 12
  #define PAGE_SIZE 256

// input & correct files
	FILE *backingStore;
	FILE *addressFile;
	FILE *correct;

// arrays for storing BACKING_STORE.bin & addresses
  char address[ADDRESS_SIZE];
  signed char backValue[PAGE_SIZE];
  signed char value;

// Make the TLB array
  int pageTLB[TABLE_SIZE];
  int frameTLB[TABLE_SIZE];

// Make the Page Table
  int pageTable[TABLES_OF_PAGES];

// Make the memory
  int memory[FRAMES][FRAME_SIZE];

// ints for counting
  int pageFaultCount = 0;
  int TLBhitCount = 0;
  int TLBEntryCount = 0;
	int addressCount = 0;

// ints for tracking
  int logicalAddress = 0;
  int frameNumber = -1;
  int firstFreeFrame = 0;
  int firstFreePage = 0;

// bools for tracking
   bool isInitialized = false;
   bool TLBhit = false;

/******************************************************
 * Assumptions:
 *   If you want your solution to match follow these assumptions
 *   1. In Part 1 it is assumed memory is large enough to accommodate
 *      all frames -> no need for frame replacement
 *   2. Part 1 solution uses FIFO for TLB updates
 *   3. In the solution binaries it is assumed a starting point at frame 0,
 *      subsequently, assign frames sequentially
 *   4. In Part 2 you should use 128 frames in physical memory
 ******************************************************/

int main(int argc, char * argv[]) {

		// argument processing
		backingStore = fopen(argv[1], "rb");
		addressFile = fopen(argv[2], "r");
		correct = fopen("correct.txt", "w+");

    // read addresses.txt
		while(fgets(address, ADDRESS_SIZE, addressFile) != NULL) {
      logicalAddress = atoi(address);

      // Step 0:
      // get page number and offset
      //   bit twiddling
      int offset = logicalAddress & 0xff;
      int pageNumber = logicalAddress >> 8;

      // initialize arrays
      if (!isInitialized){
        for (int i = 0; i < TABLES_OF_PAGES; i++) {
            pageTable[i] = -1;
         }

         for (int i = 0; i < TABLE_SIZE; i++) {
           pageTLB[i] = -1;
           frameTLB[i] = -1;
         }

         isInitialized = true;
     }

     // need to get the physical address (frame + offset):
     // Step 1: check in TLB for frame
     frameNumber = -1;
     TLBhit = false;

     for(int i = 0; i < TABLE_SIZE; i++) {
       if(pageTLB[i] == pageNumber) {
         frameNumber = frameTLB[i];
         TLBhitCount++;
         TLBhit = true;
         break;
       }
     }

     //   if !get_frame_TLB() -> :(
     if(frameNumber == -1) {
       //     Step 2: not in TLB, look in page table
       for(int i = 0; i <= pageNumber; i++) {
         if(pageNumber == i) {
           frameNumber = pageTable[i];
         }
       }

       if(frameNumber == -1) {
         //       PAGE_FAULT!
         //       Step 3:
         //       dig up frame in BACKING_STORE.bin (backing_store_to_memory())
         //       bring in frame page# x 256
         //       store in physical memory
         fseek(backingStore, pageNumber * PAGE_SIZE, SEEK_SET);
         fread(backValue, sizeof(signed char), PAGE_SIZE, backingStore);
         for(int i = 0; i < PAGE_SIZE; i++) {
           memory[firstFreeFrame][i] = backValue[i];
         }

         //       Step 4:
         //       update page table with corresponding frame from storing
         //         into physical memory
         pageTable[pageNumber] = firstFreeFrame;
         firstFreeFrame++;
         firstFreePage++;
         pageFaultCount++;
         frameNumber = firstFreeFrame - 1;
       }
     }

     if(!TLBhit) {
       //   Step 5: (always) update TLB when we find the frame
       //     update TLB (updateTLB())
       for(int i = 0; i < TLBEntryCount; i++) {
         if(pageTLB[i] == pageNumber) {
           break;
         }
       }
       if(TLBEntryCount >= TABLE_SIZE) {
         TLBEntryCount = 0;
       }
       pageTLB[TLBEntryCount] = pageNumber;
       frameTLB[TLBEntryCount] = frameNumber;
       TLBEntryCount++;
     }

     //   Step 6: read val from physical memory
     value = memory[frameNumber][offset];
     fprintf(correct,"Virtual address: %d Physical address: %d Value: %d\n", logicalAddress, (frameNumber << 8) | offset, value);
      addressCount++;
		}

    double pageFaultRate = (double)pageFaultCount / (double)addressCount;
    double TLBHitRate = (double)TLBhitCount / (double)addressCount;

		fprintf(correct, "Number of Translated Addresses = %d\n", addressCount);
		fprintf(correct, "Page Faults = %d\n", pageFaultCount);
		fprintf(correct, "Page Fault Rate = %.3f\n", pageFaultRate);
		fprintf(correct, "TLB Hits = %d\n", TLBhitCount);
		fprintf(correct, "TLB Hit Rate = %.3f\n", TLBHitRate);

    fclose(backingStore);
		fclose(addressFile);
}
