/*
 * Editor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "grep.h"

/* make BLKSIZE and LBSIZE 512 for smaller machines */
char   *patternptr1;
char pat[100];
int argcount; 

int main(int argc, char *argv[]) {
	char *p1, *p2;
	FILE* fin;
	int i = 2;
	argcount = argc;

	patternptr1 = strcpy(pat,argv[1]);

	while(i < argc){
	  fin = fopen(argv[i++],"r");
	  commands(fin,i);
	}
	quit(0);
	return 0;
}

void commands(FILE* fin,int i) {  
  global(fin,i);
  // global(0);
}

void quit(int n) {
  // 0 = match
  if (n == 0){}
  // 1 = no match
  if (n == 1){
    fprintf(stderr, "status: no match found.\n");
    exit(1);
  }
  // 2 = error
  if (n == 2){
    fprintf(stderr, "status: error with pattern or matching.\n");
    exit(1);
  }
}

void global(FILE* fin,int i) {
  
  char* lineptr = NULL;
  size_t linesize = 1000;
  int linelen;
  regex_t regex;
  int regex_value;

  regex_value = regcomp(&regex, patternptr1, 0);
  if (regex_value){
    quit(2);
  }
  
  while ((linelen = getline(&lineptr,&linesize,fin)) >= 0){
    regex_value = regexec(&regex, lineptr,0,NULL,0);
    if (!regex_value){
      if (argcount > 3){ printf("file number %d: " , i-2); }
       printf("%s",lineptr);
    }
  }

  free(lineptr);
  lineptr = NULL;
  fclose(fin);
}
