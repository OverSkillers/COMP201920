#include "ast.h"
#include "structures.h"

/*node* create_node(const char* name){
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
}*/



program* insert_program(char* id, field_decl_list* field_decls, method_decl_list* method_decls){
	program* prgm = malloc(sizeof(program));

	prgm->id = id;
	prgm->field_decl_l = field_decls;
	prgm->method_decl_l = method_decls;

	return prgm;
}

method_decl_list* insert_method_decl(method_decl_list* head, method_decl* decl){
	if (decl == NULL) return NULL;

	method_decl_list* node = malloc(sizeof(method_decl_list));

	node->data = decl;
	node->next = NULL;

	if (head == NULL) return node;

	node->next = head;

	return node;
}

field_decl* create_field_decl(t_var type, char* id){
	field_decl* node_decl = malloc(sizeof(field_decl));

	node_decl->id = id;
	node_decl->type = type;

	return node_decl;
}

field_decl_list* insert_field_decl(field_decl_list* head, t_var type, field_decl_ids* ids){
	if (ids == NULL) return NULL;

	field_decl_ids* iterator = ids;
	for (; iterator; iterator = iterator->next){
		field_decl_list* node = malloc(sizeof(field_decl_list));
		field_decl_list* temp;

		node->data = create_field_decl(type, iterator->id);
		node->next = NULL;

		if (head == NULL){
			head = node;
			continue;
		}

		for (temp = head; temp->next; temp = temp->next);
		temp->next = node;
	}

	return head;
}

field_decl_ids* insert_field_decl_id(field_decl_ids* head, char* id){
	field_decl_ids* node = malloc(sizeof(field_decl_ids));

	node->id = strdup(id);
	node->next = NULL;

	if (head == NULL) return node;

	node->next = head;

	return node;
}

//TODO: Free field_decl_ids

method_decl* create_method_decl(method_header* header, method_body* body){
	method_decl* node = malloc(sizeof(method_decl));

	node->header = header;
	node->body = body;

	return node;
}

method_header* create_method_header(t_var type, char* id, param_decl_list* params){
	method_header* header = malloc(sizeof(method_header));

	header->id = strdup(id);
	header->type = type;

	if (params != NULL) header->params = params;

	return header;
}


param_decl_list* insert_param_decl(param_decl_list* head, param_decl* param){
	param_decl_list* node = malloc(sizeof(param_decl_list));

	node->data = param;
	node->next = NULL;

	if (head == NULL) return node;

	node->next = head;

	return node;
}

param_decl* create_param_decl(t_var type, char* id){
	param_decl* node = malloc(sizeof(param_decl));

	node->type = type;
	node->id = strdup(id);

	return node;
}

method_body* create_method_body(method_body_nodes* nodes){
	method_body* body = malloc(sizeof(method_body));

	body->nodes = nodes;

	return body;
}

method_body_nodes* insert_mbody_statement(method_body_nodes* head, statement_list* stmts){
	method_body_nodes* node = malloc(sizeof(method_body_nodes));

	node->data.stmts = stmts;
	node->type = t_stmt;
	node->next = NULL;

	if (head == NULL) return node;

	node->next = head;

	return node;
}

method_body_nodes* insert_mbody_vardecl(method_body_nodes* head, vardecl_list* vars){
	method_body_nodes* node = malloc(sizeof(method_body_nodes));

	node->data.vars = vars;
	node->type = t_vardecl;
	node->next = NULL;

	if (head == NULL) return node;

	node->next = head;

	return node;
}


vardecl_ids* insert_vardecl_id(vardecl_ids* head, char* id){
	vardecl_ids* node = malloc(sizeof(vardecl_ids));

	node->id = strdup(id);
	node->next = NULL;

	if (head == NULL) return node;

	node->next = head;

	return node;
}

vardecl* create_vardecl(t_var type, char* id){
	vardecl* var = malloc(sizeof(vardecl));

	var->id = id;
	var->type = type;

	return var;
}

//TODO: Free vardecl_ids
vardecl_list* insert_vardecl(vardecl_list* head, t_var type, vardecl_ids* ids){
	if (ids == NULL) return NULL;

	vardecl_ids* iterator = ids;
	for (; iterator; iterator = iterator->next){
		vardecl_list* node = malloc(sizeof(vardecl_list));
		vardecl_list* temp;

		node->data = create_vardecl(type, iterator->id);
		node->next = NULL;

		if (head == NULL){
			head = node;
			continue;
		}

		for (temp = head; temp->next; temp = temp->next);
		temp->next = node;
	}

	return head;
}

