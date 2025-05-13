# Compiler for Simple Programming Language (C--)

This project implements a compiler for a simple programming language called "C--". The compiler takes a source file as input and generates a token stream that can be used for further processing or analysis.

## Features

### Lexer

The lexer performs the following tasks:

-   Recognizes keywords like `if`, `else`, `int`, `return`, `void`, `while`
-   Supports basic punctuation and operators (`+`, `-`, `*`, `/`, `==`, `!=`, `<=`, etc.)
-   Handles C-style comments (`/* ... */`)
-   Reports invalid tokens with line and column indicators
-   Can output the token stream

### Parser

-   Generates an abstract syntax tree (AST) from the token stream
-   Reports syntax errors with line indicators
-   Can output the AST

### Semantic Analysis

-   Symbol Table Generation
-   Type Checking
-   Can output the symbol table

## Build Instructions

Make sure you have `g++` installed. Then, in the project directory:

```bash
make
```

## Usage

```bash
./lexer <source_file> [-lps]
```

-   `<source_file>`: The input source file to be tokenized.
-   `-l`: Print lexer output.
-   `-p`: Print parser output.
-   `-s`: Print symbol table.

## Examples

```bash
./lexer Tests/Error.c--
./lexer Tests/Simple.c-- -lps
```

## Error Handling

Each error is reported with a line number and error message.

## Cleanup

To remove the compiled binary:

```bash
make clean
```

To remove all generated files:

```bash
make fclean
```

## License

This project is distributed under the [MIT License](https://opensource.org/licenses/MIT).
