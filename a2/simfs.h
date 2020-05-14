#include <stdio.h>
#include "simfstypes.h"

/* File system operations */
void printfs(char *);
void initfs(char *);
// Added by myself
void creating(char *, char *);
void deleting(char *, char *);
void reading(char *, char *, char *, char *);
void writing(char *, char *, char *, char *);


/* Internal functions */
FILE *openfs(char *filename, char *mode);
void closefs(FILE *fp);