statement* create_if_statement(expression* condition, statement_list* if_branch, statement_list* else_branch){
	statement* stmt = malloc(sizeof(statement));

	stmt->type = t_if;
	stmt->data.if_stmt->if_branch = if_branch;
	stmt->data.if_stmt->else_branch = else_branch;

	return stmt;
}

statement_list* insert_statement(statement_list* head, statement* stmt){
	statement_list* node = malloc(sizeof(statement_list));

	node->data = stmt;
	node->next = NULL;

	if (head == NULL) return node;

	node->next = head;

	return node;
}


expression* insert_expression_node(t_expr type, char* left, char* right){
	expression* expr = malloc(sizeof(expression));

	expr->type = type;
	expr->left = left;
	expr->right = right;

	return expr;
}

void print_tree(program* root){
	//printf("Begin printing\n");

	printf("Program\n");

	if (root->id != NULL) printf("..ID(%s)\n", root->id);

	if (root->field_decl_l != NULL){
		for (field_decl_list* temp1 = root->field_decl_l; temp1; temp1 = temp1->next){
			printf("..FieldDecls\n");
			if(temp1->data->type == 0){
				printf("....Int\n");
			}
			if(temp1->data->type == 1){
				printf("....Double\n");
			}
			if(temp1->data->type == 2){
				printf("....Bool\n");
			}
			if(temp1->data->type == 3){
				printf("....Void\n");
			}
			if(temp1->data->type == 4){
				printf("....StringArray\n");
			}
			printf("....Id(%s)\n", temp1->data->id);
		}
	}

	if (root->method_decl_l != NULL){
		method_decl_list* temp;
		for (temp = root->method_decl_l; temp; temp = temp->next){
			printf("..MethodDecl\n");
			printf("....MethodHeader\n");
			if(temp->data->header->type == 0){
				printf("......Int\n");
			}
			if(temp->data->header->type == 1){
				printf("......Double\n");
			}
			if(temp->data->header->type == 2){
				printf("......Bool\n");
			}
			if(temp->data->header->type == 3){
				printf("......Void\n");
			}
			if(temp->data->header->type == 4){
				printf("......StringArray\n");
			}
			printf("......Id(%s)\n", temp->data->header->id);

			if (temp->data->header->params != NULL){
				param_decl_list* params = temp->data->header->params;
				for (param_decl_list* temp1 = params; temp1; temp1 = temp1->next){
					printf("......MethodParams\n");
					printf("........ParamDecl\n");
					if(temp1->data->type == 0){
						printf("..........Int\n");
					}
					if(temp1->data->type == 1){
						printf("..........Double\n");
					}
					if(temp1->data->type == 2){
						printf("..........Bool\n");
					}
					if(temp1->data->type == 3){
						printf("..........Void\n");
					}
					if(temp1->data->type == 4){
						printf("..........StringArray\n");
					}
					printf("..........Id(%s)\n", temp1->data->id);
				}
			}

			if (temp->data->body != NULL){
				method_body_nodes* nodes = temp->data->body->nodes;
				printf("....MethodBody\n");
				for (method_body_nodes* temp1 = nodes; temp1; temp1 = temp1->next){
					if (temp1->type == t_vardecl){
						vardecl_list* decls = temp1->data.vars;
						for (vardecl_list* temp2 = decls; temp2; temp2 = temp2->next){
							printf("......VarDecl\n");
							if(temp2->data->type == 0){
								printf("........Int\n");
							}
							if(temp2->data->type == 1){
								printf("........Double\n");
							}
							if(temp2->data->type == 2){
								printf("........Bool\n");
							}
							if(temp2->data->type == 3){
								printf("........Void\n");
							}
							if(temp2->data->type == 4){
								printf("........StringArray\n");
							}
							printf("........Id(%s)\n", temp2->data->id);
						}
					}if (temp1->type == t_stmt){
						statement_list* stmt = temp1->data.stmts;
						for(statement_list* temp2 = stmt; temp2; temp2 = temp2->next){
						printf("Statement\n");
						}
					}
				}
			}
		}
	}
}

//TODO: free tree
