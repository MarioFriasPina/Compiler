#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <fstream>
#include <unordered_set>
#include <string>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Token names
const char* tokens[] = {
    "ELSE",
    "IF",
    "INT",
    "RETURN",
    "VOID",
    "WHILE",
    "PLUS",
    "MINUS",
    "STAR",
    "SLASH",
    "LPAREN",
    "RPAREN",
    "LBRACKET",
    "RBRACKET",
    "LBRACE",
    "RBRACE",
    "COMMA",
    "SEMICOLON",
    "LESS",
    "GREATER",
    "EQUAL",
    "NOTEQUAL",
    "ASSIGN",
    "LESSEQUAL",
    "GREATEREQUAL",
    "ID",
    "NUM",
    "COMMENT",
    "ENDFILE",
    "ERROR",
    "INITIAL"
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
    T_SLASH, // /
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
    T_ASSIGN, // =
    T_LESSEQUAL, // <=
    T_GREATEREQUAL, // >=
    T_ID, // identifier
    T_NUM, // number
    T_COMMENT, // /* ... */
    T_ENDFILE, // $
    T_ERROR, // IN CASE OF ERROR
    T_INITIAL
};

// State Names for debugging
const char* state_names[] = {
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

struct Token {
    TokenType token;
    std::string tokenString;

    Token(TokenType token, char *tokenString) : token(token), tokenString(tokenString) {}
    Token() : token(TokenType::T_ERROR), tokenString("") {}

    void print(int line) {
        if (token == TokenType::T_COMMENT) return;
        printf("Line %d : %s : %s\n", line, tokens[token], tokenString.c_str());
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

// Lexical analyzer
Token getToken(size_t &pos, size_t len, const char *str, size_t &line_start, size_t &line, bool print = true);