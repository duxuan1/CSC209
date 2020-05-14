/* This file contains functions that are not part of the visible "interface".
 * They are essentially helper functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "simfs.h"

/* Internal helper functions first.
 */

FILE *
openfs(char *filename, char *mode)
{
    FILE *fp;
    if((fp = fopen(filename, mode)) == NULL) {
        perror("openfs");
        exit(1);
    }
    return fp;
}

void
closefs(FILE *fp)
{
    if(fclose(fp) != 0) {
        perror("closefs");
        exit(1);
    }
}

/* File system operations: creating, deleting, reading, and writing to files.
 */

// Signatures omitted; design as you wish.

// return the index of first empty entry
int findEmptyEntry(char *filename, fentry fe[MAXFILES]) {
    int index = 0;
    int boolean = 0;
    while (boolean == 0 && index < MAXFILES) {
        if ((fe[index].name[0]) == '\0'){
            boolean = 1;
        }
        index ++;
    }
    if (boolean == 1) {
        return index - 1;
    } else {
        return -1;
    }
}

// return the index of first empty blcok
int activeBlocks(char *filename, fnode fn[MAXBLOCKS]) {
    int index = 0;
    int count = 1;
    while (count == 0 && index < MAXBLOCKS) {
        if (fn[index].blockindex >= 0){
            count++;
        }
        index ++;
    }
    return count;
}

// return the index of first empty blcok
int findEmptyBlock(char *filename, fnode fn[MAXBLOCKS]) {
    int index = 0;
    int boolean = 0;
    while (boolean == 0 && index < MAXBLOCKS) {
        if (fn[index].blockindex < 0){
            boolean = 1;
        }
        index ++;
    }
    if (boolean == 1) {
        return index - 1;
    } else {
        return - 1;
    }
}

// return the index of wanted file
int findFile(char *filename, char *file, fentry fe[MAXFILES]) {
    int index = 0;
    int boolean = 0;
    while (boolean == 0 && index < MAXFILES) {
        if (strcmp((fe[index].name), file) == 0){
            boolean = 1;
        }
        index ++;
    }
    if (boolean == 1) {
        return index - 1;
    } else {
        return -1;
    }
}

// read fentry, fnode and blocks from file to memory
void readData(char *filename, char *file, fentry fe[MAXFILES], fnode fn[MAXBLOCKS], char *block) {
    FILE *fp = openfs(filename, "r");
    // read data to local files and nodes
    if ((fread(fe, sizeof(fentry), MAXFILES, fp)) == 0) {
        fprintf(stderr, "Error: could not read file entries\n");
        closefs(fp);
        exit(1);
    }
    if ((fread(fn, sizeof(fnode), MAXBLOCKS, fp)) == 0) {
        fprintf(stderr, "Error: could not read fnodes\n");
        closefs(fp);
        exit(1);
    }
    int header_block_used = (sizeof(fentry) * MAXFILES + sizeof(fnode) * MAXBLOCKS) / BLOCKSIZE;
    int numBlocks = findEmptyBlock(filename, fn) - header_block_used;
    fread(block, numBlocks * BLOCKSIZE, 1, fp);
    closefs(fp);
}

// helper function for creating
void createHelper(char *filename, char *file, fentry fe[MAXFILES], fnode fn[MAXBLOCKS], char *block) {
    // open file
    FILE *fp = openfs(filename, "r+b");
    // check file not exist
    if (strlen(file) > sizeof(fentry) - 5) {
        fprintf(stderr, "Error: name too long\n");
        closefs(fp);
        exit(1);
    }
    if (findFile(filename, file, fe) != -1) {
        fprintf(stderr, "Error: file already exist\n");
        closefs(fp);
        exit(1);
    }
    // find first empty entry and create file into this entry
    if (findEmptyEntry(filename, fe) == -1) {
        fprintf(stderr, "Error: all entry used\n");
        closefs(fp);
        exit(1);
    } else {
        int index = findEmptyEntry(filename, fe);
        strcpy(fe[index].name, file);
    }
    // writemeta data to fentry and fnode
    if(fwrite(fe, sizeof(fentry), MAXFILES, fp) < MAXFILES) {
        fprintf(stderr, "Error: write failed on init\n");
        closefs(fp);
        exit(1);
    }
    if(fwrite(fn, sizeof(fnode), MAXBLOCKS, fp) < MAXBLOCKS) {
        fprintf(stderr, "Error: write failed on init\n");
        closefs(fp);
        exit(1);
    }
    // write blocks
    int header_block_used = (sizeof(fentry) * MAXFILES + sizeof(fnode) * MAXBLOCKS) / BLOCKSIZE;
    int numBlocks = findEmptyBlock(filename, fn) - header_block_used;
    fwrite(block, numBlocks * BLOCKSIZE, 1, fp);
    // close file
    closefs(fp);
}

