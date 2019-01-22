#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <ctype.h>
#include <string.h>
#include "zcc.h"

noreturn void error(char* format, char* input){
  fprintf(stderr, format, input);
  exit(1);
}

typedef struct{
  void** data;
  int capacity;
  int length;
}Vector;

Vector* new_vector(){
  Vector* v = malloc(sizeof(Vector));
  v->capacity = 16;
  v->data = malloc(sizeof(void*) * v->capacity);
  v->length = 0;
  return v;
}

void vector_push(Vector* v, void* element){
  if(v->capacity == v->length){
    v->capacity *= 2;
    v->data = realloc(v->data, sizeof(void*) * v->capacity);
  }
  v->data[v->length] = element;
  v->length = v->length + 1;
}

//Note: design choice, allow to put duplicated keys, but restrict to get only the value associated with the latest key
typedef struct{
  Vector* keys;
  Vector* values;
}Map;

Map* new_map(){
  Map* map = malloc(sizeof(Map));
  map->keys = new_vector();
  map->values = new_vector();
  return map;
}

void map_put(Map* map, char* key, void* value){
  vector_push(map->keys, key);
  vector_push(map->values, value);
}

void* map_get(Map* map, char* key){
  void* value = NULL;
  for(int i = map->keys->length - 1; i >= 0; i--){
    if(strcmp(map->keys->data[i], key) == 0){
      value = map->values->data[i];
      break;
    }
  }
  return value;
}

enum{
  TOKEN_NUMBER = 256, //to avoid these overlapping ascii table
  TOKEN_IDENTIFIER,
  TOKEN_EOF
};

typedef struct{
  int type;
  int value;
  char* input;
}Token;

Token* new_token(int type, int value, char* input){
  Token* token = malloc(sizeof(Token));
  token->type = type;
  token->value = value;
  token->input = input;
  return token;
}

Vector* tokens = NULL; //ad-hoc, still in global
int position = 0;  //ad-hoc, the index of current charactor in the given source
Vector* code = NULL; //ad-hoc, to store a sequence of statements

Vector* tokenize(char* p){
  Vector* v = new_vector();
  while((*p) != '\0'){
    if(isspace((*p))){
      ++p;
    }else if('a' <= (*p) && (*p) <= 'z'){
      Token* token = new_token(TOKEN_IDENTIFIER, (*p), p);
      vector_push(v, token);
      ++p;
    }else if((*p) == '+' || (*p) == '-' ||
             (*p) == '*' || (*p) == '/' ||
             (*p) == '(' || (*p) == ')' ||
             (*p) == ';' || (*p) == '='){
      Token* token = new_token((*p), 0, p);
      vector_push(v, token);
      ++p;
    }else if(isdigit((*p))){
      Token* token = new_token(TOKEN_NUMBER, strtol(p, &p, 10), p);
      vector_push(v, token);
    }else{
      fprintf(stderr, "(Error) Failure to tokenize: '%s'\n", p);
      exit(1);
    }
  }
  Token* token = new_token(TOKEN_EOF, 0, p);
  vector_push(v, token);
  return v;
}

enum{
  NODE_NUMBER = 256,
  NODE_IDENTIFIER
};

typedef struct Node{
  int type;
  int value; //for NODE_NUMBER
  char name; //for NODE_IDENTIFIER
  struct Node* lhs;
  struct Node* rhs;
}Node;

