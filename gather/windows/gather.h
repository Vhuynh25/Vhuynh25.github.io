#include <stdio.h>
#include <regex.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* gather(void* filename);
void dirsearch(const char* s,const char* pattern);
int options_check(const char* opt);
