#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>  /* For mmap() */
#include <fcntl.h>     /* For open() */
#include <string.h>    /* For memcpy() */
#include <unistd.h>    /* For close() */

#define INT_SIZE 4             // size of an int
#define INT_COUNT 10           // 10 int
#define MEMORY_SIZE (INT_COUNT * INT_SIZE)  // total bytes for mapping

int main(void) {
    int intArray[INT_COUNT];
    int sum = 0;
    
    int fd = open("numbers.bin", O_RDONLY);
    if (fd < 0) {
        perror("Error opening numbers.bin");
        exit(EXIT_FAILURE);
    }
    
    // map the file.
    signed char *mmapfptr = mmap(NULL, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mmapfptr == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    // copy each int from the memmap region into intArray.
    for (int i = 0; i < INT_COUNT; i++) {
        memcpy(&intArray[i], mmapfptr + (i * INT_SIZE), INT_SIZE);
    }
    
    // unmap
    if (munmap(mmapfptr, MEMORY_SIZE) == -1) {
        perror("Error unmapping file");
    }
    
    // summing
    for (int i = 0; i < INT_COUNT; i++) {
        sum += intArray[i];
    }
    
    printf("Sum of numbers = %d\n", sum);
    
    close(fd);
    
    return 0;
}