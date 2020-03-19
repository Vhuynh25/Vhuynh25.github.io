#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// problem 1 // note: main is at the bottom
void count() {
  printf("Enter lines of text here: ");
  int c, nc = 0, nt = 0, nbackslash = 0, nl = 0;
  while ((c = fgetc(stdin)) != EOF) {
    if (c == '\n') { ++nl;
    } else if (c == '\t') { ++nt;
    } else if (c == '\\') { ++nbackslash;
    } else if (isalnum(c) != 0) { ++nc; }
  } // <- Wow really?
  printf("\nlines: %d\n", nl);
  printf("tabs: %d\n", nt);
  printf("backslashes: %d\n", nbackslash);
  printf("alphanumeric: %d\n", nc);
}

// problems for string.h
// 1) const char* strchr(const char* s, const char c);
// 2) int strspn(const char* s, const char* t);
// 3) exit();
// 4) fopen(FILE* fin, "r");

// string.h section
// size_t strcpy... what?

char* strcpy_(char* s, const char* t){ // assuming s's size >= t's size
  char* ptr = s;
  while ((*s++=*t++) != '\0'){}
  return ptr;
}

char* strncat_(char* s, const char* t, int n){ 
  char* ptr = s;
  while(*s != '\0'){s++;}
  while(n-- > 0 && *t != '\0'){
    *s++ = *t++;
  }
  return ptr;
}

int strcmp_(const char* s, const char* t){
  while(*s != '\0' && *t != '\0'){
    if (*s != *t){return *s - *t;}
    s++;
    t++;
  }
  return *s - *t;;
}
// String testing section
void test_str_funcs(){
  char str1[100] = "Cat";
  char str2[100] = "DOG";
  printf("string 1: %s \t string 2: %s\n", str1, str2);
  char* cpyptr = strcpy_(str1,str2);
  printf("strcmp: %s\n", cpyptr);
  char* catptr = strncat_(str1,str2,2);
  printf("strncat(n==2): %s\n", catptr);
  printf("strcmp: %d\n", strcmp_(str1,str2));
}

// Point
typedef struct point point;
struct point { double x, y; };
point point_init(double x, double y){
  point* ptr = (point*) malloc(sizeof(point));
  ptr->x = x;
  ptr->y = y;
  return *ptr;
}

//------ SQUARE -----------------------------------------------------------------
typedef struct square square;
struct square { point ul; double side; };
square* square_init(double ulx, double uly, double side){
  square* ptr = (square*) malloc(sizeof(square));
  ptr->ul = point_init(ulx,uly);
  ptr->side = side;
}

void square_delete(square* sq){
  free(sq);
}

void square_move(square* sq, double x, double y){
  sq->ul.x = x;
  sq->ul.y = y;
}

void square_expandby(square* sq, double expandby){
  sq->side *= expandby; 
}

double square_area(square* sq){
  return sq->side*sq->side;
}

double square_perimeter(square* r){
  return 4*r->side;
}

// print location, side, area and perimeter
void square_print(const char* msg, square* sq){
  printf("%s\n", msg);
  printf("point :(%.2f,%.2f),\n",sq->ul.x, sq->ul.y);
  printf("side: %.2f,\n area: %.2f,\n perimeter: %.2f \n", sq->side, square_area(sq), square_perimeter(sq));
}
void test_square(double ulx, double uly, double side) {
  square* sq = square_init(ulx, uly, side);
  square_print("sq is: ", sq);
  square_move(sq, 2, 2);
  square_print("sq is now: ", sq);
  square_expandby(sq, 10);
  square_print("sq has expanded to: ", sq);
  square_delete(sq);
  printf("\n\n");
}
void tests_square() {
  test_square(0, 0, 10);
  test_square(1, 1, 5);
  // add other tests if you wish
}


int main(int argc, const char* argv[]){
  count();
  putchar('\n');
  test_str_funcs();
  putchar('\n');
  tests_square();
  return 0;
}
