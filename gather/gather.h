#include <stdio.h>
#include <regex.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>

void gather(char* filename);
void dirsearch(const char* s,const char* pattern);
int options_check(const char* opt);
