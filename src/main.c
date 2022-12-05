#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include "include.h"

uc_array_data_t heap;
array_data_t stack;

void initialize_memory(int heap_size, int stack_size, bool no_actual_memory) {
  // initialize data array
  unsigned char zero = 0;
  if (heap_size != 0) {
    heap.size = stack_size;
    heap.index = 0;
  }
  else { 
    heap.size = HEAP_SIZE;
    heap.index = 0;
  }

  if (stack_size != 0) {
    stack.size = stack_size;
    stack.index = 0;
  }
  else { 
    stack.size = HEAP_SIZE;
    stack.index = 0;
  }

  if (no_actual_memory) {
    heap.array = NULL; 
    stack.array = NULL; 
  }
  else {
    heap.array = (uint8_t*)malloc(heap.size*sizeof(char));
    memset(heap.array, zero, heap.size*sizeof(char));
    stack.array = (int*)malloc(stack_size*sizeof(char));
    memset(stack.array, 0, stack_size*sizeof(int));
  }
}

char* file_to_char(FILE* file) {
  if (file == NULL) {
    return NULL;
  }
  fpos_t position;
  fgetpos(file, &position);
  int size = 0;
  char c;
  do {
    c = fgetc(file);
    switch(c) {
      case '<' :
      case '>' :
      case '+' :
      case '-' :
      case '.' :
      case ',' :
      case '[' :
      case ']' :
	size++;
	break;
      default :
	break;
    }
  } while (!feof(file));
  size++; 
  char* file_stream = (char*)malloc(size*sizeof(char));
  int i = 0;
  fsetpos(file, &position);
  do {
    c = fgetc(file);
    switch(c) {
      case '<' :
      case '>' :
      case '+' :
      case '-' :
      case '.' :
      case ',' :
      case '[' :
      case ']' :
	*(file_stream+i) = c;
	i++;
	break;
      default :
	break;
    }
  } while (!feof(file)); 
  *(file_stream + size) = '\0';
  return file_stream;
}

int main(int argc, char* argv[]) {
  FILE* file;
  int heap_size, stack_size, return_val = 0;
  char* output_name = "a.out"; char* input;
  bool cflag, dflag = false;

  // option handling
  int opt;
  while((opt = getopt(argc, argv, "hcdf:s:o:t:")) != -1) {
    switch(opt) {
      case 'h' :
        fprintf(stderr, " ____  _____ ____ \n| __ )|  ___/ ___|\n|  _ \\| |_ | |    \n| |_) |  _|| |___ \n|____/|_|   \\____|\n\nBrainfuck Compiler and Interpreter\nTags:\n\t-h: Prints this message\n\t-c: Turns on compiler mode\n\t-d: Turns on debugger mode\n\t-s [size]: Allocates size of memory using [size]\n\t-t [size]: Allocates size of stack using [size]\n\t-f [filename]: Compiles/interprets the file given\n\t-o [output]: (compiler mode only) Uses [output] as the file name for binary\n"); 
	exit(0);
      case 's' :
        if (atoi(optarg) != 0) {
          heap_size = atoi(optarg);
        }
        break;
      case 't' :
        if (atoi(optarg) != 0) {
          stack_size = atoi(optarg);
	}
	break;
      case 'f' :
        file = fopen(optarg, "r");
	break;
      case 'c' :
	cflag = true;
	break;
      case 'o' :
        output_name = optarg;
	break;
      case 'd' :
	dflag = true;
	break;
      deafult:
        fprintf(stderr, "Usage: %s [-s size], [-h] help\n", argv[0]);
        exit(0);
    }
  }

  // get input from file, lex it, then parse it into a syntax tree
  if (file != NULL) {
    input = file_to_char(file);
  }
  tokens_t* tokens = lexer(input);
  syntax_tree_t* tree = parser(tokens);
  // check flags
  if (cflag) {
    initialize_memory(heap_size, stack_size, true);
    return_val = compiler_main(tree, output_name);
  } 
  else if (dflag) {
    initialize_memory(heap_size, stack_size, false);
    return_val = debugger(tree);
  }
  else {
    initialize_memory(heap_size, stack_size, false);
    return_val = interpreter_loop(tree);
  }
  // free everything 
  tree_free(tree);
  free(tokens);
  free(heap.array);
  free(stack.array);

  // return (hopefully 0)
  return return_val;
}