// count how many blocks does the file own
int countBlocks(char *filename, char *file, fentry fe[MAXFILES], fnode fn[MAXBLOCKS]) {
    int fileindex = findFile(filename, file, fe);
    int first = fe[fileindex].firstblock;
    if (first == -1) {
        return 0;
    }
    int curr = fn[first].blockindex;
    int count = 1;
    if (fn[first].nextblock == -1) {
        return count;
    }
    while (curr != -1) {
        count += 1;
        first = fn[first].nextblock;
        curr = fn[first].nextblock;
    }
    return count;
}

// count how many blocks does the file own
int findBlocks(char *filename, char *file, int index, fentry fe[MAXFILES], fnode fn[MAXBLOCKS]) {
    int fileindex = findFile(filename, file, fe);
    int first = fe[fileindex].firstblock;
    int count = 1;
    while (count != index) {
        count += 1;
        first = fn[first].nextblock;
    }
    return first;
}

// initialize a new block with index to all zeros 
void initializeBlock(char *filename, char *file, fentry fe[MAXFILES], fnode fn[MAXBLOCKS], char *block) {
    FILE *fp = openfs(filename, "r+b");
    int fileindex = findFile(filename, file, fe);
    int first = fe[fileindex].firstblock;
    int emptyblock;
    if (findEmptyBlock(filename, fn) == -1) {
        fprintf(stderr, "Error: all block used\n");
        closefs(fp);
        exit(1);
    } else {
        emptyblock = findEmptyBlock(filename, fn);
    }
    if (first == -1) {
        // assign the block to file
        int header_block_used = (sizeof(fentry) * MAXFILES + sizeof(fnode) * MAXBLOCKS) / BLOCKSIZE;
        fe[emptyblock - header_block_used].firstblock = emptyblock;
        fn[emptyblock].blockindex = abs(fn[emptyblock].blockindex);
    } else {
        int curr = fn[first].blockindex;
        int next = fn[first].nextblock;
        int initialized = 0;
        while (initialized == 0) {
            if (next == -1) {
                fn[curr].nextblock = emptyblock;
                fn[emptyblock].blockindex = abs(fn[emptyblock].blockindex);
                initialized = 1;
            }
            first = fn[first].nextblock;
            curr = fn[first].blockindex;
            next = fn[first].nextblock;
        }
    }
    fseek(fp, emptyblock * BLOCKSIZE, SEEK_SET);
    char zerobuf[BLOCKSIZE] = {0};
    fwrite(zerobuf, BLOCKSIZE, 1, fp);
    closefs(fp);
}   

int floorDivision(int a, int b) {
    int result;
    if (a % b != 0) {
        result = a / b + 1;
    } else {
        result = a / b;
    }
    return result;
}

int calculateSize(char *filename, char *file, fentry fe[MAXFILES], fnode fn[MAXBLOCKS], char *block) {
    FILE *fp = openfs(filename, "r");
    int fileindex = findFile(filename, file, fe);
    int first = fe[fileindex].firstblock;
    int blocks_have = countBlocks(filename, file, fe, fn);
    if (blocks_have == 0) {
        closefs(fp);
        return 0;
    }
    int size = 0;
    while (blocks_have != 0) {
        for (int i = 0; i < BLOCKSIZE; i++) {
            fseek(fp, first * BLOCKSIZE + i, SEEK_SET);
            char str[1];
            fread(str, 1, 1, fp);
            if (strcmp(str, "")) {
                size += 1;
            }
        }
        first = fn[first].nextblock;
        blocks_have--;
    }
    closefs(fp);
    return size;
}

