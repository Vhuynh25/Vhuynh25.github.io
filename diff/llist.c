#include "llist.h"

// std list functions for line nodes

lnode* create_lnode(int num, char* line){ // create new line node given line number and line
  lnode *newnode = (lnode*)malloc(sizeof(lnode));
  newnode->linenum = num;
  newnode->line = line;
  newnode->last = NULL;
  newnode->next = NULL;
  return newnode;
}

dlist* create_dlist(){
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
  int linenum = holder->linenum;
  l->head = l->head->next;
  if (l->size > 1) {l->head->last = NULL;}
  free(holder);
  l->size -= 1;
  return linenum;
}

int list_pop_back(dlist* l){
  if (list_empty(l)){
    printf("List is empty\n");
    exit(1);
  }
  lnode* holder = l->tail;
  int linenum = holder->linenum;
  l->tail = l->tail->last;
  if (l->size > 1) {l->tail->next = NULL;}
  free(holder);
  l->size -= 1;
  return linenum;
}

lnode* list_push_front(dlist* l,int linenum, char* line){
  lnode* newhead = create_lnode(linenum,line);
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

lnode* list_push_back(dlist* l,int linenum, char* line){
  lnode* newtail = create_lnode(linenum,line);
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
    printf("%d\n", index->linenum);
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
    printf("%d\n", index->linenum);
    index = index->last;
  }
}

void list_clear(dlist* l){
  while(!list_empty(l)){
    list_pop_front(l);
  }
  free(l);
}

lnode* list_findline(lnode* n, char* s){
  lnode *index = n;
  while(index){
    if (strcmp(index->line,s) == 0){return index;} 
  }
  return NULL;
}
