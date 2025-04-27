#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <fstream>
#include <unordered_set>
#include <string>
#include <vector>
#include <array>
#include <stack>
#include <unordered_map>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Token names (31 tokens)
static const std::array<std::string, 31> token_names = {
    "else",
    "if",
    "int",
    "return",
    "void",
    "while",
    "+",
    "-",
    "*",
    "/",
    "(",
    ")",
    "[",
    "]",
    "{",
    "}",
    ",",
    ";",
    "<",
    ">",
    "==",
    "!=",
    "=",
    "<=",
    ">=",
    "id",
    "num",
    "COMMENT",
    "$",
    "ERROR",
    "Program"
};

// Token types returned by the lexer
enum TokenType {
    T_ELSE, // else
    T_IF, // if
    T_INT, // int
    T_RETURN, // return
    T_VOID, // void
    T_WHILE, // while
    T_PLUS, // +
    T_MINUS, // -
    T_STAR, // *
    T_SLASH, // / --------------
    T_LPAREN, // (
    T_RPAREN, // )
    T_LBRACKET, // [
    T_RBRACKET, // ]
    T_LBRACE, // {
    T_RBRACE, // }
    T_COMMA, // ,
    T_SEMICOLON, // ;
    T_LESS, // <
    T_GREATER, // >
    T_EQUAL, // ==
    T_NOTEQUAL, // !=
    T_ASSIGN, // = ---------------
    T_LESSEQUAL, // <=
    T_GREATEREQUAL, // >=
    T_ID, // identifier
    T_NUM, // number
    T_COMMENT, // /* ... */
    T_ENDFILE, // $
    T_ERROR, // IN CASE OF ERROR
    T_INITIAL
};

// State Names for debugging (79 states)
static const std::array<std::string, 79> state_names = {
    "ELSE_ACCEPT",
    "IF_ACCEPT",
    "INT_ACCEPT",
    "RETURN_ACCEPT",
    "VOID_ACCEPT",
    "WHILE_ACCEPT",
    "PLUS_ACCEPT",
    "MINUS_ACCEPT",
    "STAR_ACCEPT",
    "SLASH_ACCEPT",
    "LPAREN_ACCEPT",
    "RPAREN_ACCEPT",
    "LBRACKET_ACCEPT",
    "RBRACKET_ACCEPT",
    "LBRACE_ACCEPT",
    "RBRACE_ACCEPT",
    "COMMA_ACCEPT",
    "SEMICOLON_ACCEPT",
    "LESS_ACCEPT",
    "GREATER_ACCEPT",
    "EQUAL_ACCEPT",
    "NOTEQUAL_ACCEPT",
    "ASSIGN_ACCEPT",
    "LESSEQUAL_ACCEPT",
    "GREATEREQUAL_ACCEPT",
    "ID_ACCEPT",
    "NUM_ACCEPT",
    "COMMENT_ACCEPT",
    "ENDFILE",
    "ERROR", // Error state

    "INITIAL", // initial state
    "PLUS",
    "MINUS",
    "STAR",
    "LPAREN",
    "RPAREN",
    "LBRACKET",
    "RBRACKET",
    "LBRACE",
    "RBRACE",
    "COMMA",
    "SEMICOLON",
    "EXCLAMATION",
    "NOTEQUAL",
    "LESS",
    "LESSEQUAL",
    "GREATER",
    "GREATEREQUAL",
    "ASSIGN",
    "EQUAL",
    "NUM",
    "SLASH",
    "COMMENT",
    "PREPARE_EXIT",
    "END_COMMENT",
    "I",
    "IF",
    "IN",
    "INT",
    "ID",
    "E",
    "EL",
    "ELS",
    "ELSE",
    "R",
    "RE",
    "RET",
    "RETU",
    "RETUR",
    "RETURN",
    "V",
    "VO",
    "VOI",
    "VOID",
    "W",
    "WH",
    "WHI",
    "WHIL",
    "WHILE"
};

// Finite state machine for lexical analysis
enum State {
    ELSE_ACCEPT,
    IF_ACCEPT,
    INT_ACCEPT,
    RETURN_ACCEPT,
    VOID_ACCEPT,
    WHILE_ACCEPT,
    PLUS_ACCEPT,
    MINUS_ACCEPT,
    STAR_ACCEPT,
    SLASH_ACCEPT,
    LPAREN_ACCEPT,
    RPAREN_ACCEPT,
    LBRACKET_ACCEPT,
    RBRACKET_ACCEPT,
    LBRACE_ACCEPT,
    RBRACE_ACCEPT,
    COMMA_ACCEPT,
    SEMICOLON_ACCEPT,
    LESS_ACCEPT,
    GREATER_ACCEPT,
    EQUAL_ACCEPT,
    NOTEQUAL_ACCEPT,
    ASSIGN_ACCEPT,
    LESSEQUAL_ACCEPT,
    GREATEREQUAL_ACCEPT,
    ID_ACCEPT,
    NUM_ACCEPT,
    COMMENT_ACCEPT,
    ENDFILE,
    ERROR, // Error state

