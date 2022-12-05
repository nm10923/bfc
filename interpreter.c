#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "include.h"

extern uc_array_data_t heap;
extern array_data_t stack;

values_t* interpreter_init() {
  values_t* values = (values_t*)initialize_struct(VALUES_T);
  values->tp = -1; values->loop_priority = 0; values->endl_priority = 0;
  memset(values->loop_stack, 0, 256*sizeof(int));
  return values;
}

int interpreter_loop(syntax_tree_t* tree) {
  values_t* values = interpreter_init();
  int running = I_PROGRAM_RUNNING; int return_val = 0;
  do {
    running = interpreter(tree, values);
    return_val = handle_errors(values, running);
  } while (running == I_PROGRAM_RUNNING);
  free(values);
  return return_val; 
}

int interpreter(syntax_tree_t* tree, values_t* values) {
  tree_reset_index(tree);
  switch(tree->nodes->instruction->type) {
    case MOVL :
      #ifdef DEBUG
        printf("MOVL, %d\n", values->tp);
      #endif
      for (int i = 0; i < tree->nodes->instruction->amount; i++) {
        heap.array--;
        heap.index--;
      }
      tree_iterate_node(tree, R_NEXT_INSTRUCTION);
      values->tp++;
      return I_PROGRAM_RUNNING;
    case MOVR :
      #ifdef DEBUG
        printf("MOVR, %d\n", values->tp);
      #endif
      for (int i = 0; i < tree->nodes->instruction->amount; i++) {
        heap.array++;
        heap.index++;
      }
      tree_iterate_node(tree, R_NEXT_INSTRUCTION);
      values->tp++;
      return I_PROGRAM_RUNNING;
      break;
    case ADD :
      #ifdef DEBUG
        printf("ADD, %d\n", values->tp);
      #endif
      for (int i = 0; i < tree->nodes->instruction->amount; i++) {
        ++*heap.array;
      }
      tree_iterate_node(tree, R_NEXT_INSTRUCTION);
      values->tp++;
      return I_PROGRAM_RUNNING;
      break; 
    case SUB :
      #ifdef DEBUG
        printf("SUB, %d\n", values->tp);
      #endif
      for (int i = 0; i < tree->nodes->instruction->amount; i++) {
        --*heap.array;
      }
      tree_iterate_node(tree, R_NEXT_INSTRUCTION);
      values->tp++;
      return I_PROGRAM_RUNNING;
      break;
    case PUTC :
      ;
      #ifdef DEBUG
        printf("PUTC, %d\n", values->tp);
      #endif
      int print_check = 0;
      for (int i = 0; i < tree->nodes->instruction->amount; i++) {
        print_check = printf("%c", *heap.array);
        if (print_check < 0) {
          return I_PUTC_FAIL;
        }
      }
      tree_iterate_node(tree, R_NEXT_INSTRUCTION);
      values->tp++;
      return I_PROGRAM_RUNNING;
      break;
    case GETC :
      ;
      #ifdef DEBUG
        printf("GETC, %d\n", values->tp);
      #endif
      // TODO figure out how scanf return works
      int scan_check = 0;
      for (int i = 0; i < tree->nodes->instruction->amount; i++) {
        scanf("%c", heap.array);
        tree_iterate_node(tree, R_NEXT_INSTRUCTION);
        values->tp++;
      }
      return I_PROGRAM_RUNNING;
      break;
    case LOOP :
      #ifdef DEBUG
        printf("LOOP, %d\n", values->tp);
      #endif
      for (int i = 0; i < tree->nodes->instruction->amount; i++) {
        values->loop_priority++;
        values->loop_stack[values->loop_priority] = (values->tp)+1;
        tree_iterate_node(tree, R_NEXT_INSTRUCTION);
        values->tp++;
      }
      return I_PROGRAM_RUNNING;
      break;
    case ENDL :
      if (*heap.array == 0) {
        values->loop_stack[values->loop_priority] = 0;
        --values->loop_priority;
        values->tp++;
        #ifdef DEBUG
          printf("ENDL, %d\n", values->tp);
        #endif
	return I_PROGRAM_RUNNING;
        break;
      }
      else {
	tree_iterate_node(tree, R_LOOP);
        values->tp = values->loop_stack[values->loop_priority];
        #ifdef DEBUG
          printf("ENDL, %d\n", values->tp);
        #endif
      }
      return I_PROGRAM_RUNNING;
      break;
    case NIL : return I_PROGRAM_DONE;
      break;
  }
  return 0;
}

