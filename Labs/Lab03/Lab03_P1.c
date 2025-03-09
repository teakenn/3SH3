#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 10

// Define the page properties based on a 4-KB page size (2^12 bytes)
#define OFFSET_BITS 12
#define PAGE_SIZE (1 << OFFSET_BITS)      // 4096 bytes
#define OFFSET_MASK (PAGE_SIZE - 1)         // 0xFFF or 4095
#define PAGES 8                           // The program uses 8 pages

int main(void) {
    // Open the labaddr.txt file for reading.
    FILE *fptr = fopen("labaddr.txt", "r");
    if (fptr == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Define the page table for 8 pages.
    int page_table[PAGES] = {6, 4, 3, 7, 0, 1, 2, 5};

    // Buffer to hold each address
    char buff[BUFFER_SIZE];

    // Read each line
    while (fgets(buff, BUFFER_SIZE, fptr) != NULL) {
    
        char *newline = strchr(buff, '\n');
        if (newline) {
            *newline = '\0';
        }

        // Convert the string to an integer (logical address)
        int logical_addr = atoi(buff);

        // Compute the page number
        int page_number = logical_addr >> OFFSET_BITS;

        // Compute the offset
        int offset = logical_addr & OFFSET_MASK;

        int frame_number = page_table[page_number];

        // Shift the frame number left by OFFSET_BITS and combine it with the offset.
        int physical_addr = (frame_number << OFFSET_BITS) | offset;

        printf("Virtual addr is %d: Page# = %d & Offset = %d. Physical addr = %d.\n",
               logical_addr, page_number, offset, physical_addr);
    }

    // Close 
    fclose(fptr);
    return 0;
}