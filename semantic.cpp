#include "compiler.hpp"

/**
 * @brief Recursively traverse the AST, adding declarations to the symbol table
 *
 * @param ast The current subtree of the AST
 * @param tbl The symbol table
 */
void traverse(AST &ast, SymbolTable &tbl) {
    // Add declarations to symbol table
    if (ast.value == "DECLARATION") {
        std::string type = ast.children[0].value;
        std::string name = ast.children[1].value;

        tbl.symbols[name] = Symbol(name, type, ast.line);

        if (ast.children[2].value == "(") {
            // Save void in symbol
            if (ast.children.size() > 4 && ast.children[3].value == "void") {
                tbl.symbols[name].args.push_back("void");
            }

            // Create a new scope in the symbol table
            tbl.children.push_back(SymbolTable(name, &tbl));
            for (AST &child : ast.children) {
                traverse(child, tbl.children.back());
            }
            return;

        } else if (ast.children[2].value == "[") {
            tbl.symbols[name].size = std::stoi(ast.children[3].value);
        }
    }
    else if (ast.value == "PARAM") {
        // Save in current table
        std::string type = ast.children[0].value;
        std::string name = ast.children[1].value;

        tbl.symbols[name] = Symbol(name, type, ast.line);

        if (ast.children.size() > 3 && ast.children[2].value == "[") {
            tbl.symbols[name].size = 1;
        }

        // Save in parent table, in the function
        tbl.parent->symbols[tbl.name].args.push_back(name);
    }
    else if (ast.value == "VAR_DECLARATION") {
        std::string type = ast.children[0].value;
        std::string name = ast.children[1].value;

        tbl.symbols[name] = Symbol(name, type, ast.line);

        if (ast.children.size() > 3 && ast.children[2].value == "[") {
            tbl.symbols[name].size = std::stoi(ast.children[3].value);
        }
    }
    else if (ast.value == "ITERATION_STMT" || ast.value == "SELECTION_STMT") {
        std::string name = ast.children[0].value + "_" + tbl.name + "_" + std::to_string(tbl.children.size());

        // Create new scope
        tbl.children.push_back(SymbolTable(name, &tbl));
        
        for (AST &child : ast.children) {
            traverse(child, tbl.children.back());
        }
        return;
    }

    // Traverse children, recursively
    for (AST &child : ast.children) {
        traverse(child, tbl);
    }
}

SymbolTable symbol_table(AST ast, bool print) {
    SymbolTable sym("Global");

    // Add output and input functions to global symbol table
    sym.symbols["output"] = Symbol("output", "void", 0);
    sym.symbols["output"].args.push_back("int");
    sym.symbols["input"] = Symbol("input", "int", 0);
    sym.symbols["input"].args.push_back("void");

    traverse(ast, sym);

    if (print) sym.print();

    return sym;
}

