#include "ast.h"

node* create_node(const char* name,const char* type, int line,int col){
	//printf("Creating node %s\n", name);
	node* n = (node *) malloc(sizeof(node));
	// TODO: Free this memory
	n->name = strdup(name);
	// TODO: Free this memory
	n->type = strdup(type);
    n->line = line;
    n->col = col;
    n->param = 0;
	// TODO: Free this memory
	n->annotation = NULL;
	return n;
}

void set_annotation(node* dest, const char* annotation){
	/*TODO: Free this memory*/
	if (annotation){
		dest->annotation = strdup(annotation);
	}
}

void set_literal_symbol(node* dest, const char* literal){
	if (literal){
		dest->literal = strdup(literal);
	}
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



void print_tree(int level, node* n,int abs){
	if (n == NULL) return;
	for (int i = 0; i < level; i++) printf("..");
	if(abs == 0){
		if(strcmp(n->type,"NULL")==0){
			printf("%s", n->name);
		}else if(strcmp(n->name,"StrLit")==0){
			printf("%s(\"%s\")",n->name,n->type);
		}else{
			printf("%s(%s)",n->name,n->type);
		}
		printf("\n");
	}
    if(abs == 1){
        if(strcmp(n->type,"NULL")==0){
			printf("%s", n->name);
		}else if(strcmp(n->name,"StrLit")==0){
			printf("%s(\"%s\")",n->name,n->type);
		}else{
			printf("%s(%s)",n->name,n->type);
		}
		
		if (n->annotation != NULL) printf(" - %s", n->annotation);
		printf("\n");
    }
	print_tree(level + 1, n->son, abs);
	print_tree(level, n->next, abs);
}

void free_tree(node* n){
	if (n == NULL){
		return;
	}
	if (n->son != NULL){
		free_tree(n->son);
		n->son = NULL;
	}
	if (n->next != NULL){
		free_tree(n->next);
		n->next = NULL;
	}
	free(n->name);
	free(n->type);
	free(n->annotation);
	free(n->literal);
	free(n);
}

int getLine(struct _args* n){
	if(n == NULL){
		return 0;
	}
	else{
		return n->line;
	}
}

int getCol(struct _args* n){
	if(n == NULL){
		return 0;
	}
	else{
		return n->col;
	}
}

int getLineN(node* n){
	if(n == NULL){
		return 0;
	}
	else{
		return n->line;
	}
}

int getColN(node* n){
	if(n == NULL){
		return 0;
	}
	else{
		return n->col;
	}
}
