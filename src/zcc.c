#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "zcc.h"

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

void tokenize(char* p){
  int i = 0;
  while((*p) != '\0'){
    if(isspace((*p))){
      ++p;
    }else if((*p) == '+' || (*p) == '-'){
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

void error(char* input){
  fprintf(stderr, "(Error) Unexpected token: %s\n", input);
  exit(1);
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "(Error) Incorrect numbers of arguments\n");
    return 1;
  }else{

    tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");

    int i = 0;
    if(tokens[i].type != TOKEN_NUMBER){
      error(tokens[i].input);
    }else{
      printf("  mov rax, %d\n", tokens[i].value);
      ++i;
    }
    while(tokens[i].type != TOKEN_EOF){
      if(tokens[i].type == '+'){
        ++i;
        if(tokens[i].type != TOKEN_NUMBER){
          error(tokens[i].input);
        }else{
          printf("  add rax, %d\n", tokens[i].value);
          ++i;
        }
      }else if(tokens[i].type == '-'){
        ++i;
        if(tokens[i].type != TOKEN_NUMBER){
          error(tokens[i].input);
        }else{
          printf("  sub rax, %d\n", tokens[i].value);
          ++i;
        }
      }else{
        error(tokens[i].input);
      }
    }
    printf("  ret\n");

    return 0;
  }
}
