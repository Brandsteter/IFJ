/**
 *Project: Implementacia prekladača imperatívneho jazyka IFJ22
 *@file sym_table.c
 *@brief Symbol table implemented via Binary Search Tree 
 *@author Daniel Blaško <xblask05>
 *References: VUT FIT IAL 2022/2023 Study References
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "symtable.h"
#include "param_list.h"

void bst_init(bst_node_t **btree) {
    (*btree) = NULL;
}

bst_node_t *new_node_func(int return_type, paramList *param_list) {
    bst_node_t *new_node = (bst_node_t *) malloc(sizeof(bst_node_t));
    if (new_node == NULL) {
        exit(INTERNAL_ERR);
    }
    //Setting Node to function mode
    new_node->id = NULL;
    new_node->is_function = true;

    new_node->function_parameters.is_defined = true;
    new_node->function_parameters.return_type = return_type;
    new_node->function_parameters.param_list = param_list;

    new_node->lptr = NULL;
    new_node->rptr = NULL;
    return new_node;
}

bst_node_t *new_node_var(int var_type, char* name) {
    
    bst_node_t *new_node = (bst_node_t *) malloc(sizeof(bst_node_t));
    if (new_node == NULL) {
        exit(INTERNAL_ERR);
    }   
    new_node->id = name;
    //Setting Node to variable mode
    new_node->is_function = false;
    new_node->variable_data.var_type = var_type;
    new_node->variable_data.is_param = false;
    new_node->lptr = NULL;
    new_node->rptr = NULL;
    return new_node;
}

bst_node_t *new_node_parameter_var(int var_type, char* name, int param_index) {
    
    bst_node_t *new_node = (bst_node_t *) malloc(sizeof(bst_node_t));
    if (new_node == NULL) {
        exit(INTERNAL_ERR);
    }   
    new_node->id = name;
    //Setting Node to variable mode
    new_node->is_function = false;
    new_node->variable_data.var_type = var_type;
    new_node->variable_data.is_param = true;
    new_node->variable_data.param_index = param_index;
    new_node->lptr = NULL;
    new_node->rptr = NULL;
    return new_node;
}

void bst_insert(bst_node_t **btree, bst_node_t *new_node, char *id) {
  if ((*btree) == NULL) {
    (*btree) = new_node;
    (*btree)->id = id;
  }
  //Zavola funkciu do potomka
  else {
    //Found a node with identical identificator
    if (strcmp(id, (*btree)->id) == 0) {
      //TODO - funkcia/premenna uz je v strome
      (*btree) = new_node;
      return;
    }
    //ID of new node is smaller than current root, continuing left in the tree
    if (strcmp(id, (*btree)->id) < 0) {
      bst_insert(&(*btree)->lptr, new_node, id);
      return;
    }
    //ID of new node is bigger than current root, continuing right in the tree
    if (strcmp(id, (*btree)->id) > 0) {
      bst_insert(&(*btree)->rptr, new_node, id);
      return;
    }
  }
}  

bst_node_t *bst_search(bst_node_t **btree, char *id) {
  if ((*btree) != NULL) {
    if (strcmp(id, (*btree)->id) == 0) {

      return (*btree);
    }
    else {
      if (strcmp(id, (*btree)->id) < 0) {
        return bst_search(&(*btree)->lptr, id);
      }
      else {
        return bst_search(&(*btree)->rptr, id);
      }
    }
  }
  return NULL;
}

void bst_replace_by_rightmost(bst_node_t *target_node, bst_node_t **btree)  {
bst_node_t *del_node;
  
  if ((*btree)->rptr == NULL) {
    del_node = (*btree);
    target_node = (*btree);

    target_node->lptr = NULL;
    (*btree) = NULL;
    if (del_node->lptr != NULL) {
      target_node->lptr = del_node->lptr;
      (*btree) = del_node->lptr;
    }
    free(del_node);
    return;
  }

  
  if ((*btree)->rptr->rptr == NULL) {
    del_node = (*btree)->rptr;

    target_node = del_node;

    (*btree)->rptr = NULL;
    if(del_node->lptr != NULL) {
      (*btree)->rptr = del_node->lptr;
    }

    free(del_node);
    return;
  }
  bst_replace_by_rightmost(target_node, &(*btree)->rptr);
}


void bst_delete(bst_node_t **btree, char *id) {
  bst_node_t *del_node;
  if ((*btree) != NULL) {
    
    //prvok je root
    if (strcmp((*btree)->id, id) == 0) {
      //leaf node
      if (((*btree)->lptr == NULL) && ((*btree)->rptr == NULL)) {
        del_node = (*btree);
        free(del_node);
        return;
      }
      //len lavy potomok
      if  (((*btree)->rptr == NULL) && (*btree)->lptr != NULL) {
        del_node = (*btree);
        (*btree) = (*btree)->lptr;
        free(del_node);
        return;
      }
      //len pravy potomok
      if (((*btree)->rptr != NULL) && (*btree)->lptr == NULL) {
        del_node = (*btree);
        (*btree) = (*btree)->rptr;
        free(del_node);
        return;
      }
      //obaja potomkovia
      if (((*btree))->rptr != NULL && ((*btree)->lptr != NULL)) {
        bst_replace_by_rightmost((*btree), &(*btree)->lptr);
        return;
      }
    }

    //root key je mensi ako hladany key
    if (strcmp((*btree)->id, id) == 0) {
      //kontrola, ci existuje pravy poduzol a ci sa rovna kluc
      if (((*btree)->rptr != NULL) && (strcmp((*btree)->rptr->id, id) == 0)) {
        del_node = (*btree)->rptr;
        //leaf node
        if ((del_node->rptr == NULL) && (del_node->lptr == NULL)) {
          free(del_node);
          (*btree)->rptr = NULL;
          return;
        }
        //len lavy potomok
        if((del_node->rptr == NULL) && (del_node->lptr != NULL)) {
          (*btree)->rptr = (*btree)->rptr->lptr;
          free(del_node);
          return;
        }
        //len pravy potomok
        if((del_node->rptr != NULL) && (del_node->lptr == NULL)) {
          (*btree)->rptr = (*btree)->rptr->rptr;
          free(del_node);
          return;
        }
        //obaja potomkovia
        if((del_node->rptr != NULL) && (del_node->lptr != NULL)) {
          bst_replace_by_rightmost((*btree)->rptr, &(*btree)->rptr->lptr);
          return;
        } 
      }

      //nenajdene
      bst_delete(&(*btree)->rptr, id);
    }

    //root key je vacsi ako hladany key
    if (strcmp((*btree)->id, id) > 0) {
      //kontrola, ci existuje lavy poduzol a ci sa rovna kluc
      if (((*btree)->lptr != NULL) && (strcmp((*btree)->lptr->id, id) == 0)) {
        del_node = (*btree)->lptr;
        //leaf node
        if ((del_node->lptr == NULL)&&(del_node->rptr == NULL)) {
            (*btree)->lptr = NULL;
            free(del_node);
            return;
        }
        //iba lavy potomok
        if ((del_node->lptr != NULL) && (del_node->rptr == NULL)) {
          (*btree)->lptr = (*btree)->lptr->lptr;
          free(del_node);
          return;
        }
        //iba pravy potomok
        if ((del_node->lptr == NULL) && (del_node->rptr != NULL)) {
          (*btree)->lptr = (*btree)->lptr->rptr;
          free(del_node);
          return;
        }
        //obaja potomkovia
        if ((del_node->lptr != NULL) && (del_node->rptr != NULL)) {
          bst_replace_by_rightmost((*btree)->lptr, &(*btree)->lptr->lptr);
          return;
        }
      }

      //nenajdene
      bst_delete(&(*btree)->lptr, id);
    }
  }
}



void bst_dispose(bst_node_t *btree) {
  if (btree == NULL) {
    return;
  }
  //bst_dispose(btree->function_parameters.var_tree);
  bst_dispose(btree->lptr);
  bst_dispose(btree->rptr);
  free(btree);
  btree = NULL;
}

void bst_free_param_lists(bst_node_t *btree) {
  
  if (btree != NULL) {
    bst_free_param_lists(btree->lptr);
    param_list_dispose(btree->function_parameters.param_list);
    bst_free_param_lists(btree->rptr);
  }
}

void bst_print_node(bst_node_t *node) {
  printf("[%s]", node->id);
}




