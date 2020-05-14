#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>


// Message to print in the signal handling function. 
#define MESSAGE "%ld reads were done in %ld seconds.\n"


/* Global variables to store number of read operations and seconds elapsed.
 * These have to be global so that signal handler to be written will have
 * access.
 */
long num_reads = 0, seconds;

void handler(int code) {
    fprintf(stderr, MESSAGE, num_reads, seconds);
    exit(1);
}

int main(int argc, char ** argv) {
    
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }   

    seconds = strtol(argv[1], NULL, 10);

    
    struct itimerval c; 
    struct timeval s;
    struct timeval s1;
    s1.tv_sec = 0;
    s1.tv_usec = 0;
    s.tv_sec = seconds;
    s.tv_usec = 0;
    c.it_value =  s;
    c.it_interval = s1;


    setitimer(ITIMER_PROF, &c, NULL);

    FILE *fp;
    if ((fp = fopen(argv[2], "r+")) == NULL) {    // Read+Write for later ...
      perror("fopen");
      exit(1);
    }

    /* In an infinite loop, read an int from a random location in the file
     * and print it to stderr.
     */
    // Declare a struct to be used by the sigaction function:
    struct sigaction newact;

    // Specify that we want the handler function to handle the
    // signal:
    newact.sa_handler = handler;

    // Use default flags:
    newact.sa_flags = 0;

    // Specify that we don't want any signals to be blocked during
    // execution of handler:
    sigemptyset(&newact.sa_mask);

    // Modify the signal table so that handler is called when
    // signal SIGINT is received:
    sigaction(SIGPROF, &newact, NULL);

    for (;;) {
        int r = (random()%100) * sizeof(int);   
        int num;  
        fseek(fp, r, SEEK_SET);
        fread(&num, sizeof(int), 1, fp);
        printf("%d\n", num);
        num_reads += 1;
    }

    return 1;  //something is wrong if we ever get here!
}

