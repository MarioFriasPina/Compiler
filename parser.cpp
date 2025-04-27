#include "compiler.hpp"

/* Peek at the next token */
Token peek(const std::vector<Token>& tokens, size_t &current) {
    if (current < tokens.size())
        return tokens[current];
    return tokens.back(); // Return last token if out of bounds
}

/* Advance to the next token */
Token advance_token(const std::vector<Token>& tokens, size_t &current) {
    if (current < tokens.size())
        current++;
    return peek(tokens, current);
}

/* Check if a token is a terminal */
bool isTerminal(std::string token) {
    return std::find(token_names.begin(), token_names.end(), token) != token_names.end();
}

AST parser(std::vector<Token> tokens, size_t &current, bool print) {
    std::stack<AST *> nodeStack; // Stack for the tree
    std::stack<std::string> stack; // Stack for the parser

    AST root(TokenType::T_INITIAL, "PROGRAM", 0);

    // Push the starting symbols to the stack
    stack.push("$");
    stack.push("PROGRAM");

    // Push the root node to the stack
    nodeStack.push(&root);

    // Get the first token
    Token current_token = peek(tokens, current); 

    while (!stack.empty()) {
        std::string top = stack.top(); stack.pop();

        AST *current_node = nodeStack.top(); nodeStack.pop();

        // Check if at end
        if (top == "$") break;

        // Ignore comments
        if (current_token.token == TokenType::T_COMMENT) {
            current_token = advance_token(tokens, current);
        }

        // Check if the top of the stack is a terminal
        if (isTerminal(top)) {
            // Check if the top of the stack is the current token
            if (top == token_names[current_token.token]) {

                // Fill current node with the token info
                current_node->type = current_token.token;
                current_node->value = current_token.tokenString;
                
                advance_token(tokens, current);
                current_token = peek(tokens, current);
            } else {
                // Error, expected terminal
                fprintf(stderr, "Line %d : Expected %s, got %s\n", current_token.line, top.c_str(), token_names[current_token.token].c_str());
                return AST(TokenType::T_ERROR, "", 0);
            }
        }
        // Check if the top of the stack is a non-terminal
        else {
            // Get the production rule
            auto entry = parsing_table.find(std::make_pair(top, current_token.token));

            if (entry == parsing_table.end()) {
                // Error, no production rule found for the current token and non-terminal
                fprintf(stderr, "Line %d : %s -> %s not found on parsing table\n", current_token.line, top.c_str(), token_names[current_token.token].c_str());
                return AST(TokenType::T_ERROR, "", 0);
            }

            const std::vector<std::string>& production = entry->second;

            // Create the children of the current node
            for (auto &sym : production) {
                current_node->children.emplace_back(AST(TokenType::T_ERROR, sym, current_node->level + 1));
            }

            // Push to stack
            for (int i = int(production.size()) - 1; i >= 0; i--) {
                stack.push(production[i]);
                nodeStack.push(&current_node->children[i]);
            }             
        }
    }

    if (print) root.print();

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