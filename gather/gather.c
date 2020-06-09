#include "gather.h"

int main(int argc, const char* argv[]){
  if (argc < 3){
    fprintf(stderr, "Usage: ./program regularexpression directory/file\n");
  }
  dirsearch(argv[2],argv[1]);
}

void dirsearch(const char* s,const char* pattern){
  struct dirent *de;
  DIR *dptr = opendir(s);
  DIR *sub = NULL;

  if (dptr == NULL){
    return;
  }
  
  regex_t regex;
  int regex_value;
  char* buf = NULL;
  regex_value = regcomp(&regex, pattern, 0);
  if (regex_value){
    return;
  }

  while ((de = readdir(dptr)) != NULL){
    dirsearch(de->d_name,pattern);
    regex_value = regexec(&regex, de->d_name,0,NULL,0);
    
    if (!regex_value){
      char fullpath[500];
      realpath(s,fullpath);
      strcat(fullpath,"/");
      strcat(fullpath,de->d_name);
      gather(fullpath); }
  }
}
//realpath(filename,fullpath);
void gather(char* filename){
  FILE *fin;

  printf("\n%s\n",filename);
  fin = fopen(filename,"r");
  if (fin == NULL){
    return;
  }
  printf("\n%s\n",filename);
  char outname[50] = "";
  strcat(outname,basename(filename));
  
  FILE *fout;
  fout = fopen(outname,"w");
  
  if (fout == NULL){
    return;
  }

  char c;
  while ((c = fgetc(fin)) != EOF){
    fputc(c,fout);
  }
    
}
