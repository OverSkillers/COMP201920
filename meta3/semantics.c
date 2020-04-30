#include "semantics.h"

#define _NONE 0

table_t* sem(node* tree,node* begin){
    global = new_table("Class",begin);
    // temp = vardecl ou funcdecl
    node* temp = tree->son; 
    table_t* cur_table = global;
    while(temp){
        printf("%s\n",temp->name);
        temp = temp->next;
    }

    return global;
}

