#include "compiler.hpp"

bool isacceptance(State state) {
    if (state == State::ENDFILE || (state >= State::ELSE_ACCEPT && state <= State::COMMENT_ACCEPT))
        return true;
    return false;
}

State get_state(State state, char c) {
    switch (state)
    {
        case State::INITIAL:
            switch (c) {
                case ' ': case '\t': case '\n':
                    state = State::INITIAL;
                    break;
                case '+':
                    state = State::PLUS;
                    break;
                case '-':
                    state = State::MINUS;
                    break;
                case '*':
                    state = State::STAR;
                    break;
                case '(':
                    state = State::LPAREN;
                    break;
                case ')':
                    state = State::RPAREN;
                    break;
                case '[':
                    state = State::LBRACKET;
                    break;
                case ']':
                    state = State::RBRACKET;
                    break;
                case '{':
                    state = State::LBRACE;
                    break;
                case '}':
                    state = State::RBRACE;
                    break;
                case ',':
                    state = State::COMMA;
                    break;
                case ';':
                    state = State::SEMICOLON;
                    break;
                case '!':
                    state = State::EXCLAMATION;
                    break;
                case '<':
                    state = State::LESS;
                    break;
                case '>':
                    state = State::GREATER;
                    break;
                case '=':
                    state = State::ASSIGN;
                    break;
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    state = State::NUM;
                    break;
                case '/':
                    state = State::SLASH;
                    break;
                case 'i':
                    state = State::I;
                    break;
                case 'e':
                    state = State::E;
                    break;
                case 'r':
                    state = State::R;
                    break;
                case 'v':
                    state = State::V;
                    break;
                case 'w':
                    state = State::W;
                    break;
                case 'a': case 'b': case 'c': case 'd': case 'f': case 'g': case 'h': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 's': case 't': case 'u': case 'x': case 'y': case 'z': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
                    state = State::ID;
                    break;
                case '$':
                    state = State::ENDFILE;
                    break;
                default:
                    fprintf(stderr, "Invalid character: %d\n", c);
                    return State::ERROR;
                    break;

            }
            break;
        case State::PLUS:
            state = accept_punct.contains(c) ? State::PLUS_ACCEPT : State::ERROR;
            break;
        case State::MINUS:
            state = accept_punct.contains(c) ? State::MINUS_ACCEPT : State::ERROR;
            break;
        case State::STAR:
            state = accept_punct.contains(c) ? State::STAR_ACCEPT : State::ERROR;
            break;
        case State::LPAREN:
            state = accept_punct.contains(c) ? State::LPAREN_ACCEPT : State::ERROR;
            break;
        case State::RPAREN:
            state = accept_punct.contains(c) ? State::RPAREN_ACCEPT : State::ERROR;
            break;
        case State::LBRACKET:
            state = accept_punct.contains(c) ? State::LBRACKET_ACCEPT : State::ERROR;
            break;
        case State::RBRACKET:
            state = accept_punct.contains(c) ? State::RBRACKET_ACCEPT : State::ERROR;
            break;
        case State::LBRACE:
            state = accept_punct.contains(c) ? State::LBRACE_ACCEPT : State::ERROR;
            break;
        case State::RBRACE:
            state = accept_punct.contains(c) ? State::RBRACE_ACCEPT : State::ERROR;
            break;
        case State::COMMA:
            state = accept_punct.contains(c) ? State::COMMA_ACCEPT : State::ERROR;
            break;
        case State::SEMICOLON:
            state = accept_punct.contains(c) ? State::SEMICOLON_ACCEPT : State::ERROR;
            break;
        case State::LESS:
            if (c == '=')
                state = State::LESSEQUAL;
            else
                state = accept_punct.contains(c) ? State::LESS_ACCEPT : State::ERROR;
            break;
        case State::GREATER:
            if (c == '=')
                state = State::GREATEREQUAL;
            else
                state = accept_punct.contains(c) ? State::GREATER_ACCEPT : State::ERROR;
            break;
        case State::ASSIGN:
            if (c == '=')
                state = State::EQUAL;
            else
                state = accept_punct.contains(c) ? State::ASSIGN_ACCEPT : State::ERROR;
            break;
        case State::LESSEQUAL:
            state = accept_punct.contains(c) ? State::LESSEQUAL_ACCEPT : State::ERROR;
            break;
        case State::GREATEREQUAL:
            state = accept_punct.contains(c) ? State::GREATEREQUAL_ACCEPT : State::ERROR;
            break;
        case State::EQUAL:
            state = accept_punct.contains(c) ? State::EQUAL_ACCEPT : State::ERROR;
            break;
        case State::EXCLAMATION:
            state = c == '=' ? State::NOTEQUAL : State::ERROR;
            break;
        case State::NOTEQUAL:
            state = accept_punct.contains(c) ? State::NOTEQUAL_ACCEPT : State::ERROR;
            break;
        case State::NUM:
            if (isdigit(c))
                state = State::NUM;
            else
                state = accept_id.contains(c) ? State::NUM_ACCEPT : State::ERROR;
            break;
        case State::SLASH:
            if (c == '*')
                state = State::COMMENT;
            else
                state = accept_punct.contains(c) ? State::SLASH_ACCEPT : State::ERROR;
            break;
        case State::COMMENT:
                state = c == '*' ? State::PREPARE_EXIT : State::COMMENT;
            break;
        case State::PREPARE_EXIT:
            if (c == '/')
                state = State::END_COMMENT;
            else
                state = State::COMMENT;
            break;
        case State::END_COMMENT:
            state = accept_punct.contains(c) ? State::COMMENT_ACCEPT : State::ERROR;
            break;
        case State::I:
            if (c == 'f')
                state = State::IF;
            else if (c == 'n')
                state = State::IN;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::IF:
            if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::IF_ACCEPT : State::ERROR;
            break;
        case State::IN:
            if (c == 't')
                state = State::INT;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::INT:
            if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::INT_ACCEPT : State::ERROR;
            break;
        case State::E:
            if (c == 'l')
                state = State::EL;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::EL:
            if (c == 's')
                state = State::ELS;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::ELS:
            if (c == 'e')
                state = State::ELSE;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::ELSE:
            if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ELSE_ACCEPT : State::ERROR;
            break;
        case State::R:
            if (c == 'e')
                state = State::RE;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::RE:
            if (c == 't')
                state = State::RET;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::RET:
            if (c == 'u')
                state = State::RETU;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::RETU:
            if (c == 'r')
                state = State::RETUR;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::RETUR:
            if (c == 'n')
                state = State::RETURN;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::RETURN:
            if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::RETURN_ACCEPT : State::ERROR;
            break;
        case State::V:
            if (c == 'o')
                state = State::VO;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::VO:
            if (c == 'i')
                state = State::VOI;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::VOI:
            if (c == 'd')
                state = State::VOID;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::VOID:
            if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::VOID_ACCEPT : State::ERROR;
            break;
        case State::W:
            if (c == 'h')
                state = State::WH;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::WH:
            if (c == 'i')
                state = State::WHI;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::WHI:
            if (c == 'l')
                state = State::WHIL;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::WHIL:
            if (c == 'e')
                state = State::WHILE;
            else if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        case State::WHILE:
            if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::WHILE_ACCEPT : State::ERROR;
            break;
        case State::ID:
            if (isalpha(c))
                state = State::ID;
            else
                state = accept_id.contains(c) ? State::ID_ACCEPT : State::ERROR;
            break;
        default:
            state = State::ERROR;
            break;
    }
    return state;
}

