# zcc
(Working In Progress) A tiny compiler


# Notes
## Register
    * rsp
    * rax: store the return value of a function call
    * rdi: store the first argument for a function call
    * rsi: store the second argument for a function call
    * rbp
    * rsp
    * rcx

## Assembly
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
