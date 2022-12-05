#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <stdint.h> 
#include <stdbool.h>
#include <string.h>
#include "include.h"

extern array_data_t data;

tokens_t* lexer(char* string) {
  int tp = 0;
  tokens_t* tokens = (tokens_t*)malloc((strlen(string)+1)*sizeof(tokens_t));
  int i = 0; 
  int loop_ctr, endl_ctr = 0;
  char c = *(string);
  while (1) {
    c = *(string + i);
    switch(c)  {
      case '<' :
	*(tokens+tp) = MOVL;
	tp++;
        break;
      case '>' :
	*(tokens+tp) = MOVR;
	tp++;
        break;
      case '+' :
	*(tokens+tp) = ADD;
	tp++;
	break;
      case '-' :
	*(tokens+tp) = SUB;
	tp++;
	break;
      case '.' :
	*(tokens+tp) = PUTC;
	tp++;
	break;
      case ',' :
	*(tokens+tp) = GETC;
	tp++;
	break;
      case '[' :
	*(tokens+tp) = LOOP;
	tp++; loop_ctr++;
	break;
      case ']' :
	*(tokens+tp) = ENDL;
	tp++; endl_ctr++;
	break;
    }
    if (c == '\0') {
      *(tokens+tp) = NIL;
      break;
    }
    i++;
  }
  return tokens;
}
