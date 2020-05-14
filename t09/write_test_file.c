#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: create a file\n");
        exit(1);
    }   

    FILE *fp = fopen(argv[1], "w");
       
    for (int i = 0; i < 100; i++) {
        int r = random()%100;     
        fwrite(&r, sizeof(int), 1, fp);
    }
    if(fclose(fp) != 0) {
        perror("closefs");
        exit(1);
    }
}      