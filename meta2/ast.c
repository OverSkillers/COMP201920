#include "ast.h"

node* create_node(const char* name,const char* type){
	//printf("Creating node %s\n", name);
	node* n = (node *) malloc(sizeof(node));
	n->name = strdup(name);
	n->type = strdup(type);
	return n;
}

void add_next(node* n, node* n2){
	if (n == NULL || n2 == NULL){
		return;
	}
	node* temp = n;
	/*printf("Adding next %s(%s) ", n2->name, n2->type);
	printf("to %s(%s)\n", n->name, n->type);*/
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
	/*printf("Adding son %s(%s) ", son->name, son->type);
	printf("to %s(%s)\n", n->name, n->type);*/
	if (n->son == NULL){
		n->son = son;
	} else {
		add_next(n->son, son);
	}
}

bool is_block(node* n){
	if (n != NULL && n->next != NULL) return true;
	return false;
}



void print_tree(int level, node* n){
	if (n == NULL) return;
	for (int i = 0; i < level; i++) printf("..");
	if(strcmp(n->type,"NULL")==0){
		printf("%s\n", n->name);
	}else if(strcmp(n->name,"StrLit")==0){
		printf("%s(\"%s\")\n",n->name,n->type);
	}else{
		printf("%s(%s)\n",n->name,n->type);
	}
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
