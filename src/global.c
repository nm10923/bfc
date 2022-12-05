#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h> 
#include "include.h"
extern uc_array_data_t heap;
extern array_data_t stack;

int handle_errors(values_t* values, int code) {
  const char* lexer = "[LEXER]";
  const char* interpreter = "[INTERPRETER]";
  int return_val = 0;
  switch(code) {
    case L_ILLEGAL_POINTER_MOVE :
      fprintf(stderr, "%s: Illegal pointer move at %d\n", lexer, values->tp); 
      return_val = 1;
      break;
    case L_ILLEGAL_ENDL :
      fprintf(stderr, "%s: Illegal loop closing at %d\n", lexer, values->tp); 
      return_val = 1;
      break;
    case I_PROGRAM_RUNNING :
    case I_PROGRAM_DONE :
      break;
    case I_PUTC_FAIL :
      fprintf(stderr, "%s: [.] instruction failure at %d\n", interpreter, values->tp);
      return_val = 1;
      break;
    case I_GETC_FAIL :
      fprintf(stderr, "%s: [,] instruction failure at %d\n", interpreter, values->tp);
      return_val = 1;
      break;
  }
  return return_val;
}

void* initialize_struct(int type) {
  switch(type) {
    case ASM_FILE_T : ;
      asm_file_t* asm_file = (asm_file_t*)malloc(sizeof(asm_file_t));
      return asm_file;
      break;
    case SECTION_T : ;
      section_t* sections = (section_t*)malloc(sizeof(section_t));
      return sections;
      break;
    case ASM_INSTRUCTION_T : ;
      asm_instruction_t* list = (asm_instruction_t*)malloc(sizeof(asm_instruction_t));
      return list;
    case FUNCTION_T : ;
      function_t* functions = (function_t*)malloc(sizeof(function_t));
      functions->list = (asm_instruction_t*)initalize_struct(ASM_INSTRUCTION_T) ;
      return functions;
      break;
    case SYNTAX_TREE_T : ;
      syntax_tree_t* tree = (syntax_tree_t*)malloc(sizeof(syntax_tree_t));
      // Tree variable initalizing
      tree->loop_index = -1;
      tree->nodes_size = 1;
      tree->nodes = (tree_node_t*)initialize_struct(TREE_NODE_T);
      // Tree node variable initalizing
      tree->nodes->standing = S_MAIN_STATEMENT;
      tree->nodes->instruction = (instruction_t*)initialize_struct(INSTRUCTION_T);
      tree->nodes->relationships = malloc(sizeof(int));
      tree->nodes->nodes_index = 0;
      memset(tree->nodes->relationships, R_NEXT_INSTRUCTION, sizeof(int));
      tree->nodes->next_nodes = (tree_node_t**)malloc(sizeof(tree_node_t));
      return tree;
    case TREE_NODE_T : ;
      tree_node_t* node = (tree_node_t*)malloc(sizeof(tree_node_t*));
      return node;
    case INSTRUCTION_T : ;
      instruction_t* instruction = (instruction_t*)malloc(sizeof(instruction_t));
      return instruction;
    case LOOKUP_ARRAY_T : ;
      lookup_array_t* table = (lookup_array_t*)malloc(sizeof(lookup_array_t));
      table->elements = (element_t*)initialize_struct(ELEMENT_T);
      return table;
    case ELEMENT_T : ;
      element_t* element = (element_t*)malloc(sizeof(element_t));
      element->previous = NULL; element->next = NULL;
    case VALUES_T : ;
      values_t* values = (values_t*)malloc(sizeof(values_t));
    default :
      return NULL;
  }
}

void lua_iterate_element(lookup_array_t* array) {
  if (array->elements != NULL) {
    array->elements = array->elements->next;
    array->index++;
  }
  else {
    return;
  }
}

void lua_reset_index(lookup_array_t* array) {
  for (int i = array->index; i > 0; i--) {
    array->elements--; array->index--;
  }
}

void lua_new_element(lookup_array_t* array, int id, void* return_val) {
  if (array->elements->previous == NULL) {
    array->elements->id = id; array->elements->return_val = return_val;
    array->elements->next = NULL;
  } 
  else {
    element_t* element = (element_t*)initialize_struct(ELEMENT_T);
    element->id = id; element->return_val = return_val;
    element->previous = array->elements;
    element->next = NULL;
    array->elements = element;
  }
}

void* lua_find_by_id(lookup_array_t* array, int id) {
  lua_reset_index(array);
  do {
    lua_iterate_element(array);
    array->index++;
    if (array->elements->id == id) {
      return array->elements->return_val;
    }
  } while (array->elements != NULL);
  return NULL;
}

void lua_edit_element(lookup_array_t* array, int id, void* return_val) {
  lua_find_by_id(array, id);
  array->elements->return_val = return_val;
}