Token getToken(size_t &pos, size_t len, const char *str, size_t &line_start, size_t &line, bool print) {
    // The current token
    Token token;
    // The current state of the lexer
    State state = State::INITIAL;
    // The initial position of the token
    size_t start_pos = pos;

    // Check if end of file has been reached
    if (str[pos] == '\0' || str[pos] == '$' || pos >= len) {
        token.token = TokenType::T_ENDFILE;
        token.tokenString = "$";
        token.line = line;
        token.column = pos - line_start;
        ++pos;
        if (print)
            token.print();
        return token;
    }

    while (!isacceptance(state)) {
        // Update line
        if (str[pos] == '\n' && State::INITIAL == state) {
            ++line;
            line_start = pos + 1;
        }

        // The possible next state
        State next_state = get_state(state, str[pos]);

        // If the state is the initial state, update the start position
        if (state == State::INITIAL) {
            start_pos = pos;
        }

        // If the next state is an error, print the error
        if (next_state == State::ERROR) {
            fprintf(stderr, ANSI_COLOR_RED "Line %lld: Error on %s formation:\n", line, state_names[state].c_str()); // Print the error

            // Find the end of the current line
            size_t end_pos = pos;
            while (str[end_pos] != '\n' && str[end_pos] != '$' && end_pos < len)
                ++end_pos;

            fprintf(stderr, "%.*s\n", (int)(end_pos - line_start), str + line_start); // Print the current line
            fprintf(stderr, "%*c\n" ANSI_COLOR_RESET, (int)(pos - line_start), '^'); // Print the position of the error with a caret

            g_errors = true;

            token.token = TokenType::T_ERROR;
            token.tokenString = std::string(str + start_pos, pos - start_pos);
            return token;
        }

        state = next_state;

        // Update the current position
        ++pos;
    }
    --pos;

    token.token = static_cast<TokenType>(state);
    token.tokenString = std::string(str + start_pos, pos - start_pos);
    token.line = line;
    token.column = pos - line_start;

    if (print) {
        token.print();
    }
    return token;
}