#include "compiler.hpp"

// Helper to get the current token
Token peek(const std::vector<Token>& tokens, size_t &current) {
    if (current < tokens.size())
        return tokens[current];
    return tokens.back(); // Return last token if out of bounds
}

// Helper to advance to the next token
Token advance_token(const std::vector<Token>& tokens, size_t &current) {
    if (current < tokens.size())
        current++;
    return peek(tokens, current);
}

// Helper to check if at end
bool isAtEnd(const std::vector<Token>& tokens, size_t &current) {
    return peek(tokens, current).token == TokenType::T_ENDFILE;
}

// Helper to check if a token is a terminal
bool isTerminal(std::string token) {
    return std::find(token_names.begin(), token_names.end(), token) != token_names.end();
}

AST parser(std::vector<Token> tokens, size_t &current, bool print) {
    AST root = AST(TokenType::T_INITIAL, "Program"); // Root of the AST
    std::stack<std::string> stack; // Stack for the parser

    // Push the starting symbols to the stack
    stack.push("$");
    stack.push("PROGRAM");

    Token current_token = peek(tokens, current); // Get the current token

    while (!stack.empty()) {
        std::string top = stack.top(); // Get the top of the stack
        stack.pop(); // Pop the top of the stack

        // Ignore comments
        if (current_token.token == TokenType::T_COMMENT) {
            current_token = advance_token(tokens, current);
        }

        if (top == "$") {
            if (current_token.token == TokenType::T_ENDFILE) {
                return root;
            } else {
                // Error, expected $
                printf("Expected $ at end of file\n");
                return root;
            }
        }

        if (isTerminal(top)) { // Terminal
            if (top == token_names[current_token.token]) {
                advance_token(tokens, current);
                current_token = peek(tokens, current);
            } else {
                // Error, expected terminal
                fprintf(stderr, "Line %d :Expected %s, got %s\n", current_token.line, top.c_str(), token_names[current_token.token].c_str());
                return root;
            }
        } else { // Non-terminal
            auto entry = parsing_table.find(std::make_pair(top, current_token.token));
            if (entry != parsing_table.end()) {
                const std::vector<std::string>& production = entry->second;

                if (production.size() != 0) { // If the production is not empty
                    for (auto it = production.rbegin(); it != production.rend(); it++) {
                        stack.push(*it);
                    }
                }
            } else {
                // Error, expected non-terminal
                fprintf(stderr, "Line %d :%s -> %s not found on parsing table\n", current_token.line, top.c_str(), token_names[current_token.token].c_str());
                return root;
            }
                    
        }
    }

    if (print) {
        root.print();
    }

    return root;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s input_file [-p]\n\t-p: print tokens\n", argv[0]);
        return 1;
    }

    bool toprint = false;

    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-p") == 0) {
                toprint = true;
            }
        }
    }

    std::ifstream file(argv[1]);

    if (!file.is_open()) {
        printf("Failed to open file %s\n", argv[1]);
        return 1;
    }

    // Read the file into a string
    std::string buffer((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

    file.close();

    // Add a $ at the end of the string
    buffer += '$';

    // Length of string
    size_t len = buffer.length();
    // Current position
    size_t i = 0;
    // Current line start
    size_t line_start = 0;
    // Current line
    size_t line = 0;

    // Token vector
    std::vector<Token> tokens;

    // Loop through the string and get tokens
    while (i < len) {
        Token token = getToken(i, len, buffer.c_str(), line_start, line, false);

        // If the token is not an error, update the current position
        if (token.token == TokenType::T_ERROR) {
            // Skip to the end of the line, or the end of the file
            while (buffer[i] != '\n' && buffer[i] != '$')
                ++i;
            // Update the line number
            ++line;
            // Update the line start
            line_start = i;
        }
        else {
            // Add the token to the vector
            tokens.push_back(token);
        }
    }

    size_t current = 0; // Current token index

    // Parse the tokens
    AST root = parser(tokens, current, toprint);

    return 0;
}