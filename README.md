# Lexer for Simple Programming Language

This project implements a lexical analyzer (lexer) for a simplified programming language using a deterministic finite automaton (DFA). It reads a source file, tokenizes the content, and reports any lexical errors with helpful diagnostics.

## Features

-   Recognizes keywords like `if`, `else`, `int`, `return`, `void`, `while`
-   Supports basic punctuation and operators (`+`, `-`, `*`, `/`, `==`, `!=`, `<=`, etc.)
-   Handles C-style comments (`/* ... */`)
-   Reports invalid tokens with line and column indicators
-   Can optionally suppress token output

## Build Instructions

Make sure you have `g++` installed. Then, in the project directory:

```bash
make
```

## Usage

```bash
./lexer <source_file> [-p]
```

-   `<source_file>`: The input source file to be tokenized.
-   `-p`: Optional flag to suppress token output.

## Examples

```bash
./lexer Tests/Error.c--
./lexer Tests/Simple.c-- -p
```

## Error Reporting

If an invalid token is encountered, the lexer highlights the offending character and the state in which the error occurred. Example:

```cpp
Line 3: Error on ID formation:
int 9num;
    ^
```

## Cleanup

To remove the compiled binary:

```bash
make clean
```

## License

This project is distributed under the [MIT License](https://opensource.org/licenses/MIT).
