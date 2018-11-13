#include <stdio.h>
#include "zcc.h"

int main(int argc, char* argv[]){

  printf(".intel_syntax noprefix\n");
  printf(".global _main\n");
  printf("_main:\n");
  printf("  mov rax, 14\n");
  printf("  ret\n");

  return 0;
}