std::string typecheck(AST ast, SymbolTable tbl) {
    if (ast.type == TokenType::T_ID) {
        // Check if the symbol exists in the symbol table
        if (!tbl.exists_in_parent(ast.value)) {
            fprintf(stderr, "Line %d: Undefined symbol: %s\n", ast.line, ast.value.c_str());
            return "";
        }
        return tbl.symbols[ast.value].type;
    }
    if (ast.type == TokenType::T_NUM) {
        return "int";
    }

    // Change scope, when entering a new one
    if ((ast.value == "DECLARATION" && ast.children[2].value == "(") || ast.value == "ITERATION_STMT" || ast.value == "SELECTION_STMT") {
        // Find the new scope in the symbol table
        SymbolTable scope = tbl.get_child(ast.children[1].value);

        if (scope.name == "") {
            fprintf(stderr, "Line %d: Undefined symbol: %s\n", ast.line, ast.children[1].value.c_str());
            return "";
        }

        // Check the type in the new scope
        return typecheck(ast.children.back(), scope);
    }
    if (ast.value == "EXPRESSION") {
        // Check assignment
        if (ast.children[1].value == "=") {
            // Check the type of the left side
            std::string left = typecheck(ast.children[0], tbl);
            std::string right = typecheck(ast.children[2], tbl);

            if (left == "" || right == "") return "";

            if (left != right) {
                fprintf(stderr, "Line %d: Type mismatch: %s != %s\n", ast.line, left.c_str(), right.c_str());
                return "";
            }

            return left;
        }
        
    }
    else if (ast.value == "ADDITIVE_EXPRESSION" || ast.value == "TERM") {
        std::string left = typecheck(ast.children[0], tbl);
        std::string right = typecheck(ast.children[2], tbl);

        if (left == "" || right == "") return "";

        if (left != right) {
            fprintf(stderr, "Line %d: Type mismatch: %s != %s\n", ast.line, left.c_str(), right.c_str());
            return "";
        }

        return left;
    }
    else if (ast.value == "RETURN_STMT") {
        std::string value = typecheck(ast.children[1], tbl);
        std::string ret_type = tbl.parent->symbols[tbl.name].type;

        if (value == "") return "";

        if (value != ret_type) {
            fprintf(stderr, "Line %d: Type mismatch: %s != %s\n", ast.line, value.c_str(), ret_type.c_str());
            return "";
        }

        return value;
    }
    else if (ast.value == "FACTOR") {
        // Check array
        if (ast.children[1].value == "[") {
            Symbol *array = tbl.get_symbol(ast.children[0].value);
            std::string index = typecheck(ast.children[2], tbl);

            if (array == nullptr) {
                fprintf(stderr, "Line %d: Undefined symbol: %s\n", ast.line, ast.children[0].value.c_str());
                return "";
            }

            // Check that the symbol is an array
            if (array->size == 0) {
                fprintf(stderr, "Line %d: Type mismatch: %s != array\n", ast.line, array->type.c_str());
                return "";
            }

            // Check that the index is an integer
            if (index != "int") {
                fprintf(stderr, "Line %d: Type mismatch: %s != int\n", ast.line, index.c_str());
                return "";
            }

            return array->type;
        }
        // Check function call
        else if (ast.children[1].value == "(") {
            Symbol *func = tbl.get_symbol(ast.children[0].value);

            if (func == nullptr) {
                fprintf(stderr, "Line %d: Undefined symbol: %s\n", ast.line, ast.children[0].value.c_str());
                return "";
            }

            // Check that the symbol is a function
            if (func->args.size() == 0) {
                fprintf(stderr, "Line %d: Type mismatch: %s != function\n", ast.line, func->type.c_str());
                return "";
            }

            // Get the number of arguments in factor, removing the id and the parentheses
            size_t num_args = ast.children.size() - 3;

            // Void parameter functions are called with 0 arguments
            if (func->args.size() == 1 && func->args[0] == "void") num_args = 0;

            // Check that the number of arguments is correct
            if (func->args.size() != num_args) {
                fprintf(stderr, "Line %d: Parameter mismatch: %lld != %lld\n", ast.line, func->args.size(), num_args);
                return "";
            }

            // Check that the types of the arguments are correct
            for (size_t i = 0; i < num_args; i++) {
                std::string arg = typecheck(ast.children[i + 2], tbl);
                if (arg != func->args[i]) {
                    fprintf(stderr, "Line %d: Type mismatch: %s != %s\n", ast.line, arg.c_str(), func->args[i].c_str());
                    return "";
                }
            }

            return func->type;
        }
    }

    // In all other cases
    std::string ret = ":)";
    for (AST &child : ast.children) {
        if (typecheck(child, tbl) == "") ret = "";
    }
    return ret;
}

bool semantic_analyzer(AST ast, bool print) {
    SymbolTable tbl = symbol_table(ast, print);

    std::string result = typecheck(ast, tbl);

    if (result == "") return false;

    if (print) printf("Typecheck passed\n");

    return true;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s input_file [-pls] \n\t-p : print parser output\n\t-l : print lexer output\n \t-s : print symbol table\n", argv[0]);
        return 1;
    }

    bool print_lexer = false;
    bool print_parser = false;
    bool print_symbol_table = false;

    if (argc > 2) {
        if (argv[2][0] == '-') {
            for (size_t i = 1; i < strlen(argv[2]); i++) {
                if (argv[2][i] == 'p') {
                    print_parser = true;
                }
                else if (argv[2][i] == 'l') {
                    print_lexer = true;
                }
                else if (argv[2][i] == 's') {
                    print_symbol_table = true;
                }
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
        Token token = getToken(i, len, buffer.c_str(), line_start, line, print_lexer);

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
    AST root = parser(tokens, current, print_parser);

    // Analyze the AST for semantic errors
    bool result = semantic_analyzer(root, print_symbol_table);

    if (result)
        return 0;

    return -1;
}