void writeBlocks(char *filename, char *file, int start, int length, 
fentry fe[MAXFILES], fnode fn[MAXBLOCKS], char *block) {
    FILE *fp = openfs(filename, "r+b");
    // check file exist
    if (findFile(filename, file, fe) == -1) {
        fprintf(stderr, "Error: file not found\n");
        closefs(fp);
        exit(1);
    }
    // check start
    if (start != 0) {
        int size = calculateSize(filename, file, fe, fn, block);
        if (start > size) {
            fprintf(stderr, "Error: invalid start\n");
            closefs(fp);
            exit(1);
        }
    }
    // initialize all needed blocks
    char str[(BLOCKSIZE+1) * MAXBLOCKS];
    fread(str, length, 1, stdin);
    if (strlen(str) < length) {
        fprintf(stderr, "Error: length larger than stdin\n");
        closefs(fp);
        exit(1);
    }
    int blocks_to_use = floorDivision(start + length, BLOCKSIZE);
    if (blocks_to_use - activeBlocks(filename,fn) - countBlocks(filename,file ,fe ,fn)> MAXBLOCKS) {
        fprintf(stderr, "Error: need blocks more than max blocks\n");
        closefs(fp);
        exit(1);
    }    
    int blocks_built = countBlocks(filename, file, fe, fn);
    while (blocks_built < blocks_to_use) {
        initializeBlock(filename, file, fe, fn, block); 
        blocks_built ++;
    }
    // write stdin to blocks
    int blockstart = start % BLOCKSIZE;
    int blockrank = start / BLOCKSIZE + 1;
    int blockindex = findBlocks(filename, file, blockrank, fe, fn);
    int curr = 0;
    char *pointer = &str[0];
    if ((blockstart + length) <= BLOCKSIZE) {
        fseek(fp, blockindex * BLOCKSIZE + blockstart, SEEK_SET);
        fwrite(pointer, length, 1, fp);
    } else {
        while (blocks_to_use  != 0) {
            blockstart = start % BLOCKSIZE;
            blockrank = start / BLOCKSIZE + 1;
            blockindex = findBlocks(filename, file, blockrank, fe, fn);
            int blocklength;
            if (length > BLOCKSIZE) {
                blocklength = BLOCKSIZE - blockstart;
            } else {
                blocklength = length;
            }
            fseek(fp, blockindex * BLOCKSIZE + blockstart, SEEK_SET);
            fwrite(pointer, blocklength, 1, fp);
            start += blocklength;
            length -= blocklength;
            curr += blocklength;
            pointer = &str[curr];
            blocks_to_use --;
        }
    }
    closefs(fp);
}

void writeHelper(char *filename, char *file, fentry fe[MAXFILES], fnode fn[MAXBLOCKS], char *block, int start, int length) {
    // open the file
    FILE *fp = openfs(filename, "r+b");
    writeBlocks(filename, file, start, length, fe, fn, block);

    int size = calculateSize(filename, file, fe, fn, block);
    int fileindex = findFile(filename, file, fe);
    fe[fileindex].size = size;

    if(fwrite(fe, sizeof(fentry), MAXFILES, fp) < MAXFILES) {
        fprintf(stderr, "Error: write failed on fentry\n");
        closefs(fp);
        exit(1);
    }
    if(fwrite(fn, sizeof(fnode), MAXBLOCKS, fp) < MAXBLOCKS) {
        fprintf(stderr, "Error: write failed on fnode\n");
        closefs(fp);
        exit(1);
    }
    // close the file
    closefs(fp);
}

void readBlocks(char *filename, char *file, int start, int length, 
fentry fe[MAXFILES], fnode fn[MAXBLOCKS], char *block) {
    FILE *fp = openfs(filename, "r");
    // check file exist
    if (findFile(filename, file, fe) == -1) {
        fprintf(stderr, "Error: file not found\n");
        closefs(fp);
        exit(1);
    }
    // check start
    int size = calculateSize(filename, file, fe, fn, block);
    if (start + length> size) {
        fprintf(stderr, "Error: invalid start and length\n");
        closefs(fp);
        exit(1);
    }
    // initialize all needed blocks
    int blocks_have = countBlocks(filename, file, fe, fn);
    // read blocks to stdout
    int blockstart = start % BLOCKSIZE;
    int blockrank = start / BLOCKSIZE + 1;
    int blockindex = findBlocks(filename, file, blockrank, fe, fn);
    int curr = 0;

    if ((blockstart + length) <= BLOCKSIZE) {
        char str[BLOCKSIZE+1];
        fseek(fp, blockindex * BLOCKSIZE + blockstart, SEEK_SET);
        fread(str, 1, length, fp);
        fwrite(str, strlen(str), 1, stdout);
    } else {
        while (blocks_have != 0) {
            blockstart = start % BLOCKSIZE;
            blockrank = start / BLOCKSIZE + 1;
            blockindex = findBlocks(filename, file, blockrank, fe, fn);
            int blocklength;
            if (length > BLOCKSIZE) {
                blocklength = BLOCKSIZE - blockstart;
            } else {
                if ((blockstart + length) >= BLOCKSIZE) {
                    blocklength = BLOCKSIZE - blockstart;
                } else {
                    blocklength = length;
                }
            }
            char str[blocklength];
            fseek(fp, blockindex * BLOCKSIZE + blockstart, SEEK_SET);
            fread(str, 1, blocklength, fp);
            fwrite(str, strlen(str), 1, stdout);
            start += blocklength;
            length -= blocklength;
            curr += blocklength;
            blocks_have --;
        }
    }
    closefs(fp);
}

