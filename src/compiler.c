#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h> 
#include "include.h"
// x86_64 linux only
// Also i know this is more of a transpiler than a compiler but whatever
extern array_data_t data;
void new_asm_instruction(function_t* function, asm_instructions_t inst, char** argv, int argc) {
  char** args;
  asm_instruction_t* new_inst = (asm_instruction_t*)initialize_struct(ASM_INSTRUCTION_T);
  args = malloc(argc*sizeof(char*)); 
  for (int i = 0; i < argc; i++) {
    *(args+i) = malloc(strlen(*(argv+i)+1)*sizeof(char));
    strcpy(*(args+i), *(argv+i));
  }
  new_inst->instruction = inst; 
  new_inst->args = args;
  new_inst->previous = function->list;
}

int compiler_main(syntax_tree_t* tree, char* output) {
  asm_file_t* asm_file = (asm_file_t*)initialize_struct(ASM_FILE_T);
  tree_reset_index(tree);
  return 0;
}
