#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "diff.h"
#define BUFMAX 1000
#define TOKMAX 100000
int brief_mode = 0;
int side_by_side = 0;
int report_identical_files = 0;

//linked list stuff - hold lines in a linked list
typedef struct lnode lnode;
struct lnode{
  lnode* last;
  lnode* next;
  int linenum;
  char* line;
};

lnode* create_lnode(int num, char* line){
  lnode *newnode = (lnode*)malloc(sizeof(lnode));
  newnode->linenum = num;
  newnode->line = line;
  newnode->last = NULL;
  newnode->next = NULL;
  return newnode;
}

typedef struct dlist dlist;
struct dlist{
  lnode* head;
  lnode* tail;
  size_t size;
};

dlist* create_list(){
  dlist *newlist = (dlist*)malloc(sizeof(dlist));
  newlist->head = NULL;
  newlist->tail = NULL;
  newlist->size = 0;
  return newlist;
}

bool list_empty(dlist* l){return l->size == 0;}

int list_pop_front(dlist* l){
  if (list_empty(l)){
    printf("List is empty\n");
    exit(1);
  }
  lnode* holder = l->head;
  int data = holder->data;
  l->head = l->head->next;
  if (l->size > 1) {l->head->last = NULL;}
  free(holder);
  l->size -= 1;
  return data;
}

int list_pop_back(dlist* l){
  if (list_empty(l)){
    printf("List is empty\n");
    exit(1);
  }
  lnode* holder = l->tail;
  int data = holder->data;
  l->tail = l->tail->last;
  if (l->size > 1) {l->tail->next = NULL;}
  free(holder);
  l->size -= 1;
  return data;
}

lnode* list_push_front(dlist* l,int data){
  lnode* newhead = create_lnode(data);
  if (list_empty(l)){
    l->head = newhead;
    l->tail = newhead;
  }
  else {
    l->head->last = newhead;
    newhead->next = l->head;
    l->head = newhead;
  }
  l->size += 1;
  return l->head;
}

lnode* list_push_back(dlist* l,int data){
  lnode* newtail = create_lnode(data);
  if (list_empty(l)){
    l->head = newtail;
    l->tail = newtail;
  }
  else {
    newtail->last = l->tail;
    l->tail = l->tail->next = newtail;
    
  }
  l->size += 1;
  return l->tail;
}

void list_print(dlist* l){
  if (list_empty(l)){
    printf("List is empty\n");
    return;
  }
  lnode* index = l->head;
  while(index){
    printf("%d\n", index->data);
    index = index->next;
  }
}

void list_print_reverse(dlist* l){
  if (list_empty(l)){
    printf("List is empty\n");
    return;
  }
  lnode* index = l->tail;
  while(index){
    printf("%d\n", index->data);
    index = index->last;
  }
}

void list_clear(dlist* l){
  while(!list_empty(l)){
    list_pop_front(l);
  }
  free(l);
}

void list_test(){
  dlist* list = create_list();
  list_push_front(list,0);
  list_push_front(list,1);
  list_push_back(list,2);
  list_print_reverse(list);
  list_pop_front(list);
  list_pop_back(list);
  list_print(list);
  list_clear(list);
}

int main(int argc, const char* argv[]){
  list_test();
  return 0;
}

