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

    AST root(TokenType::T_INITIAL, "PROGRAM", -1);

    // Push the starting symbols to the stack
    stack.push("$");
    stack.push("PROGRAM");

    // Push the root node to the stack
    nodeStack.push(&root);

    // Get the first token
    Token current_token = peek(tokens, current);

    // Check if there are any errors
    std::vector<std::string> errors;

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
                current_node->line = current_token.line;
                
                current_token = advance_token(tokens, current);
            } else {
                // Print error
                errors.push_back("Line " + std::to_string(current_token.line) + ": unexpected token " + token_names[current_token.token] + ", expected " + top);
                continue;
            }
        }
        // Check if the top of the stack is a non-terminal
        else {
            // Get the production rule
            auto entry = parsing_table.find(std::make_pair(top, current_token.token));

            if (entry == parsing_table.end()) {
                std::vector<TokenType> expected;

                // Get the expected tokens from the parsing table
                for (auto &kv : parsing_table) {
                    if (kv.first.first == top) {
                        expected.push_back(kv.first.second);
                    }
                }

                errors.push_back("Line " + std::to_string(current_token.line) + ": unexpected token " + token_names[current_token.token] + ", expected one of: ");
                for (size_t i = 0; i < expected.size(); i++) {
                    errors.back() += token_names[expected[i]];
                    if (i < expected.size() - 1) errors.back() += ", ";
                }
                errors.back() += "\n";

                // Skip to the next safe token
                while (current_token.token != TokenType::T_ENDFILE && safe_tokens.count(current_token.token) == 0)
                    current_token = advance_token(tokens, current);

                // Drop the bad tokens
                stack.pop();
                nodeStack.pop();

                continue;
            }

            const std::vector<std::string>& production = entry->second;

            // Create the children of the current node
            for (auto &sym : production) {
                current_node->children.emplace_back(AST(TokenType::T_ERROR, sym, current_token.line));
            }

            // Push to stack
            for (int i = int(production.size()) - 1; i >= 0; i--) {
                stack.push(production[i]);
                nodeStack.push(&current_node->children[i]);
            }             
        }
    }

    if (print) {
        //root.print();
        root.pretty_print();

        if (!errors.empty()) {
            printf(ANSI_COLOR_RED);
            for (auto &error : errors) {
                printf("%s", error.c_str());
            }
            printf(ANSI_COLOR_RESET);
        }
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
    size_t line = 1;

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