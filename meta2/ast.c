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

field_decl_list* insert_field_decl(field_decl_list* head, t_var type, field_decl_ids* ids){
	if (ids == NULL) return NULL;

	field_decl_list* node = malloc(sizeof(field_decl_list));
	field_decl_list* temp;

	field_decl* node_decl = malloc(sizeof(field_decl));
	node_decl->ids = ids;
	node_decl->type = type;

	node->data = node_decl;
	node->next = NULL;

	if (head == NULL) return node;

	for (temp = head; temp->next; temp = temp->next);
	temp->next = node;

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

method_header* create_method_header(t_var type, char* id, method_params_list* params){
	method_header* header = malloc(sizeof(method_header));

	header->id = strdup(id);
	header->type = type;

	if (params != NULL) header->params = params;

	return header;
}


expression* insert_expression_node(t_expr type, char* left, char* right){
	expression* expr = malloc(sizeof(expression));

	expr->type = type;
	expr->left = left;
	expr->right = right;

	return expr;
}

void print_tree(program* root){
	printf("Begin printing\n");

	if (root->id != NULL) printf("ID(%s)\n", root->id);

	if (root->field_decl_l != NULL){
		printf("FieldDecls\n");
		for (field_decl_list* temp1 = root->field_decl_l; temp1; temp1 = temp1->next){
			printf("Type(%d)\n", temp1->data->type);
			for (field_decl_ids* temp = temp1->data->ids; temp; temp = temp->next){
				printf("Id(%s)\n", temp->id);
			}
		}
	}

	if (root->method_decl_l != NULL){
		printf("Methods\n");
		for (method_decl_list* temp = root->method_decl_l; temp; temp = temp->next){
			printf("MethodType(%d)\n", temp->data->header->type);
			printf("MethodId(%s)\n", temp->data->header->id);
		}
	}
}

//TODO: free tree