# CompilerFrontEnd
# Scanner/Parser for SUBC

# Running the Compiler
1. Invoke 'make'. This will create an executable `p1`
2. There are 2 acceptable ways to run the program:
   1. `./p1 path/to/testprog`
       * Runs the program, but produces no output.
   2. `./p1 -ast path/to/testprog`
       * Prints the abstract syntax tree, and nothing else.


# To Validate Output From the -ast Switch
1. Pipe the output to a file.
   1. `./p1 path/to/testprog`
2. Diff with the desired file.
   1. `diff tests/tiny_01.tree out.tree`
3. Alternatively, combine these two into one line:
   1. `./p1 -ast tests/tiny_01 > out.tree && diff tests/tiny_01.tree out.tree`
4. For convenience, there is a bash script that will test all cases.
   1. `bash script.bash`
