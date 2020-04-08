int fwalk(FILE* f1, FILE* f2, int* linenum);
int fwalk_brief(FILE* f1, FILE* f2);
void print_version();
int opt_check(const char* s);
char strcmp2(char *s1,const char *s2);
int fline(FILE* f, const char* s);
void print_right(char* tokbuf, const char* delims);
void print_left(char* tokbuf, const char* delims);
