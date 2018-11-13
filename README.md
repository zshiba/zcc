# zcc
(Working In Progress) A tiny compiler


# Notes
## Assembly
### x86-64 Registers
    * rsp: stack pointer
    * rax: store the return value of a function call
    * rdi: store the first argument for a function call
    * rsi: store the second argument for a function call
    * rdx: store the third argument for a function call
    * rip: program counter
    * rbp
    * rsp
    * rcx

### Instractions
    * `#` (comment line)
    * `.intel_syntax noprefix` (Add this at the top to specify the Intel syntax is used.)
    * add
    * sub
    * mov
    * call
    * ret


## Shell
```
$ echo @?  #Print the return code of the command run the most recently
```

## Makefile
    * [phony](https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html#Phony-Targets)
    * [wildcard](https://www.gnu.org/software/make/manual/html_node/Wildcard-Function.html#Wildcard-Function)
    * [implicit rules](https://www.gnu.org/software/make/manual/html_node/Using-Implicit.html#Using-Implicit)
    * [automatic variables](https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html#Automatic-Variables)

## Keyword
    * Application Binary Interface (ABI)
    * `-E` option for the compiler to show the code after the preprocessor. e.g. `gcc -E file.c`