int debugger(syntax_tree_t* tree) {
  // COMMAND TABLE
  // cells [sn] [en] - prints out content of cells from [sn] to [en]
  // run - runs program
  // break - sets break point
  // disable - disables break point
  // step - executes next instruction
  // list - lists instructions
  // help - prints help
  values_t* values = interpreter_init();
  int bp = 0;
  char* check; char input[256]; const char* dbg = "[DEBUG]";
  bool get_input = true; bool parse_input = false;
 
  getinput :
  printf("%s: ", dbg);
  check = fgets(input, 256, stdin);
  if (check != NULL) {;
    // parse input into command form
    int r = 256; int c = 1; // rows number is arbitrary
    for (int i = 0; i < strlen(input); i++) { 
      c += (input[i] == ' ');
    }
    char** args = malloc(c*sizeof(char*));
    for (int i = 0; i < c; i++) { 
      *(args+i) = malloc(r*sizeof(char));
    }
    int i = 0; int j = 0; int k = 0;
    do {
      if (input[i] == ' ') {
        k = 0;
        j++;
      }
      if (input[i] == '\n') { 
        *(*(args+j)+k) = '\0';
	break;
      }
      *(*(args+j)+k) = input[i];
      i++; k++;
    } while (1);

    // giant else if tree cuz i can't use switches
    do {
      int l = 0;
      if (!strcmp(*(args+l), "cells")) {
        l++; int sn = atoi(args[l]);
        l++; int en = atoi(args[l]);
        printf("%s Cells %d-%d:\n", dbg, sn, en);
        for (int m = sn; m <= en; m++) {
          printf("%u, ", *((heap.array-heap.index)+m));
        }
        printf("\n");
        l = 0;
	break;
      }
      else if (!strcmp(*(args+l), "run")) {
	values = interpreter_init();
	int running = I_PROGRAM_RUNNING;
	do {
          running = interpreter(tree, values);
	  if (running != I_PROGRAM_RUNNING) {
	    if (running == I_PROGRAM_DONE) {
              printf("%s Program successfully executed\n", dbg);
	      break;
	    }
	    else {
              handle_interpreter_errors(values, running);
	    }
            break;
	  } 
	  if ((values->tp == bp) & (bp != 0)) {
            printf("%s Breakpoint at %d reached\n", dbg, bp);
	    break;
	  }
	} while (1);
	l = 0;
	break;
      }
      else if (!strcmp(*(args+l), "list")) {
	int n = 0;
	do {
          switch (*(tree->tokens+n)) {
            case MOVR :
              printf(">");
	      break;
            case MOVL :
              printf("<");
	      break;
            case ADD :
              printf("+");
	      break;
            case SUB :
              printf("-");
	      break;
            case PUTC :
              printf(".");
	      break;
            case GETC :
              printf(",");
	      break;
            case LOOP :
              printf("[");
	      break;
            case ENDL :
              printf("]");
	      break;
	    case NIL :
	      break;
	  }
	  if (*(tree->tokens+n) == NIL) {
	    printf("\n");
            break;
	  }
	  n++;
	} while (1);
	l = 0;
	break;
      }
      else if (!strcmp(*(args+l), "break")) { 
	bp = values->tp;
	printf("%s Break point at instruction %d\n", dbg, values->tp);
	l = 0;
	break;
      }
      else if (!strcmp(*(args+l), "step")) {
	int running = I_PROGRAM_RUNNING;
        running = interpreter(tree, values);
	if (running != I_PROGRAM_RUNNING) {
	  if (running == I_PROGRAM_DONE) {
	    printf("%s No more instructions\n", dbg);
	  }
	  else {
            handle_interpreter_errors(values, running);
	  }
	} 
	l = 0;
	break;
      }
      else if (!strcmp(*(args+l), "clear")) {
        printf("\e[1;1H\e[2J");	
	l = 0;
	break;
      }
      else if (!strcmp(*(args+l), "reset")) {
        free(values);
        values = interpreter_init();
	memset((heap.array-heap.index), 0, heap.size*sizeof(char));
	l = 0;
	break;
      }
      else if (!strcmp(*(args+l), "help")) {
        printf("Commands:\n\tcells [sn] [en]: Prints value of cells from sn to en (note the first cells index is zero)\n\trun: Runs program\n\tbreak: Sets the break point\n\tdisable: Disables the break point\n\tstep: Executes the next instruction\n\tlist: Lists the instructions\n\tclear: Clears the screen\n\treset: Resets the machine's state\n");
	break;
      }
      else if (!strcmp(*(args+l), "quit")) {
        exit(0);
      }
      else if (!strcmp(*(args+l), "\0")) {
        break;
      }
      else {
        fprintf(stderr, "%s is not a valid instruction\n", args[l]);
	break;
      }
    } while (1);
    goto getinput;
  }
  else {
    fprintf(stderr, "Issue with attaining user input");
    exit(1);
  }
}
