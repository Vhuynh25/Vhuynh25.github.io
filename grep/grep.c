/*
 * Editor
 */

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "grep.h"

#define	LBSIZE	10000
#define	ESIZE	256
#define	GBSIZE	256
#define	NBRA	5

#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12
#define	CBACK	14
#define	CCIRC	15 // for '^'
#define	STAR	01

int	peekc;
int	lastc;
char	linebuf[LBSIZE]; // for file lines
char	expbuf[ESIZE+4]; // for regex 
int	vflag	= 1;
char	*loc1;
char	*loc2;
char	*braslist[NBRA];
char	*braelist[NBRA];
int	nbra;

int fcount; // tracking file num
char filename[1000];

int main(int argc, char **argv) {
  char *p1=filename;
  argv++;
  fcount = argc - 2; 
  compile(*argv);
  
  while (--argc > fcount){
    argv++;
    strcpy(p1, *argv);
    execute(*argv);
  }
  return 0;
}

void quit(int n) {
  if(n == 1){
    fprintf(stderr, "No match found");
    exit(1);
  }
  else if (n == 2){
    fprintf(stderr, "Input or program failure");
    error(2);
  }
  exit(0);
}

void compile(char* argv) {
	int c;
	char pat[100] = "";
	char *ep, *patptr = pat;
	char *lastep;
	char bracket[NBRA], *bracketp;
	int cclcnt;
	strcat(pat,argv);
	strcat(pat,"\0");

	ep = expbuf;
	bracketp = bracket;
	nbra = 0;
	
	if ((c = *patptr)=='^') {
	  patptr++;
	  *ep++ = CCIRC;
	}
	peekc = c;
	lastep = 0;
	for (;;) {
	  /* if (ep >= &expbuf[ESIZE]) */
	  /*   quit(2); */
	  c = *patptr++;
	  if (c == '\n') {
	    peekc = c;
	    c = EOF;
	  }
	  if (c==EOF || c == '\0') {
	    if (bracketp != bracket){
	      quit(2);}
	    *ep++ = CEOF;
	    return;
	  }
	  if (c!='*')
	    lastep = ep;
	  switch (c) {
	    
	  case '\\':
	    if ((c = *patptr++)=='(') {
	      if (nbra >= NBRA)
		quit(2);
	      *bracketp++ = nbra;
	      *ep++ = CBRA;
	      *ep++ = nbra++;
	      continue;
	    }
	    if (c == ')') {
	      if (bracketp <= bracket)
		quit(2);
	      *ep++ = CKET;
	      *ep++ = *--bracketp;
	      continue;
	    }
	    if (c>='1' && c<'1'+NBRA) {
	      *ep++ = CBACK;
	      *ep++ = c-'1';
	      continue;
	    }
	    *ep++ = CCHR;
	    if (c=='\n')
	      quit(2);
	    *ep++ = c;
	    continue;
	    
	  case '.':
	    *ep++ = CDOT;
	    continue;
	    
	  case '\n':
	    quit(2);
	    
	  case '*':
	    if (lastep==0 || *lastep==CBRA || *lastep==CKET){
	      *ep++ = CCHR;
	      *ep++ = c;}
	    *lastep |= STAR;
	    continue;
	    
	  case '$':
	    if ((peekc=*(patptr+1)) != EOF && peekc!='\n' && peekc != '\0'){
	      *ep++ = CCHR;
	      *ep++ = c;}
	    
	    *ep++ = CDOL;
	    continue;
	    
	  case '[':
	    *ep++ = CCL;
	    *ep++ = 0;
	    cclcnt = 1;
	    if ((c=*patptr++) == '^') {
	      c = *patptr++;
	      ep[-2] = NCCL;
	    }
	    do {
	      if (c=='\n')
		quit(2);
	      if (c=='-' && ep[-1]!=0) {
		if ((c=*patptr++)==']') {
		  *ep++ = '-';
		  cclcnt++;
		  break;
		}
		while (ep[-1]<c) {
		  *ep = ep[-1]+1;
		  ep++;
		  cclcnt++;
		  if (ep>=&expbuf[ESIZE])
		    quit(2);
		}
	      }
	      *ep++ = c;
	      cclcnt++;
	      if (ep >= &expbuf[ESIZE])
		quit(2);
	    } while ((c = *patptr++) != ']');
	    lastep[1] = cclcnt;
	    continue;
	  default:
	    *ep++ = CCHR;
	    *ep++ = c;
	  }
	}
}

