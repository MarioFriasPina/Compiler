#include "compiler.hpp"

bool g_errors = false;

void body_gen(AST ast, AST content, SymbolTable &tbl, std::ofstream &file) {
    if (content.value == "EXPRESSION") {
        // Assignment
        if (content.children[1].value == "=") {
            factor_gen(content.children[2], ast, tbl, file);          
            // Save the value in the correct position of the stack
            file << "\tsw $a0 " + std::to_string(tbl.get_symbol(content.children[0].value)->offset) + "($fp) # Save $a0 as the new value of " + content.children[0].value + " in the stack\n";
        }
    }
    else if (content.value == "FACTOR") {
        factor_gen(content, ast, tbl, file);
    }
    else if (content.value == "SELECTION_STMT") {
        if_gen(content, tbl, file);
    }
    else if (content.value == "RETURN_STMT") {
        factor_gen(content.children[1], ast, tbl, file);
    }
}

void call_func(AST ast, SymbolTable &tbl, std::ofstream &file) {
    std::string name = ast.children[0].value;
    // Save fp
    file << "\t\t # Call function " + name + "\n";
    file << "\tsw $fp 0($sp) # Save frame pointer in the stack\n";
    file << "\taddiu $sp $sp -4 # Allocate space in the stack\n";

    // Special cases for input and output
    if (name == "output") {
        // Generate the parameter
        if (ast.children[2].type == TokenType::T_NUM)
            file << "\tli $a0 " + ast.children[2].value + " # Load constant into $a0\n";
        else if (ast.children[2].type == TokenType::T_ID && tbl.get_symbol(ast.children[2].value)->args.empty())
            file << "\tlw $a0 " + std::to_string(tbl.get_symbol(ast.children[2].value)->offset) + "($fp) # Load the value of " + ast.children[2].value + "from the stack\n";
        else
            factor_gen(ast.children[2], ast, tbl, file);

        file << "\tsw $a0 0($sp) # Save the parameter in the stack\n";
        file << "\taddiu $sp $sp -4 # Allocate space in the stack\n";

        // Call the output function
        file << "\tjal output_entry\n";
        return;
    }
    else if (name == "input") {
        // Call the input function
        file << "\tjal input_entry\n";
        return;
    }

    // Generate parameters for function 
    SymbolTable *func = tbl.get_function(name);
    int z = 4;
    int i = 2;
    for (auto &sym : func->symbols) {
        if (ast.children[i].type == TokenType::T_NUM) // If the parameter is a constant
            file << "\tli $a0 " + ast.children[i].value + " # Load constant into $a0\n";
        else if (ast.children[i].type == TokenType::T_ERROR) // If the parameter is a non-terminal
            factor_gen(ast.children[i], ast, tbl, file);
        else if (tbl.get_symbol(ast.children[i].value)->args.empty()) // If the parameter is a variable
            file << "\tlw $a0 " + std::to_string(z) + "($fp) # Load the value of " + ast.children[i].value + " from the stack\n";
        else // If the parameter is a function
            call_func(ast.children[i], tbl, file);

        file << "\tsw $a0 0($sp) # Save the parameter in the stack\n";
        file << "\taddiu $sp $sp -4 # Allocate space in the stack\n";

        sym.second.offset = z;
        z += 4;
        i++;
    }

    // Return
    file << "\tjal " + name + "_entry\n";
}

