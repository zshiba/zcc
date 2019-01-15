#include <stdio.h>
#include <stdlib.h>
#include "zcc.h"

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Wrong number of arguments\n");
    return 1;
  }else{

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");

    char* p = argv[1];
    printf("  mov rax, %ld\n", strtol(p, &p, 10));
    while((*p) != '\0'){
      if((*p) == '+'){
        ++p;
        printf("  add rax, %ld\n", strtol(p, &p, 10));
      }else if((*p) == '-'){
        ++p;
        printf("  sub rax, %ld\n", strtol(p, &p, 10));
      }else{
        fprintf(stderr, "Unexpected charactor: '%c'\n", (*p));
        return 1;
      }
    }
    printf("  ret\n");

    return 0;
  }
}
