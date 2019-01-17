#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "zcc.h"

void error(char* format, char* input){
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

enum{
  TOKEN_NUMBER = 256, //to avoid these overlapping ascii table
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

void tokenize(char* p){
  while((*p) != '\0'){
    if(isspace((*p))){
      ++p;
    }else if((*p) == '+' || (*p) == '-' || (*p) == '*' || (*p) == '/' || (*p) == '(' || (*p) == ')'){
      Token* token = new_token((*p), 0, p);
      vector_push(tokens, token);
      ++p;
    }else if(isdigit((*p))){
      Token* token = new_token(TOKEN_NUMBER, strtol(p, &p, 10), p);
      vector_push(tokens, token);
    }else{
      fprintf(stderr, "(Error) Failure to tokenize: '%s'\n", p);
      exit(1);
    }
  }
  Token* token = new_token(TOKEN_EOF, 0, p);
  vector_push(tokens, token);
}

enum{
  NODE_NUMBER = 256
};

typedef struct Node{
  int type;
  int value;
  struct Node* lhs;
  struct Node* rhs;
}Node;

Node* new_node(int type, int value, Node* lhs, Node* rhs){
  Node* node = malloc(sizeof(Node));
  node->type = type;
  node->value = value;
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
    Node* newbie = new_node(NODE_NUMBER, ((Token*)tokens->data[position])->value, NULL, NULL);
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
      node = new_node('*', 0, node, term());
    else if(consume('/'))
      node = new_node('/', 0, node, term());
    else
      return node;
  }
}

Node* add(){
  Node* node = multiply();
  for(;;){
    if(consume('+'))
      node = new_node('+', 0, node, multiply());
    else if(consume('-'))
      node = new_node('-', 0, node, multiply());
    else
      return node;
  }
}

void generate(Node* node){
  if(node->type == NODE_NUMBER){
    printf("  push %d\n", node->value);
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
void print(Node* node){
  if(node != NULL){
    fprintf(stderr, "%d %d\n", node->type, node->value);
    print(node->lhs);
    print(node->rhs);
  }
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "(Error) Incorrect numbers of arguments\n");
    return 1;
  }else{

    tokens = new_vector();
    tokenize(argv[1]);
//for(int i = 0; i < 100; i++)
//  fprintf(stderr, "%d %d\n", tokens[i].type, tokens[i].value);

    Node* node = add();
//print(node);

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");

    generate(node);

    printf("  pop rax\n"); //expect that the return value is on top of the stack
    printf("  ret\n");
    return 0;
  }
}
