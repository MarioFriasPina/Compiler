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

/**
 * Recursively traverse the AST and prune it according to the following rules:
 *   * Drop unwanted leaf terminals entirely.
 *   * If this non-terminal is in your drop-set, splice its children in.
 *   * Collapse any node with exactly one child.
 */
void prune(AST *node) {
    // Recurse into children, build a new “kept” list
    std::vector<AST> kept;
    kept.reserve(node->children.size());

    for (auto& child : node->children) {
        prune(&child);

        // Drop unwanted leaf terminals entirely
        if (child.children.empty() && dropTerminals.count(child.type))
            continue;

        // If this non‑terminal is in your drop‑set, splice its children in
        if (dropNonTerminals.count(child.value)) {
            for (auto& grand : child.children) {
                kept.push_back(std::move(grand));
            }
        }
        else
            kept.push_back(std::move(child));
    }

    // Replace the old children list
    node->children = std::move(kept);

    // Collapse any node with exactly one child
    if (node->children.size() == 1) {
        // move the single child up into this node
        AST single = std::move(node->children[0]);
        *node = std::move(single);
    }
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

    // Check if there are any errors
    //std::vector<std::string> errors;

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
                printf("Line %d: unexpected token %s, expected %s\n", current_token.line, token_names[current_token.token].c_str(), top.c_str());
                //errors.push_back("Line " + std::to_string(current_token.line) + ": unexpected token " + token_names[current_token.token] + ", expected " + top);
                return root;
            }
        }
        // Check if the top of the stack is a non-terminal
        else {
            // Get the production rule
            auto entry = parsing_table.find(std::make_pair(top, current_token.token));

            // Check if the production rule exists
            if (entry == parsing_table.end()) {
                std::vector<TokenType> expected;

                // Get the expected tokens from the parsing table
                for (auto &kv : parsing_table) {
                    if (kv.first.first == top) {
                        expected.push_back(kv.first.second);
                    }
                }

                printf("Line %d: unexpected token %s, expected one of: ", current_token.line, token_names[current_token.token].c_str());
                for (size_t i = 0; i < expected.size(); i++) {
                    printf("%s", token_names[expected[i]].c_str());
                    if (i < expected.size() - 1) printf(", ");
                }
                printf("\n");

                /*
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
                */

                return root;
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

    // Prune the tree, to remove unnecessary nodes
    prune(&root);

    if (print)
        root.print();

    return root;
}