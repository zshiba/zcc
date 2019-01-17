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
* rbp: base pointer
* rcx

(memo IA-32)
* eax: accumulator
* ebx: base register
* ecx: count register
* edx: data register
* esi: source index
* edi: destination index
* ebp: base pointer
* esp: stack pointer

### Instractions (NEED CHECK)
* `#` (comment line)
* `.intel_syntax noprefix` (Add this at the top to specify the Intel syntax is used.)
* add r1, r2: add r2 to r1 and store the result in r1
* sub r1, r2: subtract r2 from r1 and store the result in r1
* mul r: multiply r by rax and store the result in rax
* div r: divide concatination(rdx, rax) by r and store the result in rax
* mov r1, r2: load r2 to r1
* mov r, v: load v to r
* call s: invoke s
* ret v: return v
* push v: push v to the stack
* pop r: pop the value from the stack and store it in r

### Emulating stack machine
example: v1 + v2
```
push v1
push v2
pop rdi
pop rax
add rax, rdi
push rax
```

### Function frame
prologue
```
push rbp
mov rbp, rsp
sub rsp, n (where n is the total size of local variables in byte)
```
epilogue
```
mov rsp, rbp
pop rbp
ret
```

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
