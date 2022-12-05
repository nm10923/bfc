#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "include.h"
void tree_new_node(syntax_tree_t* tree, int standing, int type) {
  int outcomes = 1;
  tree_node_t* node = (tree_node_t*)initialize_struct(TREE_NODE_T);
  node->previous_nodes = (tree_node_t**)malloc(sizeof(tree_node_t*));
  node->instruction = (instruction_t*)initialize_struct(INSTRUCTION_T);
  node->standing = standing; node->instruction->type = type; node->instruction->amount = outcomes;
  
  if (tree->nodes->instruction->type == node->instruction->type) {
    free(node->instruction);
    free(node);
    tree->nodes->instruction->amount++;
    return;
  }

  switch(standing) {
    case S_INSTRUCTION :
      node->next_nodes = (tree_node_t**)malloc(sizeof(tree_node_t*)*outcomes);
      node->relationships = (lookup_array_t*)initialize_struct(LOOKUP_ARRAY_T);
      lua_new_element(node->relationships, R_NEXT_INSTRUCTION, NULL);
      *(node->next_nodes) = (tree_node_t*)NULL;
      node->amount = outcomes; node->nodes_index = 0; tree->nodes_size++; tree->nodes_index++;
      break;
    case S_LOOP :
      node->next_nodes = (tree_node_t**)malloc(sizeof(tree_node_t*)*outcomes);
      node->relationships = (lookup_array_t*)initialize_struct(LOOKUP_ARRAY_T);
      lua_new_element(node->relationships, R_NEXT_INSTRUCTION, NULL);
      *(node->next_nodes) = (tree_node_t*)NULL;
      node->amount = outcomes; node->nodes_index = 0;
      tree->nodes_size++; tree->nodes_index++; tree->loop_index++;
      tree->loop_stack[tree->loop_index] = tree->nodes;
      break;
    case S_END_OF_LOOP :
      outcomes++;
      node->next_nodes = (tree_node_t**)malloc(sizeof(tree_node_t*)*outcomes); 
      node->relationships = (lookup_array_t*)initialize_struct(LOOKUP_ARRAY_T);
      lua_new_element(node->relationships, R_LOOP, &tree->loop_stack[tree->loop_index]);
      lua_new_element(node->relationships, R_END_OF_LOOP, NULL);
      node->amount = outcomes; node->nodes_index = 0;
      tree->nodes_size++; tree->nodes_index++;
      break;
    case S_END_OF_PROGRAM :
      node->next_nodes = (tree_node_t**)NULL;
      node->relationships = (lookup_array_t*)NULL;
      node->amount = 0;
      node->nodes_index = 0;
      tree->nodes_size++; tree->nodes_index++;
      break;
    default :
      free(node);
      return;
  }
  
  switch(tree->nodes->standing) {
    case S_MAIN_STATEMENT :
    case S_INSTRUCTION :
    case S_LOOP :
    case S_END_OF_PROGRAM :
      *(tree->nodes->next_nodes) = node;
      break;
    case S_END_OF_LOOP :
      *(tree->nodes->next_nodes+1) = node;
      break;
  }
  *(node->previous_nodes) = tree->nodes;
  tree->nodes = node;
}

void tree_iterate_node(syntax_tree_t* tree, int relationship) {
  tree->nodes = *(tree->nodes->next_nodes+tree->nodes->nodes_index);
  if (tree->nodes->amount > 1) {
    tree->nodes->nodes_index++;
  }
  tree->nodes_index++;
}

void tree_deiterate_node(syntax_tree_t* tree) {
  tree->nodes = *(tree->nodes->previous_nodes);
  tree->nodes_index--;
}

void tree_reset_index(syntax_tree_t* tree) {
  for (int i = 0; i < tree->nodes_size; i++) {
    tree_deiterate_node(tree);
  } 
  tree->nodes_index = 0;
}

void tree_free(syntax_tree_t* tree) {
  for (int i = 0; i < tree->nodes_size; i++) {
    tree_reset_index(tree);
    free(tree->nodes);
    free(tree->nodes->relationships);
  }
  free(tree);
}

syntax_tree_t* parser(tokens_t* tokens) {
  syntax_tree_t* tree = (syntax_tree_t*)initialize_struct(SYNTAX_TREE_T);
  tree->tokens = tokens;
  int n = 0;
  do { 
    switch(*(tokens+n)) {
      case MOVL :
      case MOVR :
      case ADD :
      case SUB :
      case PUTC :
      case GETC :
	tree_new_node(tree, S_INSTRUCTION, *(tokens+n));
        break;
      case LOOP :
	tree_new_node(tree, S_LOOP, *(tokens+n));
	break;
      case ENDL :
	tree_new_node(tree, S_END_OF_LOOP, *(tokens+n));
	break;
      case NIL :
	break;
    }
    if (*(tokens+n) == NIL) {
      tree_new_node(tree, S_END_OF_PROGRAM, *(tokens+n));
      break;
    }
    n++;
  } while (1);
  return tree;
}