void factor_gen(AST ast, AST parent, SymbolTable &tbl, std::ofstream &file) {
    if (ast.value == "FACTOR") {
        factor_gen(ast.children[0], ast, tbl, file);
    }
    if (ast.type == TokenType::T_ID) {
        if (tbl.get_symbol(ast.value)->args.empty())
            file << "\tlw $a0 " + std::to_string(tbl.get_symbol(ast.value)->offset) + "($fp) # Load the value of " + ast.value + " from the stack\n";
        else
            call_func(parent, tbl, file);
    }
    else if (ast.type == TokenType::T_NUM) {
        file << "\tli $a0 " + ast.value + " # Load constant into $a0\n";
    }

    // Generate the rest of the expression
    for (size_t i = 1; i < ast.children.size(); i += 2) {
        if (ast.children[i].type == TokenType::T_PLUS) {
            factor_gen(ast.children[i - 1], ast, tbl, file);
            file << "\tsw $a0 0($sp)\n\taddiu $sp $sp -4\n";
            factor_gen(ast.children[i + 1], ast, tbl, file);
            file << "\tlw $t1 4($sp)\n\tadd $a0 $t1 $a0 # ADD\n\taddiu $sp $sp 4\n";
        }
        else if (ast.children[i].type == TokenType::T_MINUS) {
            factor_gen(ast.children[i - 1], ast, tbl, file);
            file << "\tsw $a0 0($sp)\n\taddiu $sp $sp -4\n";
            factor_gen(ast.children[i + 1], ast, tbl, file);
            file << "\tlw $t1 4($sp)\n\tsub $a0 $t1 $a0 # SUBSTRACT\n\taddiu $sp $sp 4\n";
        }
        else if (ast.children[i].type == TokenType::T_STAR) {
            factor_gen(ast.children[i - 1], ast, tbl, file);
            file << "\tsw $a0 0($sp)\n\taddiu $sp $sp -4\n";
            factor_gen(ast.children[i + 1], ast, tbl, file);
            file << "\tlw $t1 4($sp)\n\tmul $a0 $t1 $a0 # MULTIPLY\n\taddiu $sp $sp 4\n";
        }
        else if (ast.children[i].type == TokenType::T_SLASH) {
            file << "\t# DIVIDE\n";
            factor_gen(ast.children[i - 1], ast, tbl, file);
            file << "\tsw $a0 0($sp)\n\taddiu $sp $sp -4\n";
            factor_gen(ast.children[i + 1], ast, tbl, file);
            file << "\tlw $t1 4($sp)\n\tdiv $a0 $t1 $a0 # DIVIDE\n\taddiu $sp $sp 4\n";
        }
        else if (ast.children[i].type == TokenType::T_EQUAL) {
            factor_gen(ast.children[i - 1], ast, tbl, file);
            file << "\tsw $a0 0($sp)\n\taddiu $sp $sp -4\n";
            factor_gen(ast.children[i + 1], ast, tbl, file);
            file << "\tlw $t1 4($sp)\n\tseq $a0 $t1 $a0 # EQUAL\n\taddiu $sp $sp 4\n";
        }
        else if (ast.children[i].type == TokenType::T_NOTEQUAL) {
            factor_gen(ast.children[i - 1], ast, tbl, file);
            file << "\tsw $a0 0($sp)\n\taddiu $sp $sp -4\n";
            factor_gen(ast.children[i + 1], ast, tbl, file);
            file << "\tlw $t1 4($sp)\n\tsne $a0 $t1 $a0 # NOT EQUAL\n\taddiu $sp $sp 4\n";
        }
        else if (ast.children[i].type == TokenType::T_GREATER) {
            factor_gen(ast.children[i - 1], ast, tbl, file);
            file << "\tsw $a0 0($sp)\n\taddiu $sp $sp -4\n";
            factor_gen(ast.children[i + 1], ast, tbl, file);
            file << "\tlw $t1 4($sp)\n\tslt $a0 $t1 $a0 # GREATER\n\taddiu $sp $sp 4\n";
        }
        else if (ast.children[i].type == TokenType::T_GREATEREQUAL) {
            factor_gen(ast.children[i - 1], ast, tbl, file);
            file << "\tsw $a0 0($sp)\n\taddiu $sp $sp -4\n";
            factor_gen(ast.children[i + 1], ast, tbl, file);
            file << "\tlw $t1 4($sp)\n\tsle $a0 $t1 $a0 # GREATER OR EQUAL\n\taddiu $sp $sp 4\n";
        }
        else if (ast.children[i].type == TokenType::T_LESS) {
            factor_gen(ast.children[i - 1], ast, tbl, file);
            file << "\tsw $a0 0($sp)\n\taddiu $sp $sp -4\n";
            factor_gen(ast.children[i + 1], ast, tbl, file);
            file << "\tlw $t1 4($sp)\n\tslt $a0 $a0 $t1 # LESS\n\taddiu $sp $sp 4\n";
        }
        else if (ast.children[i].type == TokenType::T_LESSEQUAL) {
            factor_gen(ast.children[i - 1], ast, tbl, file);
            file << "\tsw $a0 0($sp)\n\taddiu $sp $sp -4\n";
            factor_gen(ast.children[i + 1], ast, tbl, file);
            file << "\tlw $t1 4($sp)\n\tsle $a0 $a0 $t1 # LESS OR EQUAL\n\taddiu $sp $sp 4\n";
        }
    }
}

void if_gen(AST ast, SymbolTable &tbl, std::ofstream &file) {
    tbl.children[tbl.current_child].current_child = 0;
    std::string name = tbl.children[tbl.current_child++].name;
    // Check the condition of the if statement
    factor_gen(ast.children[2], ast, tbl, file);
    file << "\tbeq $a0 1 true_" + name + " # Branch if condition is true\n";

    // Generate the false branch in case there is an else 
    file << "false_" + name + ":\n";

    // Do the content of the else
    if (ast.children.size() == 7) {
        body_gen(ast, ast.children[6], tbl, file);
    }

    file << "\tb end_" + name + " # Branch to the end of the if statement\n";

    // Generate the true branch
    file << "true_" + name + ":\n";

    // Do the content of the if
    body_gen(ast, ast.children[4], tbl, file);

    file << "end_" + name + ":\n";
}

void funcgen(AST ast, SymbolTable &tbl, std::ofstream &file) {
    // Generate the entry point for each function
    file << tbl.name + "_entry:\n";

    file << "\tmove $fp $sp # Save frame pointer\n";
    file << "\tsw $ra 0($sp) # Save return address\n";
    file << "\taddiu $sp $sp -4 # Allocate space in the stack\n";

    // Generate the body of the function 
    for (auto &content : ast.children) {
        body_gen(ast, content, tbl, file);
    }

    // Exit the function
    file << "\tlw $ra 4($sp) # Restore Return address\n";
    file << "\taddiu $sp $sp " + std::to_string(4 * tbl.symbols.size() + 8) + " # Restore stack pointer to the last function\n";
    file << "\tlw $fp 0($sp) # Restore frame pointer to the last function\n";
    file << "\tjr $ra\n";
}