int execute(char* file) {
	char *p1, *p2;
	int c, result;
	FILE* fin;

	if (file && (fin = fopen(file, "r")) == NULL){quit(2);}

	while (fgets(linebuf,LBSIZE,fin)){
	  p1 = linebuf;
	  p2 = expbuf;
	  result = 0;
	  if (*p2==CCIRC) {
	    loc1 = p1;
	    if ((result = advance(p1, p2+1)) == 1) {
	      loc1 = p1;
	      printmatch();
	    }
	    continue;
	  }
	  /* regular algorithm */
	  do {
	    if ((result = advance(p1, p2)) == 1) {
	      loc1 = p1;
	      printmatch();
	    }
	  } while (*p1++ && result != 1);
	}
	
	fclose(fin);
	return(0);
	
}

int advance(char *lp, char *ep) {
	char *curlp;
	char *lpstart = lp;
	int i;

	for (;;) switch (*ep++) {

	  case CCHR: // if upcoming *ep is char
	    if (*ep++ == *lp++) {
	      // if *ep == *lp, keep going until *ep != *lp
	      // or *ep == CEOF
	      continue;
	    }
	    return(0);

	  case CDOT:
	    if (*lp++)
	      continue;
	    return(0);
	    
	  case CDOL:
	    if (lpstart != lp){continue;}
	    if (*lp==0)
	      continue;
	    return(0);

	  case CEOF: // assuming *ep == *lp for the entire length of ep
	    loc2 = lp;
	    return(1); // return 1 to print the linebuf
	    
	  case CCL:
	    if (cclass(ep, *lp++, 1)) {
	      ep += *ep;
	      continue;
	    }
	    return(0);

	  case NCCL:
	    if (cclass(ep, *lp++, 0)) {
	      ep += *ep;
	      continue;
	    }
	    return(0);

	  case CBRA:
	    braslist[*ep++] = lp;
	    continue;
	    
	  case CKET:
	    braelist[*ep++] = lp;
	    continue;
	    
	  case CBACK:
	    if (braelist[i = *ep++]==0)
	      error(2);
	    if (backref(i, lp)) {
	      lp += braelist[i] - braslist[i];
	      continue;
	    }
	    return(0);
	    
	  case CBACK|STAR:
	    if (braelist[i = *ep++] == 0)
	      error(2);
	    curlp = lp;
	    while (backref(i, lp))
	      lp += braelist[i] - braslist[i];
	    while (lp >= curlp) {
	      if (advance(lp, ep))
		return(1);
	      lp -= braelist[i] - braslist[i];
	    }
	    continue;
	    
	  case CDOT|STAR:
	    curlp = lp;
	    while (*lp++)
	      ;
	    goto star;
	    
	  case CCHR|STAR:
	    curlp = lp;
	    while (*lp++ == *ep)
	      ;
	    ep++;
	    goto star;
	    
	  case CCL|STAR:
	  case NCCL|STAR:
	    curlp = lp;
	    while (cclass(ep, *lp++, ep[-1]==(CCL|STAR)))
	      ;
	    ep += *ep;
	    goto star;

	  star:
	    do {
	      lp--;
	      if (advance(lp, ep))
		return(1);
	    } while (lp > curlp);
	    return(0);
	    
	  default:
	    printf("%s\n", ep);
	    quit(2);
	  }
}

int backref(int i, char *lp) {
	char *bp;

	bp = braslist[i];
	while (*bp++ == *lp++)
		if (bp >= braelist[i])
			return(1);
	return(0);
}

int cclass(char *set, int c, int af) {
	int n;

	if (c==0)
		return(0);
	n = *set++;
	while (--n)
		if (*set++ == c)
			return(af);
	return(!af);
}

void printmatch(){
  if (fcount >= 1)
    printf("%s: ", filename);
  printf("%s\n", linebuf);
}
