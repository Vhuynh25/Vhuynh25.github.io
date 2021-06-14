#include "llist.h"
#include "diff.h"
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#define BUFMAX 1000
#define TOKMAX 100000
// the following global variables are all options
int brief_mode = 0;
int side_by_side = 0;
int report_identical_files = 0;
int left_columns = 0;
int suppress_common = 0;
int num = 3;
int unified = 0;
int context = 0;
const char* empty = "\n"; // empty line

int main(int argc, const char* argv[]){

  if (argc < 2){
    fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
    return 1;
  }

  while(opt_check(argv[1]) != 0){argv++;} // check for options and if end of options opt_check returns 0
  
// this is a simplified version of diff
// for purpose of this program, file 1 is being compared to file 2
// aka all suggestions are for file 1 to match file 2
  FILE *f1;
  FILE *f2;

  dlist* file1;
  file1 = create_dlist();
  dlist* file2;
  file2 = create_dlist();
  char *fpath1,*fpath2;
  char ftime1[100];
  char ftime2[100];
  struct stat fstat1, fstat2;
  if ((f1 = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error: could not open file 1");
    return 1;
  } else {
// read all of file 1 into a list 
// each node in the list is a class that holds a line from a file with a line count
    file1 = linereading(f1,file1); 
    stat(argv[1],&fstat1);
    strftime(ftime1, 100,"%Y-%m-%d %H:%M:%S",localtime(&(fstat1.st_ctime)));
    fpath1 = realpath(argv[1],NULL); // get path for file 2
  }
  if ((f2 = fopen(argv[2], "r")) == NULL) {
    fprintf(stderr, "Error: could not open file 2");
    return 1;
  } else {
// file 2
    file2 = linereading(f2,file2);
    stat(argv[2],&fstat2);
    strftime(ftime2, 100 ,"%Y-%m-%d %H:%M:%S",localtime(&(fstat2.st_ctime)));
    fpath2 = realpath(argv[2],NULL);
  }
  fclose(f1);
  fclose(f2);

  int b = -1;

  if ((b = fwalk_brief(f1,f2)) == 1 && brief_mode == 1 ){ // b == 1 means files are different
    printf("Files %s and %s differ\n", argv[0],argv[1]);
  }
  if (report_identical_files == 1 && b == 0){
    printf("Files %s and %s are identical\n", argv[0],argv[1]);
  }
  else if (brief_mode == 0){ // if brief mode is off
    if (unified == 1){ // print the file name
      printf("--- %s\n", fpath1);
      printf("+++ %s\n", fpath2);
    }
    else if (context == 1){
      printf("*** %s %s\n", argv[1],ftime1);
      printf("--- %s %s\n", argv[2],ftime2);
    }
    walk_line(file1,file2);
  }

  return 0; 
}

void walk_line(dlist* file1,dlist* file2){
  lnode *left = file1->head;
  lnode *right = file2->head;
  int hamming,count; 
  count = num; // count is for the context and unified options
  while (strlen(left->line) != 0 && strlen(right->line) != 0){
    if (strcmp(left->line,right->line) == 0){ 
	// if the lines are the same, print lines based on options
      if (side_by_side == 1 && suppress_common == 0 && left_columns == 0){
	printf("  %s\n", left->line);
      }
      else if (left_columns == 1) {printf("(\n");}
      if (count <= num && (unified == 1 || context == 1)){
	printf("  %s\n",left->line);
	count++;
      }
      left = left->next;
      right = right->next;
    } // do work depending on options
    else if ((hamming = hamming_dist(left->line,right->line)) < 6 &&
	     hamming > 0){ 
	// if hamming distance between two line is < 6 but > 0 aka the lines are only a small number of letters off of each other
      if (side_by_side == 0) 
	{need_change(left, right);}
      else { printf("| %s\n", left->line); }
      left = left->next;
      right = right->next;
      count = 1;
    }
    else if (fline(left,right->line) == NULL){
      // if we don't find the line from file 2 in file 1, add
      right = need_add(left,right,file1);
      count = 1;
    }
    else if (fline(right,left->line) == NULL){ 
	// if we don't find the line from file 1 in file 2, delete
      left = need_delete(left,right,file2);
      count = 1;
    }
  }
  // add/delete when files are not same length
  if (right->next != NULL){
    right = need_add(left->last,right,file1);
    printf("End of File\n");
  }
  if (left->next != NULL){
    left = need_delete(left,right->last,file2);
    printf("End of File\n");
  }
}

lnode* need_add(lnode* left, lnode* right,dlist* leftfile){
  lnode* rightstart = right;
  lnode* leftlist = leftfile->head;
  while (fline(leftlist,right->next->line) == NULL ||
	 strcmp(right->next->line,empty) == 0){
    right = right->next;
    // keep checking lines in file 2 if they are in file 1,
  }
 // print the difference
  if (rightstart->linenum == right->linenum){
    printf("%da%d\n",left->linenum - 1, rightstart->linenum);
  }
  else if (side_by_side == 0){
    printf("%da%d,%d\n",left->linenum - 1,rightstart->linenum,right->linenum);
  }
  print_right(rightstart,right);
  return right->next;
}

lnode* need_delete(lnode* left, lnode* right, dlist* rightfile){
  lnode* leftstart = left;
  lnode* rightlist = rightfile->head;
  while (fline(rightlist,left->next->line) == NULL ||
	 strcmp(left->next->line,empty) == 0){
    left = left->next;
    // keep checking lines in file 1 if they are in file 2,
  }
 // print the difference
  if (leftstart->linenum == left->linenum){
    printf("%da%d\n",left->linenum, right->linenum - 1);}
  else if (side_by_side == 0){
    printf("%d,%dd%d\n",leftstart->linenum,left->linenum,right->linenum - 1);
  }
  // delete
  print_left(leftstart,left);

  return left->next;
}

void need_change(lnode* left, lnode* right){ // print out the two lines that differ
  printf("%dc%d\n",left->linenum, right->linenum);
  print_left(left,left);
  printf("-----------------------\n");
  print_right(right,right);
}

int fwalk_brief(FILE* f1, FILE* f2){ // if both files are exactly the same, then return 0 else return 1
  char c1,c2;
  while ((c1 = fgetc(f1)) == (c2 = fgetc(f2)) && c1 != EOF && c2 != EOF){}
  if (c1 == c2){return 0;}
  return 1;
}

//-------------------------------------------------

void print_version(){
  printf("creator: Vien Huynh\n");
  printf("diff version 1.00\n");
  //printf();
}

int opt_check(const char* s){
  char* ptr = NULL;
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
  else if (strcmp(s,"-c") == 0){
    context = 1;     return 1;
  }
  else if (strcmp(s,"-u") == 0){
    unified = 1;     return 1;
  }
  else if ((context == 1 || unified == 1) && isdigit(*s)){
    num = atoi(s);     return 1;
  }
  else if (strcmp(s,"--suppress-common-lines") == 0){
    suppress_common = 1;     return 1;
  }
  else if (strcmp(s,"--left-column") == 0){
    left_columns = 1;     return 1;
  }
  else if (strstr(s,"-unified=") != NULL){
    
    unified = 1;
    if (isdigit(ptr =  strrchr(s,'=') + 1 )){num = atoi(ptr);}
    return 1;
  }
  else if (strstr(s,"-context=") != NULL){
    
    context = 1;
    if (isdigit(ptr =  strrchr(s,'=') + 1 )){num = atoi(ptr);}
    return 1;
  }

  return 0; // if no option was found
}

int hamming_dist(char *s1,const char *s2){
  // return '0' if the two strings are the same
// return the number of letters that two strings differ by
  int dist;
  dist = 0;
  while (*s1 != '\0' && *s2 != '\0'){
    if(*s1++ != *s2++){dist++;}
  }
  return dist;
}

dlist* linereading(FILE* f, dlist* l){ // read entire file into list
  int linecount = 0;
  size_t zero = 0;
  char *empty = "\n";
  char *ptr = NULL;
  do {
    linecount++;
    char* holder = NULL;
    getline(&holder,&zero,f);
    ptr = holder;
    list_push_back(l,linecount,holder);
  } while (strlen(ptr) != 0);
  return l;
}

lnode* fline(lnode* node, const char* s){ // find a line within the line list
  lnode* p = node;
  while (p != NULL && strcmp(p->line,s) != 0){
    p = p->next;
  }
  return p;
}

void print_right(lnode* start, lnode* end){ // print right file lines
  lnode *ptr = start;
  while (ptr != end->next && ptr != NULL){
    if (unified != 0 || context != 0){printf("+ %s\n",ptr->line);}
    else { printf("> %s\n", ptr->line);}
    ptr = ptr->next;
  }
  return ;
}

void print_lines(lnode* start, lnode* end){ // print file lines
  lnode *ptr = start;
  while (ptr != end->next && ptr != NULL){
    printf("  %s\n",ptr->line);
   ptr = ptr->next;
  }
  return ;
}

void print_left(lnode* start, lnode* end){ // print left file lines
  lnode *ptr = start;
  while (ptr != end->next && ptr != NULL){
    if (unified != 0 || context != 0){printf("- %s\n",ptr->line);}
    else if (left_columns == 0){printf("< %s\n", ptr->line);}
    else {printf("< \n");} 
    ptr = ptr->next;
  }
  return ;
}

