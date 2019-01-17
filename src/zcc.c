#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "zcc.h"

void error(char* format, char* input){
  fprintf(stderr, format, input);
  exit(1);
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

Token tokens[100]; //ad-hoc, fixed size just for now
int position = 0;  //ad-hoc, the index of current charactor in the given source

void tokenize(char* p){
  int i = 0;
  while((*p) != '\0'){
    if(isspace((*p))){
      ++p;
    }else if((*p) == '+' || (*p) == '-' || (*p) == '*' || (*p) == '/' || (*p) == '(' || (*p) == ')'){
      tokens[i].type = (*p);
      tokens[i].value = 0; //ad-hoc, redundant
      tokens[i].input = p;
      ++i;
      ++p;
    }else if(isdigit((*p))){
      tokens[i].type = TOKEN_NUMBER;
      tokens[i].value = strtol(p, &p, 10);
      tokens[i].input = p;
      ++i;
    }else{
      fprintf(stderr, "(Error) Failure to tokenize: '%s'\n", p);
      exit(1);
    }
  }
  tokens[i].type = TOKEN_EOF;
  tokens[i].value = 0; //ad-hoc, redundant
  tokens[i].input = p;
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
  if(tokens[position].type == type){
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
      error("(Error) No closing parenthesis: %s\n", tokens[position].input);
  }else if(tokens[position].type == TOKEN_NUMBER){
    Node* newbie = new_node(NODE_NUMBER, tokens[position].value, NULL, NULL);
    ++position;
    return newbie;
  }else{
    error("(Error) Unknown token: %s\n", tokens[position].input);
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
