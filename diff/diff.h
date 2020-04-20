#ifndef diff_h
#define diff_h


int linewalk(lnode* left, lnode* right);
int fwalk_brief(FILE* f1, FILE* f2);
void need_change(lnode* left, lnode* right);
lnode* need_add(lnode* left, lnode* right,dlist* leftfile);
lnode* need_delete(lnode* left, lnode* right, dlist* rightfile);
void walk_line(dlist* file1,dlist* file2);
  
void print_version();
int opt_check(const char* s);
int hamming_dist(char *s1,const char *s2);
lnode* fline(lnode* node, const char* s);
dlist* linereading(FILE* f, dlist* l);
void print_right(lnode* start, lnode* end);
void print_left(lnode* start, lnode* end);

#endif /* diff_h */