void codegen(AST ast, SymbolTable &tbl, std::ofstream &file) {
    // Start by generating the structure of the program
    file << "\t.globl main\n";

    // Get the global variables from the SymbolTable
    file << "\t.data\n";
    for (auto &var : tbl.symbols) {
        // If the variable is an array, generate a .space directive
        if (var.second.size != 0) {
            file << "\t\t" << var.first << ": .space " << var.second.size * 4 << "\n";
        }
        // If the variable is a normal int, generate a .word directive
        else if (var.second.args.empty() && var.second.type == "int") {
            file << "\t\t" << var.first << ": .word 0\n";
        }
    }

    file << "\t.text\n";

    // Generate the entrance to the program
    file << "main:\n";

    // Generate the call for the main function
    file << "\tsw $fp 0($sp) # Save frame pointer in the stack\n";
    file << "\taddiu $sp $sp -4 # Allocate space in the stack\n";

    // Generate the parameters for the main function
    SymbolTable *func = tbl.get_function("main");
    int z = 4;
    for (auto &var : func->symbols) {
        file << "\tli $a0 0 # Set the parameter to an initial value of 0\n";

        file << "\tsw $a0 0($sp) # Save the parameter in the stack\n";
        file << "\taddiu $sp $sp -4 # Allocate space in the stack\n";

        var.second.offset = z;
        z += 4;
    }

    file << "\tjal main_entry\n";

    // Exit the program correctly
    file << "\tli $v0, 10\n\tsyscall\n";

    // INPUT
    file << "input_entry:\n\tmove $fp $sp\n\tsw $ra 0($sp)\n\taddiu $sp $sp -4\n";

    file << "\tli $v0, 5 # Read an integer\n\tsyscall\n";

    file << "\tmove $a0 $v0 # Save the input in $a0\n";

    file << "\tlw $ra 4($sp)\n\taddiu $sp $sp 8\n\tlw $fp 0($sp)\n\tjr $ra\n";
    
    // OUTPUT
    file << "output_entry:\n\tmove $fp $sp\n\tsw $ra 0($sp)\n\taddiu $sp $sp -4\n";

    file << "\tlw $a0 8($sp) # Load the parameter to $a0\n";

    file << "\tli $v0, 1 # Print the parameter\n\tsyscall\n";

    file << "\tlw $ra 4($sp)\n\taddiu $sp $sp 12\n\tlw $fp 0($sp)\n\tjr $ra\n";

    tbl.current_child = 0;

    // Generate the functions
    for (auto &dec : ast.children)
        if (dec.children.size() > 1 && dec.children.back().value == "COMPOUND_STMT") {
            tbl.children[tbl.current_child].current_child = 0;
            // Generate the function
            funcgen(dec.children.back(), tbl.children[tbl.current_child++], file);
        }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s input_file [output_file] [-pls] \n\t-p : print parser output\n\t-l : print lexer output\n \t-s : print symbol table\n", argv[0]);
        return 1;
    }

    bool print_lexer = false;
    bool print_parser = false;
    bool print_symbol_table = false;

    std::string output_file = "";

    if (argc > 2) {
        int options = 0;

        // Find the options
        for (int i = 2; i < argc; i++) {
            if (argv[i][0] == '-') {
                options = i;
                break;
            }
            else {
                output_file = argv[i];
            }
        }

        // Parse the options
        if (options != 0)
            for (size_t i = 1; i < strlen(argv[options]); i++) {
                if (argv[options][i] == 'p')        print_parser = true;
                else if (argv[options][i] == 'l')   print_lexer = true;
                else if (argv[options][i] == 's')   print_symbol_table = true;
            }
        
    }

    std::ifstream file(argv[1]);

    if (!file.is_open()) {
        fprintf(stderr, ANSI_COLOR_RED"Failed to open file %s\n" ANSI_COLOR_RESET, argv[1]);
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
    SymbolTable table = semantic_analyzer(root, print_symbol_table);

    if (g_errors) {
        fprintf(stderr, ANSI_COLOR_RED "There were some errors in the C- code\n" ANSI_COLOR_RESET);
        return 1;
    }

    if (output_file == "") {
        // Get the input file name without the path and extension, and append .asm at the end and Outputs at the beginning
        output_file = argv[1];
        output_file = output_file.substr(output_file.find_last_of("/\\") + 1);
        output_file = output_file.substr(0, output_file.find_last_of("."));
        output_file = "Outputs/" + output_file + ".asm";
    }

    // Create and open the output file for writing
    std::ofstream output(output_file, std::ios::out | std::ios::trunc);

    if (!output.is_open()) {
        fprintf(stderr, ANSI_COLOR_RED "Failed to open file %s\n" ANSI_COLOR_RESET, output_file.c_str());
        return 1;
    }

    // Codegen the AST
    codegen(root, table, output);

    return 0;
}