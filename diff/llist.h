#ifndef llist_h
#define llist_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct lnode lnode;
struct lnode{
  lnode* last;
  lnode* next;
  int linenum;
  char* line;
};

typedef struct dlist dlist;
struct dlist{
  lnode* head;
  lnode* tail;
  size_t size;
};

lnode* create_lnode(int num, char* line);
dlist* create_dlist();

bool list_empty(dlist* l);
int list_pop_front(dlist* l);
int list_pop_back(dlist* l);

lnode* list_push_front(dlist* l,int linenum, char* line);
lnode* list_push_back(dlist* l,int linenum, char* line);

void list_print(dlist* l);
void list_print_reverse(dlist* l);
void list_clear(dlist* l);

#endif /* llist_h */
