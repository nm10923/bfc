# NAME
bfc - Brainfuck Compiler and Interpreter
# SYNOPSIS
**bfc [hcd] [-f** \<filename\>**] [-s** \<size\>**] [-t** \<size\>**] [-o** \<output\>**] [FILENAME]**
# DESCRIPTION
## OVERVIEW
bfc is a program that can interpret, debug, and compile Brainfuck code.
## SWITCHES
**-h**\
    prints out a help message (and a cool ass logo)\
**-c**\
    compiles the file given by the **-f** switch argument \<filename\>\
**-d**\
    debugs the file given by the **-f** switch argument \<filename\>\
**-f \<filename\>**\
    handles the file given by \<filename\>\
**-s \<size\>**\
    (interpreter mode) sets the size of the virtual heap to \<size\> * sizeof(unsigned char) bytes\
    (compiler mode) makes sure that the compiler sets the size of the heap to \<size\> * sizeof(unsigned char) bytes\
**-t \<size\>**\
    (interpreter mode) sets the size of the virtual stack to \<size\> * sizeof(int) bytes\
    (compiler mode) makes sure that the compiler sets the size of the stack to \<size\> * sizeof(int) bytes\
**-t \<output\>**\
    (compiler mode only) sets the name of the compiler output to \<output\>\
# USAGE
The compiler (or more accuratley, transpiler) mode converts the parsed input into x86\_64 Linux assembly. It requires the Nasm assembler to be installed. It currently does not support x86\_64 Mac or x86\_64 Windows assembly.\
The interpreter mode runs the parsed input directly. It uses standard libc functions and was tested on top of glibc, but it should work on other C libraries that comply to the C library standard.\
The debugger mode is a wrapper to the interpreter mode that contains a terminal interface that has multiple commands that display information about the program currently running. To get a list of the debugger mode commands, get into the debugger mode and type in `help`.
# EXAMPLES
**bfc**\
    idk probably segfaults cuz the guy who programmed this sucks\
**bfc -f helloworld.bf**\
    interprets the file "helloworld.bf"\
**bfc -c -f helloworld.bf**\
    compiles the file "helloworld.bf"\
**bfc -d -f helloworld.bf**\
    opens up the debugger with the input being the file "helloworld.bf"\
**bfc -s 300 -t 20 -f helloworld.bf**\
    interprets the file "helloworld.bf" with the virtual heap size being 300*sizeof(unsigned char) bytes and the virtual stack size being 20*sizeof(int) bytes
# EXIT VALUES
0 if the program ran succesfully with no errors.\
1 if the program failed. The specific error that occured will be printed out into stderr, in the form of "[COMPONENT] ERROR\_MESSAGE" (i.e. "[LEXER] Illegal move instruction")
# BUGS
This program is still in ultra-ultra alpha and probably won't compile, so yeah there are quite a few bugs.
# AUTHOR
Written by NM10923
# SEE ALSO
nasm(1)
