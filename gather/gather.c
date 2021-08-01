#include "gather.h"
int delete_original = 0;
int no_subs = 0; // no checking sub_directories

int main(int argc, const char* argv[]){
  if (argc < 3){
    fprintf(stderr, "Usage: ./program -options regularexpression directory/file\n");
    return 0;
  }
  while (options_check(argv[1]) == 1){argv++;}
  
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
    if (no_subs == 0) dirsearch(de->d_name,pattern);
    regex_value = regexec(&regex, de->d_name,0,NULL,0);
    
    if (!regex_value){
      char fullpath[500];
      realpath(s,fullpath);
      strcat(fullpath,"/");
      strcat(fullpath,de->d_name);
      gather(fullpath);

      if (delete_original == 1) {remove(fullpath);} // delete original
    }
  }
}

void gather(char* filename){
  FILE *fin;

  fin = fopen(filename,"r");
  if (fin == NULL){
    return;
  }

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

int options_check(const char* opt){
  if (strcmp(opt,"-d") == 0){
    printf("\n Delete original enabled.\n");
    delete_original = 1;
    return 1;
  }
  else if (strcmp(opt,"-s") == 0){
    printf("\n No subdirectories enabled.\n");
    no_subs = 1;
    return 1;
  }
  return 0;
}
