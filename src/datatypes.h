#include <stdint.h>
#include <stdbool.h>
typedef enum tokens {
  MOVR,
  MOVL,
  ADD,
  SUB, PUTC,
  GETC,
  LOOP,
  ENDL,
  NIL
} tokens_t;

enum datatypes {
  ASM_FILE_T,
  SECTION_T,
  FUNCTION_T,
  ASM_INSTRUCTION_T,
  SYNTAX_TREE_T,
  TREE_NODE_T,
  INSTRUCTION_T,
  ELEMENT_T,
  LOOKUP_ARRAY_T,
  VALUES_T
};

enum error_codes {
  // lexer codes
  L_ILLEGAL_POINTER_MOVE,
  L_ILLEGAL_ENDL,
  // interprter codes
  I_PROGRAM_RUNNING,
  I_PROGRAM_DONE,
  I_PUTC_FAIL,
  I_GETC_FAIL
  // compiler codes
}; 

typedef struct uc_array_data {
  uint8_t* array;
  int size;
  int index;
} uc_array_data_t;

typedef struct array_data {
  int* array;
  int size;
  int index;
} array_data_t;

typedef struct element {
  struct element* previous;
  int id;
  void* return_val;
  struct element* next;
} element_t;

typedef struct lookup_array {
  element_t* elements;
  int size;
  int index;
} lookup_array_t;

// parser 
enum tree_standings {
  S_MAIN_STATEMENT,
  S_INSTRUCTION,
  S_LOOP,
  S_END_OF_LOOP,
  S_END_OF_PROGRAM
};

enum tree_relationships {
  R_END_OF_PROGRAM,
  R_NEXT_INSTRUCTION,
  R_LOOP,
  R_END_OF_LOOP
};

typedef struct instruction {
  tokens_t type;
  int amount;
} instruction_t;

typedef struct tree_node {
  struct tree_node** previous_nodes;
  int standing;
  instruction_t* instruction;
  lookup_array_t* relationships; // relationships between the nodes
  int amount; // amount of next_nodes
  int nodes_index;
  struct tree_node** next_nodes;
} tree_node_t;

typedef struct syntax_tree {
  tree_node_t* nodes;
  int nodes_size;
  int nodes_index;
  tree_node_t* loop_stack[256];
  int loop_index;
  tokens_t* tokens;
} syntax_tree_t;

// interpreter
typedef struct values {
  int tp;
  int loop_priority;
  int endl_priority;
  int loop_stack[256];
} values_t;

// compiler 
// doubly linked list for instructions
typedef enum asm_instructions {
  ASM_BRK,
  ASM_ADD,
  ASM_SUB
} asm_instructions_t;

typedef struct asm_instruction {
  struct asm_instruction* previous;
  asm_instructions_t instruction;
  char** args;
  struct asm_instruction* next;
} asm_instruction_t;

typedef struct function {
  char* name;
  int size;
  int index;
  bool is_global;
  asm_instruction_t* list;
} function_t;

typedef struct section {
  char* name;
  function_t* functions;
  int size;
  int index;
} section_t;

typedef struct asm_file {
  section_t* header;
  section_t* data;
  section_t* text;
  section_t* rodata;
} asm_file_t;
