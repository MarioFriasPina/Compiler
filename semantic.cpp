#include "compiler.hpp"

/**
 * @brief Recursively traverse the AST, adding declarations to the symbol table
 *
 * @param ast The current subtree of the AST
 * @param tbl The symbol table
 */
void traverse(AST &ast, SymbolTable &tbl) {
    bool global = tbl.name == "Global" ? true : false;

    // Add declarations to symbol table
    if (ast.value == "DECLARATION") {
        std::string type = ast.children[0].value;
        std::string name = ast.children[1].value;

        tbl.symbols[name] = Symbol(name, type, ast.line, tbl.current_offset, global);

        if (ast.children.size() > 3 && ast.children[2].value == "(") {
            // Save void in symbol
            if (ast.children.size() > 4 && ast.children[3].value == "void") {
                tbl.symbols[name].args.push_back("void");
            }

            // Create a new scope in the symbol table
            tbl.children.push_back(SymbolTable(name, &tbl, type));

            for (AST &child : ast.children) {
                traverse(child, tbl.children.back());
            }
            return;

        } else if (ast.children.size() > 3 && ast.children[2].value == "[") {
            tbl.symbols[name].size = std::stoi(ast.children[3].value);
            tbl.current_offset += tbl.symbols[name].size * 4;
        }
        else
            tbl.current_offset += 4;
    }
    else if (ast.value == "PARAM") {
        // Save in current table
        std::string type = ast.children[0].value;
        std::string name = ast.children[1].value;

        tbl.symbols[name] = Symbol(name, type, ast.line, tbl.current_offset, global);
        tbl.symbols[name].param = true;

        if (ast.children.size() > 3 && ast.children[2].value == "[") {
            tbl.symbols[name].size = 1;
            tbl.current_offset += tbl.symbols[name].size * 4;
        }
        else
            tbl.current_offset += 4;

        // Save in parent table, in the function
        tbl.parent->symbols[tbl.name].args.push_back(type);
    }
    else if (ast.value == "VAR_DECLARATION") {
        std::string type = ast.children[0].value;
        std::string name = ast.children[1].value;

        tbl.symbols[name] = Symbol(name, type, ast.line, tbl.current_offset, global);

        if (ast.children.size() > 3 && ast.children[2].value == "[") {
            tbl.symbols[name].size = std::stoi(ast.children[3].value);
            tbl.current_offset += tbl.symbols[name].size * 4;
        }
        else
            tbl.current_offset += 4;
    }
    else if (ast.value == "ITERATION_STMT" || ast.value == "SELECTION_STMT") {
        std::string name = ast.children[0].value + "_line_" + std::to_string(ast.line);
        // Create a new scope in the symbol table
        tbl.children.push_back(SymbolTable(name, &tbl, tbl.return_type));

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

void correct_parentage(SymbolTable &tbl) {
    for (SymbolTable &child : tbl.children) {
        child.parent = &tbl;
        correct_parentage(child);
    }
}

SymbolTable symbol_table(AST ast, bool print) {
    SymbolTable sym("Global", NULL, "void");

    // Add output and input functions to global symbol table
    sym.symbols["output"] = Symbol("output", "void", 0, 0, true);
    sym.symbols["output"].args.push_back("int");
    sym.symbols["input"] = Symbol("input", "int", 0, 0, true);
    sym.symbols["input"].args.push_back("void");

    traverse(ast, sym);

    // Correct parentage
    correct_parentage(sym);

    if (print) sym.print();

    return sym;
}

std::string typecheck(AST ast, SymbolTable &tbl) {
    if (ast.type == TokenType::T_ID) {
        Symbol *symbol = tbl.get_symbol(ast.value);
        if (symbol == NULL) {
            fprintf(stderr, ANSI_COLOR_RED "Line %d: Undefined variable %s\n" ANSI_COLOR_RESET, ast.line, ast.value.c_str());
            g_errors = true;
            return "";
        }
        return symbol->type;
    }

    if (ast.type == TokenType::T_NUM) {
        return "int";
    }

    // Change scope, when entering a new one
    if (ast.value == "DECLARATION" && ast.children.size() > 3 && ast.children[2].value == "(")
        return typecheck(ast.children.back(), tbl.children[tbl.current_child++]);

    if (ast.value == "ITERATION_STMT" || ast.value == "SELECTION_STMT") {
        SymbolTable scope = tbl.children[tbl.current_child++];

        // Check the content inside the expression
        std::string expression = typecheck(ast.children[2], scope);

        // Check the type in the new scope
        return typecheck(ast.children.back(), scope);
    }

    if (ast.value == "EXPRESSION") {
        std::string left = typecheck(ast.children[0], tbl);
        std::string right = typecheck(ast.children[2], tbl);

        if (left == "" || right == "") return "";

        if (left != right) {
            fprintf(stderr, ANSI_COLOR_RED "Line %d: Type mismatch in expression: %s != %s\n" ANSI_COLOR_RESET, ast.line, left.c_str(), right.c_str());
            g_errors = true;
            return "";
        }

        return left;
    }
    if (ast.value == "ADDITIVE_EXPRESSION" || ast.value == "TERM") {
        std::string left = typecheck(ast.children[0], tbl);
        std::string right = typecheck(ast.children[2], tbl);

        if (left == "" || right == "") return "";

        if (left != right) {
            fprintf(stderr, ANSI_COLOR_RED "Line %d: Type mismatch in math operation: %s != %s\n" ANSI_COLOR_RESET, ast.line, left.c_str(), right.c_str());
            g_errors = true;
            return "";
        }

        return left;
    }
    if (ast.value == "RETURN_STMT") {
        std::string value = typecheck(ast.children[1], tbl);

        if (value == "") return "";

        if (value != tbl.return_type) {
            fprintf(stderr, ANSI_COLOR_RED "Line %d: Type mismatch in return statement: %s != %s\n" ANSI_COLOR_RESET, ast.line, value.c_str(), tbl.return_type.c_str());
            g_errors = true;
            return "";
        }

        return value;
    }
    if (ast.value == "FACTOR") {
        // Check array
        if (ast.children[1].value == "[") {
            Symbol *array = tbl.get_symbol(ast.children[0].value);
            std::string index = typecheck(ast.children[2], tbl);

            if (array == NULL) {
                fprintf(stderr, ANSI_COLOR_RED "Line %d: Undefined variable %s[]\n" ANSI_COLOR_RESET, ast.line, ast.children[0].value.c_str());
                g_errors = true;
                return "";
            }

            // Check that the symbol is an array
            if (array->size == 0) {
                fprintf(stderr, ANSI_COLOR_RED "Line %d: Type mismatch in array: %s != array\n" ANSI_COLOR_RESET, ast.line, array->type.c_str());
                g_errors = true;
                return "";
            }

            // Check that the index is an integer
            if (index != "int") {
                fprintf(stderr, ANSI_COLOR_RED "Line %d: Type mismatch in index: %s != int\n" ANSI_COLOR_RESET, ast.line, index.c_str());
                g_errors = true;
                return "";
            }

            return array->type;
        }
        // Check function call
        else if (ast.children[1].value == "(") {
            Symbol *func = tbl.get_symbol(ast.children[0].value);

            if (func == nullptr) {
                fprintf(stderr, ANSI_COLOR_RED "Line %d: Undefined function %s()\n" ANSI_COLOR_RESET, ast.line, ast.children[0].value.c_str());
                g_errors = true;
                return "";
            }

            // Check that the symbol is a function
            if (func->args.size() == 0) {
                fprintf(stderr, ANSI_COLOR_RED "Line %d: Type mismatch in function: %s != function\n" ANSI_COLOR_RESET, ast.line, func->type.c_str());
                return "";
            }

            // Get the number of arguments in factor, removing the id and the parentheses
            size_t num_args = ast.children.size() - 3;

            // Void parameter functions are called with 0 arguments
            if (func->args.size() == 1 && func->args[0] == "void") {
                if (num_args != 0) {
                    fprintf(stderr, ANSI_COLOR_RED "Line %d: Parameter mismatch : %lld != %lld\n" ANSI_COLOR_RESET, ast.line, func->args.size(), num_args);
                    return "";
                }
                return func->type;
            }

            // Check that the number of arguments is correct
            if (func->args.size() != num_args) {
                fprintf(stderr, ANSI_COLOR_RED "Line %d: Parameter number mismatch: %lld != %lld\n" ANSI_COLOR_RESET, ast.line, func->args.size(), num_args);
                return "";
            }

            // Check that the types of the arguments are correct
            for (size_t i = 0; i < num_args; i++) {
                std::string arg = typecheck(ast.children[i + 2], tbl);

                if (arg != func->args[i]) {
                    fprintf(stderr, ANSI_COLOR_RED "Line %d: Type mismatch in argument: %s != %s\n" ANSI_COLOR_RESET, ast.line, arg.c_str(), func->args[i].c_str());
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

SymbolTable semantic_analyzer(AST ast, bool print) {

    std::stack<SymbolTable> tbl_stack;

    SymbolTable tbl = symbol_table(ast, print);

    // Check that the global scope has a main function
    if (tbl.get_symbol("main") == NULL) {
        fprintf(stderr, ANSI_COLOR_RED "Line %d: Undefined function main()\n" ANSI_COLOR_RESET, ast.line);
        g_errors = true;
    }

    std::string result = typecheck(ast, tbl);

    if (result == "") {
        fprintf(stderr, ANSI_COLOR_RED "Typecheck failed\n" ANSI_COLOR_RESET);
        g_errors = true;
    }

    if (print) printf("Typecheck passed\n");

    return tbl;
}