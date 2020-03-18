#include "ast.h"

node* create_node(const char* name){
	//printf("Creating node %s\n", name);
	node* n = (node *) malloc(sizeof(node));
	n->name = strdup(name);
	return n;
}

void add_next(node* n, node* n2){
	if (n == NULL || n2 == NULL){
		return;
	}
	node* temp = n;
	//printf("Adding next %s to %s\n", n2->name, n->name);
	if (temp->next == NULL){
		temp->next = n2;
	} else {
		while (temp->next != NULL){
			temp = temp->next;
		}
		temp->next = n2;
	}
}

void add_son(node* n, node* son){
	if (n == NULL || son == NULL){
		return;
	}
	//printf("Adding son %s to %s\n", son->name, n->name);
	if (n->son == NULL){
		n->son = son;
	} else {
		add_next(n->son, son);
	}
}

bool is_block(node* n){
	if (n == NULL || n->next == NULL) return false;
	return true;
}

node* new_id(const char* name){
	char* arr = (char *) malloc(strlen(name) + 5);
	sprintf(arr, "Id(%s)", name);
	return create_node(arr);
}

void print_tree(int level, node* n){
	if (n == NULL) return;
	for (int i = 0; i < level; i++) printf("..");
	printf("%s\n", n->name);
	print_tree(level + 1, n->son);
	print_tree(level, n->next);
}

void free_tree(node* n){
	if (n == NULL){
		return;
	}
	if (n->son != NULL){
		free_tree(n->son);
	}
	if (n->next != NULL){
		free_tree(n->next);
	}
	free(n->name);
	free(n);
}