Node* new_node(int type, int value, char name, Node* lhs, Node* rhs){
  Node* node = malloc(sizeof(Node));
  node->type = type;
  node->value = value;
  node->name = name;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

int consume(int type){
  if(((Token*)tokens->data[position])->type == type){
    ++position;
    return 1;
  }else{
    return 0;
  }
}

Node* add();

Node* term(){
  if(consume('(')){
    Node* node = add();
    if(consume(')'))
      return node;
    else
      error("(Error) No closing parenthesis: %s\n", ((Token*)tokens->data[position])->input);
  }else if(((Token*)tokens->data[position])->type == TOKEN_NUMBER){
    Node* newbie = new_node(NODE_NUMBER, ((Token*)tokens->data[position])->value, '0', NULL, NULL);
    ++position;
    return newbie;
  }else if(((Token*)tokens->data[position])->type == TOKEN_IDENTIFIER){
    Node* newbie = new_node(NODE_IDENTIFIER, 0, ((Token*)tokens->data[position])->value, NULL, NULL);
    ++position;
    return newbie;
  }else{
    error("(Error) Unknown token: %s\n", ((Token*)tokens->data[position])->input);
  }
}

Node* multiply(){
  Node* node = term();
  for(;;){
    if(consume('*'))
      node = new_node('*', 0, '0', node, term());
    else if(consume('/'))
      node = new_node('/', 0, '0', node, term());
    else
      return node;
  }
}

Node* add(){
  Node* node = multiply();
  for(;;){
    if(consume('+'))
      node = new_node('+', 0, '0', node, multiply());
    else if(consume('-'))
      node = new_node('-', 0, '0', node, multiply());
    else
      return node;
  }
}

Node* assignment(){
  Node* node = add();
  if(consume('='))
    return new_node('=', 0, '0', node, add());
  else
    return node;
}

Node* statement(){
  Node* node = assignment();
  if(consume(';'))
    return node;
  else
    error("(Error) ';' is expected: %s\n", ((Token*)tokens->data[position])->input);
}

Vector* program(){
  Vector* code = new_vector();
  while(((Token*)tokens->data[position])->type != TOKEN_EOF){
    Node* node = statement();
    vector_push(code, node);
  }
  return code;
}

void generate_lvalue(Node* node){
  if(node->type != NODE_IDENTIFIER){
    error("(Error) variable name is expected\n", "");
  }else{
    int offset = ('z' - node->name + 1) * 8;
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", offset);
    printf("  push rax\n");
  }
}

void generate(Node* node){
  if(node->type == NODE_NUMBER){
    printf("  push %d\n", node->value);

  }else if(node->type == NODE_IDENTIFIER){
    generate_lvalue(node);

    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");

  }else if(node->type == '='){
    generate_lvalue(node->lhs);
    generate(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");

  }else{
    generate(node->lhs);
    generate(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch(node->type){
      case '+':
        printf(  "add rax, rdi\n");
        break;
      case '-':
        printf("  sub rax, rdi\n");
        break;
      case '*':
        printf("  mul rdi\n");
        break;
      case '/':
        printf("  mov rdx, 0\n"); //zero clear unused bits before the concatenation happens in div
        printf("  div rdi\n");
        break;
      default:
        error("(Error) Unknown node-type\n", "\0");
        break;
    }

    printf("  push rax\n");
  }
}


//for debug
void print_tokens(){
  fprintf(stderr, "--- print takens start\n");
  for(int i = 0; i < tokens->length; i++)
    fprintf(stderr, "type: %d, value: %d\n", ((Token*)tokens->data[i])->type, ((Token*)tokens->data[i])->value);
  fprintf(stderr, "--- print takens end\n");
}

//for debug
void print_node(Node* node){
  if(node != NULL){
    fprintf(stderr, "type: %d, value: %d, name: %c\n", node->type, node->value, node->name);
    print_node(node->lhs);
    print_node(node->rhs);
  }
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "(Error) Incorrect numbers of arguments\n");
    return 1;
  }else{

    tokens = tokenize(argv[1]);
//print_tokens();
    code = program();

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");

    //prologue
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n"); //ad-hoc, 208 (= 26 charactors * 8), a to z

    for(int i = 0; i < code->length; i++){
      Node* node = code->data[i];
//print_node(node);
      generate(node);
      printf("  pop rax\n");
    }

    //epilogue
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return 0;
  }
}