    INITIAL, // initial state
    PLUS,
    MINUS,
    STAR,
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    LBRACE,
    RBRACE,
    COMMA,
    SEMICOLON,
    EXCLAMATION,
    NOTEQUAL,
    LESS,
    LESSEQUAL,
    GREATER,
    GREATEREQUAL,
    ASSIGN,
    EQUAL,
    NUM,
    SLASH,
    COMMENT,
    PREPARE_EXIT,
    END_COMMENT,
    I,
    IF,
    IN,
    INT,
    ID,
    E,
    EL,
    ELS,
    ELSE,
    R,
    RE,
    RET,
    RETU,
    RETUR,
    RETURN,
    V,
    VO,
    VOI,
    VOID,
    W,
    WH,
    WHI,
    WHIL,
    WHILE
};

// Valid characters to go from a transition state to an acceptance state for punctuation and comment tokens
static const std::unordered_set<char> accept_punct = {' ', '\t', '\n', '(', ')', '[', ']', '{', '}', '$', ',', ';',
                                                    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                                    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                                    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

// Valid characters to go from a transition state to an acceptance state for identifier and number tokens
static const std::unordered_set<char> accept_id = {'+', '-', '*', '/', '!', '(', ')', '[', ']', '{', '}', ',', ';', '<', '>', '=', ' ', '\t', '\n', '$'};

// Token struct
struct Token {
    TokenType token; // token type
    std::string tokenString; // token string
    int line; // line number

    Token(TokenType token, char *tokenString, int line) : token(token), tokenString(tokenString), line(line) {}
    Token() : token(TokenType::T_ERROR), tokenString(""), line(0) {}

    void print() {
        if (token == TokenType::T_COMMENT) return;
        printf("Line %d : %s : %s\n", line, token_names[token].c_str(), tokenString.c_str());
    }
};

/**
 * \brief Returns true if the given state is an acceptance state.
 *
 * \param state the state to check
 * \return true if the state is an acceptance state, false otherwise
 */
bool isacceptance(State state);

/**
 * \brief Determines the next state based on the current state and input character.
 *
 * This function implements a finite state machine that processes input characters
 * and transitions between states according to predefined rules. It is used for
 * lexical analysis, transitioning from one state to another based on the character
 * encountered, which can result in acceptance, continuation, or error states.
 *
 * \param state The current state of the lexer.
 * \param c The current character being processed.
 * \return The next state after processing the character.
 */
State get_state(State state, char c);

/**
 * \brief Returns the next token from the input string.
 *
 * This function processes the input string and returns the next token.
 *
 * \param pos The current position in the input string.
 * \param len The length of the input string.
 * \param str The input string.
 * \param line_start The starting position of the current line.
 * \param line The current line number.
 * \param print If true, print the token to the console.
 * \return The next token from the input string.
 */
Token getToken(size_t &pos, size_t len, const char *str, size_t &line_start, size_t &line, bool print = true);

/* PARSER */

// Hash function for the parsing table
struct PairHash {
    size_t operator()(std::pair<std::string,TokenType> const& p) const noexcept {
      static std::hash<std::string>  hs;
      static std::hash<int>          hi;
      return hs(p.first) ^ (hi(int(p.second))<<1);
    }
  };

// The parsing table is a map from a pair of (non-terminal, token) to a vector of strings (productions)
// Each entry in the table represents a production rule for the grammar.
// The first element of the pair is the non-terminal symbol, and the second element is the token type.
// The value is a vector of strings representing the production rule.
// The parsing table is used to determine the next production rule to apply based on the current non-terminal and token.
static std::unordered_map<std::pair<std::string,TokenType>, std::vector<std::string>, PairHash> parsing_table = {
    { {"PROGRAM", T_INT}, {"DECLARATION_LIST", "$"} },
    { {"PROGRAM", T_VOID}, {"DECLARATION_LIST", "$"} },

    { {"DECLARATION_LIST", T_INT}, {"DECLARATION", "DECLARATION_LIST'"} },
    { {"DECLARATION_LIST", T_VOID}, {"DECLARATION", "DECLARATION_LIST'"} },

    { {"DECLARATION_LIST'", T_ENDFILE}, {} },
    { {"DECLARATION_LIST'", T_INT}, {"DECLARATION", "DECLARATION_LIST'"} },
    { {"DECLARATION_LIST'", T_VOID}, {"DECLARATION", "DECLARATION_LIST'"} },

    { {"DECLARATION", T_INT}, {"TYPE_SPECIFIER", "id", "DECLARATION'"} },
    { {"DECLARATION", T_VOID}, {"TYPE_SPECIFIER", "id", "DECLARATION'"} },

    { {"DECLARATION'", T_LPAREN}, {"(", "PARAMS", ")", "COMPOUND_STMT"} },
    { {"DECLARATION'", T_SEMICOLON}, {"VAR_DECLARATION'"} },
    { {"DECLARATION'", T_LBRACKET}, {"VAR_DECLARATION'"} },

    { {"VAR_DECLARATION", T_INT}, {"int", "id", "VAR_DECLARATION'"} },

    { {"VAR_DECLARATION'", T_SEMICOLON}, {";"} },
    { {"VAR_DECLARATION'", T_LBRACKET}, {"[", "num", "]", ";"} },

    { {"TYPE_SPECIFIER", T_INT}, {"int"} },
    { {"TYPE_SPECIFIER", T_VOID}, {"void"} },

    { {"PARAMS", T_INT}, {"PARAM_LIST"} },
    { {"PARAMS", T_VOID}, {"void"} },

    { {"PARAM_LIST", T_INT}, {"PARAM", "PARAM_LIST'"} },

    { {"PARAM_LIST'", T_COMMA}, {",", "PARAM", "PARAM_LIST'"} },
    { {"PARAM_LIST'", T_RPAREN}, {} },

    { {"PARAM", T_INT}, {"int", "id", "PARAM'"} },

    { {"PARAM'", T_LBRACKET}, {"[", "]"} },
    { {"PARAM'", T_COMMA}, {} },
    { {"PARAM'", T_RPAREN}, {} },

    { {"COMPOUND_STMT", T_LBRACE}, {"{", "LOCAL_DECLARATIONS", "STATEMENT_LIST", "}"} },

    { {"LOCAL_DECLARATIONS", T_INT}, {"VAR_DECLARATION", "LOCAL_DECLARATIONS"} },
    { {"LOCAL_DECLARATIONS", T_LBRACE}, {} },
    { {"LOCAL_DECLARATIONS", T_IF}, {} },
    { {"LOCAL_DECLARATIONS", T_WHILE}, {} },
    { {"LOCAL_DECLARATIONS", T_RETURN}, {} },
    { {"LOCAL_DECLARATIONS", T_LPAREN}, {} },
    { {"LOCAL_DECLARATIONS", T_ID}, {} },
    { {"LOCAL_DECLARATIONS", T_NUM}, {} },

    { {"STATEMENT_LIST", T_LBRACE}, {"STATEMENT", "STATEMENT_LIST"} },
    { {"STATEMENT_LIST", T_IF}, {"STATEMENT", "STATEMENT_LIST"} },
    { {"STATEMENT_LIST", T_WHILE}, {"STATEMENT", "STATEMENT_LIST"} },
    { {"STATEMENT_LIST", T_RETURN}, {"STATEMENT", "STATEMENT_LIST"} },
    { {"STATEMENT_LIST", T_LPAREN}, {"STATEMENT", "STATEMENT_LIST"} },
    { {"STATEMENT_LIST", T_ID}, {"STATEMENT", "STATEMENT_LIST"} },
    { {"STATEMENT_LIST", T_NUM}, {"STATEMENT", "STATEMENT_LIST"} },
    { {"STATEMENT_LIST", T_RBRACE}, {} },

    { {"STATEMENT", T_LBRACE}, {"COMPOUND_STMT"} },
    { {"STATEMENT", T_IF}, {"SELECTION_STMT"} },
    { {"STATEMENT", T_WHILE}, {"ITERATION_STMT"} },
    { {"STATEMENT", T_RETURN}, {"RETURN_STMT"} },
    { {"STATEMENT", T_LPAREN}, {"EXPRESSION_STMT"} },
    { {"STATEMENT", T_ID}, {"EXPRESSION_STMT"} },
    { {"STATEMENT", T_NUM}, {"EXPRESSION_STMT"} },

    { {"SELECTION_STMT", T_IF}, {"if", "(", "EXPRESSION", ")", "STATEMENT", "SELECTION_STMT'"} },

    { {"SELECTION_STMT'", T_ELSE}, {"else", "STATEMENT"} },
    { {"SELECTION_STMT'", T_LBRACE}, {} },
    { {"SELECTION_STMT'", T_IF}, {} },
    { {"SELECTION_STMT'", T_WHILE}, {} },
    { {"SELECTION_STMT'", T_RETURN}, {} },
    { {"SELECTION_STMT'", T_LPAREN}, {} },
    { {"SELECTION_STMT'", T_ID}, {} },
    { {"SELECTION_STMT'", T_NUM}, {} },
    { {"SELECTION_STMT'", T_RBRACE}, {} },

    { {"EXPRESSION_STMT", T_LPAREN}, {"EXPRESSION", ";"} },
    { {"EXPRESSION_STMT", T_ID}, {"EXPRESSION", ";"} },
    { {"EXPRESSION_STMT", T_NUM}, {"EXPRESSION", ";"} },

    { {"ITERATION_STMT", T_WHILE}, {"while", "(", "EXPRESSION", ")", "STATEMENT"} },

    { {"RETURN_STMT", T_RETURN}, {"return", "RETURN_STMT'"} },

    { {"RETURN_STMT'", T_SEMICOLON}, {";"} },
    { {"RETURN_STMT'", T_LPAREN}, {"EXPRESSION", ";"} },
    { {"RETURN_STMT'", T_ID}, {"EXPRESSION", ";"} },
    { {"RETURN_STMT'", T_NUM}, {"EXPRESSION", ";"} },

    { {"EXPRESSION", T_LPAREN}, {"ADDITIVE_EXPRESSION", "EXPRESSION'"} },
    { {"EXPRESSION", T_ID}, {"ADDITIVE_EXPRESSION", "EXPRESSION'"} },
    { {"EXPRESSION", T_NUM}, {"ADDITIVE_EXPRESSION", "EXPRESSION'"} },

    { {"EXPRESSION'", T_ASSIGN}, {"=", "EXPRESSION"} },
    { {"EXPRESSION'", T_LESS}, {"RELOP", "ADDITIVE_EXPRESSION"} },
    { {"EXPRESSION'", T_LESSEQUAL}, {"RELOP", "ADDITIVE_EXPRESSION"} },
    { {"EXPRESSION'", T_GREATER}, {"RELOP", "ADDITIVE_EXPRESSION"} },
    { {"EXPRESSION'", T_GREATEREQUAL}, {"RELOP", "ADDITIVE_EXPRESSION"} },
    { {"EXPRESSION'", T_EQUAL}, {"RELOP", "ADDITIVE_EXPRESSION"} },
    { {"EXPRESSION'", T_NOTEQUAL}, {"RELOP", "ADDITIVE_EXPRESSION"} },
    { {"EXPRESSION'", T_RPAREN}, {} },
    { {"EXPRESSION'", T_SEMICOLON}, {} },
    { {"EXPRESSION'", T_RBRACKET}, {} },
    { {"EXPRESSION'", T_COMMA}, {} },

    { {"RELOP", T_LESSEQUAL}, {"<="} },
    { {"RELOP", T_LESS}, {"<"} },
    { {"RELOP", T_GREATER}, {">"} },
    { {"RELOP", T_GREATEREQUAL}, {">="} },
    { {"RELOP", T_EQUAL}, {"=="} },
    { {"RELOP", T_NOTEQUAL}, {"!="} },

    { {"ADDITIVE_EXPRESSION", T_LPAREN}, {"TERM", "ADDITIVE_EXPRESSION'"} },
    { {"ADDITIVE_EXPRESSION", T_ID}, {"TERM", "ADDITIVE_EXPRESSION'"} },
    { {"ADDITIVE_EXPRESSION", T_NUM}, {"TERM", "ADDITIVE_EXPRESSION'"} },

    { {"ADDITIVE_EXPRESSION'", T_PLUS}, {"+", "TERM", "ADDITIVE_EXPRESSION'"} },
    { {"ADDITIVE_EXPRESSION'", T_MINUS}, {"-", "TERM", "ADDITIVE_EXPRESSION'"} },
    { {"ADDITIVE_EXPRESSION'", T_RPAREN}, {} },
    { {"ADDITIVE_EXPRESSION'", T_SEMICOLON}, {} },
    { {"ADDITIVE_EXPRESSION'", T_ASSIGN}, {} },
    { {"ADDITIVE_EXPRESSION'", T_LESSEQUAL}, {} },
    { {"ADDITIVE_EXPRESSION'", T_LESS}, {} },
    { {"ADDITIVE_EXPRESSION'", T_GREATER}, {} },
    { {"ADDITIVE_EXPRESSION'", T_GREATEREQUAL}, {} },
    { {"ADDITIVE_EXPRESSION'", T_EQUAL}, {} },
    { {"ADDITIVE_EXPRESSION'", T_NOTEQUAL}, {} },
    { {"ADDITIVE_EXPRESSION'", T_RBRACKET}, {} },
    { {"ADDITIVE_EXPRESSION'", T_COMMA}, {} },

    { {"ADDOP", T_PLUS}, {"+"} },
    { {"ADDOP", T_MINUS}, {"-"} },

    { {"TERM", T_LPAREN}, {"FACTOR", "TERM'"} },
    { {"TERM", T_ID}, {"FACTOR", "TERM'"} },
    { {"TERM", T_NUM}, {"FACTOR", "TERM'"} },

    { {"TERM'", T_STAR}, {"*", "FACTOR", "TERM'"} },
    { {"TERM'", T_SLASH}, {"/", "FACTOR", "TERM'"} },
    { {"TERM'", T_PLUS}, {} },
    { {"TERM'", T_MINUS}, {} },
    { {"TERM'", T_LESSEQUAL}, {} },
    { {"TERM'", T_LESS}, {} },
    { {"TERM'", T_GREATER}, {} },
    { {"TERM'", T_GREATEREQUAL}, {} },
    { {"TERM'", T_EQUAL}, {} },
    { {"TERM'", T_NOTEQUAL}, {} },
    { {"TERM'", T_SEMICOLON}, {} },
    { {"TERM'", T_RPAREN}, {} },
    { {"TERM'", T_RBRACKET}, {} },
    { {"TERM'", T_COMMA}, {} },
    { {"TERM'", T_ASSIGN}, {} },

    { {"MULOP", T_STAR}, {"*"} },
    { {"MULOP", T_SLASH}, {"/"} },

    { {"FACTOR", T_LPAREN}, {"(", "EXPRESSION", ")"} },
    { {"FACTOR", T_ID}, {"id", "FACTOR'"} },
    { {"FACTOR", T_NUM}, {"num"} },

    { {"FACTOR'", T_LBRACKET}, {"[", "EXPRESSION", "]"} },
    { {"FACTOR'", T_LPAREN}, {"(", "ARGS", ")"} },
    { {"FACTOR'", T_SEMICOLON}, {} },
    { {"FACTOR'", T_PLUS}, {} },
    { {"FACTOR'", T_MINUS}, {} },
    { {"FACTOR'", T_STAR}, {} },
    { {"FACTOR'", T_SLASH}, {} },
    { {"FACTOR'", T_LESSEQUAL}, {} },
    { {"FACTOR'", T_LESS}, {} },
    { {"FACTOR'", T_GREATER}, {} },
    { {"FACTOR'", T_GREATEREQUAL}, {} },
    { {"FACTOR'", T_EQUAL}, {} },
    { {"FACTOR'", T_NOTEQUAL}, {} },
    { {"FACTOR'", T_RPAREN}, {} },
    { {"FACTOR'", T_COMMA}, {} },
    { {"FACTOR'", T_RBRACKET}, {} },
    { {"FACTOR'", T_ASSIGN}, {} },

    { {"ARGS", T_LPAREN}, {"ARG_LIST"} },
    { {"ARGS", T_ID}, {"ARG_LIST"} },
    { {"ARGS", T_NUM}, {"ARG_LIST"} },
    { {"ARGS", T_RPAREN}, {} },

    { {"ARG_LIST", T_LPAREN}, {"EXPRESSION", "ARG_LIST'"} },
    { {"ARG_LIST", T_ID}, {"EXPRESSION", "ARG_LIST'"} },
    { {"ARG_LIST", T_NUM}, {"EXPRESSION", "ARG_LIST'"} },

    { {"ARG_LIST'", T_COMMA}, {",", "EXPRESSION", "ARG_LIST'"} },
    { {"ARG_LIST'", T_RPAREN}, {} }
};

// Abstract Syntax Tree
struct AST {
    std::vector<AST> children; // The children of the AST node
    TokenType type; // The type of the AST node
    std::string value; // The value of the AST node
    int level;

    AST(TokenType type, std::string value, int level) : type(type), value(value), level(level) {}
    AST() : type(TokenType::T_ERROR), value(""), level(0) {}

    /**
     * Prints the AST node in a tree-like structure.
     * Indents according to the level of the node.
     * Prints the type and value of the node.
     * Recursively prints all child nodes.
     */
    void print() {
        for (int i = 0; i < level; i++) printf("    ");
        printf("%s : %s\n", token_names[type].c_str(), value.c_str());

        for (auto &child : children) child.print();
    }
};

// Parser function
AST parser(std::vector<Token> tokens, size_t &current, bool print = true);