int main(int argc, const char* argv[]){

  if (argc < 2){
    fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
    return 1;
  }
  argv++;

  while(opt_check(argv[0]) != 0){argv++;}
  
  FILE *f1;
  FILE *f2;
  if ((f1 = fopen(argv[0], "r")) == NULL) {
    fprintf(stderr, "Error: could not open file 1");
    return 1;
  }
  if ((f2 = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error: could not open file 2");
    return 1;
  }

  int b = -1;

  if (brief_mode == 1 && (b = fwalk_brief(f1,f2)) == 1){
    printf("Files %s and %s differ\n", argv[0],argv[1]);
  }
  if (report_identical_files == 1 && b == 0){
    printf("Files %s and %s are identical\n", argv[0],argv[1]);
  }
  else {
    int linenum = 0;
    while (brief_mode == 0 && fwalk(f1,f2,&linenum) != -1){}
  }

  return 0; // need some way to keep track of linenums and lines from both files for stuff like 1a1,6 and 1,16d1 | add if we can't find s2 in file 1 | delete if we can't find s1 in file 2
  
}

int fwalk(FILE* f1, FILE* f2, int* linenum){ 
  // with line from file 2 walk through file 1 starting at same line number
  
  char s1[BUFMAX]; // buffers for strings from both files
  char s2[BUFMAX]; // size of 100000 for strcat and strtok later.
  char tokbuf[TOKMAX]; // holds all f1 lines in case of add or delete

  int startnum = *linenum;
  // 0 == no diff, 1 == change, 2 == add, 3 == delete
  
  fgets(s2, BUFMAX, f2);
  fgets(s1, BUFMAX, f1);
  if(strlen(s1) == 0 && strlen(s2) == 0){return -1;}
  
  if (strcmp(s1,s2) == 0){} // do work depending on -s status
  else if (strcmp2(s1,s2) != 1){}
  else if (fline(f1,s2) != 0){ // if we don't find it in file 1, add
    *linenum += 1; // increment line num
    strcat(tokbuf,s2); // append to tokbuf
    while (fgets(s2, BUFMAX, f2) != NULL && fline(f1,s2) != 0){
      // keep checking lines in file 2 if they are in file 1,
      // add to tokbuf if they aren't
      *linenum += 1;
      strcat(tokbuf,s2);
    }
    if (startnum == *linenum){printf("%da%d",startnum,startnum);}
    else {printf("%d,%da%d",startnum,*linenum,startnum);}
    print_right(tokbuf,"\n");
  }
  else if (fline(f2,s1) != 0){
    *linenum += 1; // increment line num
    strcat(tokbuf,s1); // append to tokbuf
    while (fgets(s1, BUFMAX, f1) != NULL && fline(f2,s1) != 0){
      // keep checking lines in file 1 if they are in file 2,
      // add to tokbuf if they aren't
      *linenum += 1;
      strcat(tokbuf,s2);
    }
    if (startnum == *linenum){printf("%da%d",startnum,startnum - 1);}
    else {printf("%d,%dd%d",startnum,*linenum,startnum);} // delete
    print_left(tokbuf,"\n");
  }
}

int fwalk_brief(FILE* f1, FILE* f2){
  char c1,c2;
  while ((c1 = fgetc(f1)) == (c2 = fgetc(f2)) && c1 != EOF && c2 != EOF){}
  if (c1 == c2){return 0;}
  return 1;
}

void print_version(){
  printf("creator: Vien Huynh\n");
  printf("diff version 1.00\n");
  //printf();
}

int opt_check(const char* s){
  if (strcmp(s,"-v") == 0){
    print_version();                exit(0);
  }
  else if (strcmp(s,"-q") == 0 || strcmp(s,"--brief") == 0){
    brief_mode = 1;                 return 1;
  }
  else if (strcmp(s,"-s") == 0 || strcmp(s,"--report-identical-files") == 0){
    report_identical_files = 1;     return 1;
  }
  else if (strcmp(s,"-y") == 0 || strcmp(s,"--side-by-side") == 0){
    side_by_side = 1;     return 1;
  }

  return 0; // if no option was found
}

char strcmp2(char *s1,const char *s2){
  // special strcmp that returns first different char
  // returns 1 if no same char in prefix
  // should return '\0' if the two strings are the same
  char* ptr;
  *ptr = 1;
  while ((*s1 == *s2++) && *(ptr = s1++) != '\0'){}
  return *ptr;
}

int fline(FILE* f, const char* s){ // find a line within a file
  long position = ftell(f);
  char fs[BUFMAX];
  int result;
  while (fgets(fs,BUFMAX,f) != NULL){
    if ((result = strcmp(fs,s)) == 0){
      fseek(f,position,0);
    }
  }
  return result;
}

void print_right(char* tokbuf, const char* delims){ // print right file lines
  char *ptr = strtok(tokbuf,delims);
  while (ptr != NULL){
    printf("> %s\n", ptr);
    ptr = strtok(NULL,delims);
  }
  return ;
}

void print_left(char* tokbuf, const char* delims){ // print left file lines
  char *ptr = strtok(tokbuf,delims);
  while (ptr != NULL){
    printf("< %s\n", ptr);
    ptr = strtok(NULL,delims);
  }
  return ;
}