void readHelper(char *filename, char *file, fentry fe[MAXFILES], fnode fn[MAXBLOCKS], char *block, int start, int length) {
    // open the file
    FILE *fp = openfs(filename, "r");
    readBlocks(filename, file, start, length, fe, fn, block);
    // close the file
    closefs(fp);
}

void deleteBlocks(char *filename, char *file, fentry fe[MAXFILES], fnode fn[MAXBLOCKS], char *block) {
    FILE *fp = openfs(filename, "r+b");
    // check file exist
    if (findFile(filename, file, fe) == -1) {
        fprintf(stderr, "Error: file not found\n");
        closefs(fp);
        exit(1);
    }
    int fileindex = findFile(filename, file, fe);
    int first = fe[fileindex].firstblock;
    if (first == -1) {
        // assign the block to file
        fe[fileindex].name[0] = '\0';
        fe[fileindex].size = 0;
        fe[fileindex].firstblock = -1;
    } else {
        int blocks_have = countBlocks(filename, file, fe, fn);
        while (blocks_have != 0) {
            fseek(fp, first * BLOCKSIZE, SEEK_SET);
            char zerobuf[BLOCKSIZE] = {0};
            fwrite(zerobuf, BLOCKSIZE, 1, fp);
            fn[first].blockindex = -1 * first;
            int temp = fn[first].nextblock;
            fn[first].nextblock = -1;
            first = temp;
            blocks_have--;
        }
    }
    fe[fileindex].name[0] = '\0';
    fe[fileindex].size = 0;
    fe[fileindex].firstblock = -1;
    closefs(fp);
}

void deleteHelper(char *filename, char *file, fentry fe[MAXFILES], fnode fn[MAXBLOCKS], char *block) {
    // open file
    FILE *fp = openfs(filename, "r+b");
    // find first empty entry and create file into this entry
    deleteBlocks(filename, file, fe, fn, block);
    // writemeta data to fentry and fnode
    if(fwrite(fe, sizeof(fentry), MAXFILES, fp) < MAXFILES) {
        fprintf(stderr, "Error: write failed on init\n");
        closefs(fp);
        exit(1);
    }
    if(fwrite(fn, sizeof(fnode), MAXBLOCKS, fp) < MAXBLOCKS) {
        fprintf(stderr, "Error: write failed on init\n");
        closefs(fp);
        exit(1);
    }
    // write blocks
    int header_block_used = (sizeof(fentry) * MAXFILES + sizeof(fnode) * MAXBLOCKS) / BLOCKSIZE;
    int numBlocks = findEmptyBlock(filename, fn) - header_block_used;
    fwrite(block, numBlocks * BLOCKSIZE, 1, fp);
    // close file
    closefs(fp);
}

void creating(char *filename, char *file) {
    fentry files[MAXFILES];
    fnode fnodes[MAXBLOCKS];
    char block[(BLOCKSIZE + 1) * MAXBLOCKS];
    readData(filename, file, files, fnodes, block);
    createHelper(filename, file, files, fnodes, block);
}

void reading(char *filename, char *file, char *start, char *length) {
    fentry files[MAXFILES];
    fnode fnodes[MAXBLOCKS];
    int s = strtol(start, NULL ,10);
    int l = strtol(length, NULL ,10);
    char block[(BLOCKSIZE + 1) * MAXBLOCKS];
    readData(filename, file, files, fnodes, block);
    readHelper(filename, file, files, fnodes, block, s, l);
}

void writing(char *filename, char *file, char *start, char *length) {
    fentry files[MAXFILES];
    fnode fnodes[MAXBLOCKS];
    int s = strtol(start, NULL ,10);
    int l = strtol(length, NULL ,10);
    char block[(BLOCKSIZE + 1) * MAXBLOCKS];
    readData(filename, file, files, fnodes, block);
    writeHelper(filename, file, files, fnodes, block, s, l);
}

void deleting(char *filename, char *file) {
    fentry files[MAXFILES];
    fnode fnodes[MAXBLOCKS];
    char block[(BLOCKSIZE + 1) * MAXBLOCKS];
    readData(filename, file, files, fnodes, block);
    deleteHelper(filename, file, files, fnodes, block);
}