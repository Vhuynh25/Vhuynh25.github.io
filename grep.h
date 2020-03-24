void printmatch();
int advance(char *lp, char *ep);
int append(int (*f)(void), unsigned int *a);
int backref(int i, char *lp);
int cclass(char *set, int c, int af);
void compile(char* argv);
void error(char *s);
int execute(char *file);
void quit(int n);



