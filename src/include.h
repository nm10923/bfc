#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "datatypes.h"

// #define DEBUG
#define STACK_SIZE 256
#define HEAP_SIZE 30000
// global functions
int handle_errors(values_t* values, int code);
void* initialize_struct(int type);
void lua_iterate_element(lookup_array_t* array);
void lua_new_element(lookup_array_t* array, int id, void* return_val);
void* lua_find_by_id(lookup_array_t* array, int id);

// lexer functions
int handle_lexer_errors(values_t* values, int code);
tokens_t* lexer(char* string);

// parser functions
void tree_new_node(syntax_tree_t* tree, int standing, int type);
void tree_iterate_node(syntax_tree_t* tree, int relationship);
void tree_deiterate_node(syntax_tree_t* tree);
void tree_reset_index(syntax_tree_t* tree);
void tree_free(syntax_tree_t* tree);
syntax_tree_t* parser(tokens_t* tokens);

// interpreter/debugger functions
values_t* interpreter_init();
int interpreter_loop(syntax_tree_t* tree);
int handle_interpreter_errors(values_t* values, int code);
int interpreter(syntax_tree_t* tree, values_t* values);
int debugger(syntax_tree_t* tree);

// compiler functions
void new_asm_instruction(function_t* function, asm_instructions_t inst, char** argv, int argc);
int compiler_main(syntax_tree_t* tree, char* output);
