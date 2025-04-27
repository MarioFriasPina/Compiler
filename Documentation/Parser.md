# Parser

## Original BNF Grammar

```bnf
PROGRAM ::= DECLARATION_LIST

DECLARATION_LIST ::= DECLARATION_LIST DECLARATION
DECLARATION_LIST ::= DECLARATION

DECLARATION ::= VAR_DECLARATION
DECLARATION ::= FUN_DECLARATION

VAR_DECLARATION ::= TYPE_SPECIFIER id ;
VAR_DECLARATION ::= TYPE_SPECIFIER id [ num ] ;

TYPE_SPECIFIER ::= int
TYPE_SPECIFIER ::= void

FUN_DECLARATION ::= TYPE_SPECIFIER id ( PARAMS ) COMPOUND_STMT

PARAMS ::= PARAM_LIST
PARAMS ::= void

PARAM_LIST ::= PARAM_LIST , PARAM
PARAM_LIST ::= PARAM

PARAM ::= TYPE_SPECIFIER id
PARAM ::= TYPE_SPECIFIER id [ ]

COMPOUND_STMT ::= { LOCAL_DECLARATIONS STATEMENT_LIST }

LOCAL_DECLARATIONS ::= LOCAL_DECLARATIONS VAR_DECLARATION
LOCAL_DECLARATIONS ::= ''

STATEMENT_LIST ::= STATEMENT_LIST STATEMENT
STATEMENT_LIST ::= ''

STATEMENT ::= EXPRESSION_STMT
STATEMENT ::= COMPOUND_STMT
STATEMENT ::= SELECTION_STMT
STATEMENT ::= ITERATION_STMT
STATEMENT ::= RETURN_STMT

EXPRESSION_STMT ::= EXPRESSION ;

SELECTION_STMT ::= if ( EXPRESSION ) STATEMENT
SELECTION_STMT ::= if ( EXPRESSION ) STATEMENT else STATEMENT

ITERATION_STMT ::= while ( EXPRESSION ) STATEMENT

RETURN_STMT ::= return ;
RETURN_STMT ::= return EXPRESSION ;

EXPRESSION ::= VAR = EXPRESSION
EXPRESSION ::= SIMPLE_EXPRESSION

VAR ::= id
VAR ::= id [ EXPRESSION ]

SIMPLE_EXPRESSION ::= ADDITIVE_EXPRESSION
SIMPLE_EXPRESSION ::= ADDITIVE_EXPRESSION RELOP ADDITIVE_EXPRESSION

RELOP ::= <=
RELOP ::= <
RELOP ::= >
RELOP ::= >=
RELOP ::= ==
RELOP ::= !=

ADDITIVE_EXPRESSION ::= ADDITIVE_EXPRESSION ADDOP TERM
ADDITIVE_EXPRESSION ::= TERM

ADDOP ::= +
ADDOP ::= -

TERM ::= TERM MULOP FACTOR
TERM ::= FACTOR

MULOP ::= *
MULOP ::= /

FACTOR ::= ( EXPRESSION )
FACTOR ::= VAR
FACTOR ::= CALL
FACTOR ::= num

CALL ::= id ( ARGS )

ARGS ::= ARG_LIST
ARGS ::= ''

ARG_LIST ::= ARG_LIST , EXPRESSION
ARG_LIST ::= EXPRESSION

```

## LL(1) Grammar

**The following grammar is not completely LL(1) because of [The Dangling Else Problem](https://en.wikipedia.org/wiki/Dangling_else), this will be fixed later during the parsing implementation**

DECLARATION_LIST ::= DECLARATION DECLARATION_LIST′

DECLARATION_LIST′ ::= ''
DECLARATION_LIST′ ::= DECLARATION DECLARATION_LIST′

DECLARATION ::= TYPE_SPECIFIER id DECLARATION′

DECLARATION′ ::= ( PARAMS ) COMPOUND_STMT
DECLARATION′ ::= VAR_DECLARATION'

VAR_DECLARATION ::= int id VAR_DECLARATION'

VAR_DECLARATION' ::= ;
VAR_DECLARATION' ::= [ num ] ;

TYPE_SPECIFIER ::= int
TYPE_SPECIFIER ::= void

PARAMS ::= PARAM_LIST
PARAMS ::= void

PARAM_LIST ::= PARAM PARAM_LIST′

PARAM_LIST′ ::= ''
PARAM_LIST′ ::= , PARAM PARAM_LIST′

PARAM ::= int id PARAM′

PARAM′ ::= ''
PARAM′ ::= [ ]

COMPOUND_STMT ::= { LOCAL_DECLARATIONS STATEMENT_LIST }

LOCAL_DECLARATIONS ::= ''
LOCAL_DECLARATIONS ::= VAR_DECLARATION LOCAL_DECLARATIONS

STATEMENT_LIST ::= ''
STATEMENT_LIST ::= STATEMENT STATEMENT_LIST

STATEMENT ::= EXPRESSION_STMT
STATEMENT ::= COMPOUND_STMT
STATEMENT ::= SELECTION_STMT
STATEMENT ::= ITERATION_STMT
STATEMENT ::= RETURN_STMT

SELECTION_STMT ::= if ( EXPRESSION ) STATEMENT SELECTION_STMT′

SELECTION_STMT′ ::= else STATEMENT
SELECTION_STMT′ ::= ''

EXPRESSION_STMT ::= EXPRESSION ;

ITERATION_STMT ::= while ( EXPRESSION ) STATEMENT

RETURN_STMT ::= return RETURN_STMT′

RETURN_STMT′ ::= ;
RETURN_STMT′ ::= EXPRESSION ;

EXPRESSION ::= ADDITIVE_EXPRESSION EXPRESSION′

EXPRESSION′ ::= ''
EXPRESSION′ ::= = EXPRESSION
EXPRESSION′ ::= RELOP ADDITIVE_EXPRESSION

RELOP ::= <=
RELOP ::= <
RELOP ::= >
RELOP ::= >=
RELOP ::= ==
RELOP ::= !=

ADDITIVE_EXPRESSION ::= TERM ADDITIVE_EXPRESSION′

ADDITIVE_EXPRESSION′ ::= ''
ADDITIVE_EXPRESSION′ ::= ADDOP TERM ADDITIVE_EXPRESSION′

ADDOP ::= +
ADDOP ::= -

TERM ::= FACTOR TERM′

TERM′ ::= ''
TERM′ ::= MULOP FACTOR TERM′

MULOP ::= \*
MULOP ::= /

FACTOR ::= ( EXPRESSION )
FACTOR ::= id FACTOR′
FACTOR ::= num

FACTOR′ ::= ''
FACTOR′ ::= [ EXPRESSION ]
FACTOR′ ::= ( ARGS )

ARGS ::= ''
ARGS ::= ARG_LIST

ARG_LIST ::= EXPRESSION ARG_LIST′

ARG_LIST′ ::= ''
ARG_LIST′ ::= , EXPRESSION ARG_LIST′

## FIRST and FOLLOW Sets

The FIRST and FOLLOW sets were obtained using [LL(1) Parser Visualization](https://www.cs.princeton.edu/courses/archive/spring20/cos320/LL1/)

| Nonterminal          | Nullable? | First                               | Follow                                                       |
| -------------------- | --------- | ----------------------------------- | ------------------------------------------------------------ |
| PROGRAM              | ✖         | int, void                           |                                                              |
| DECLARATION_LIST     | ✖         | int, void                           | $                                                            |
| DECLARATION_LIST′    | ✔         | int, void                           | $                                                            |
| DECLARATION          | ✖         | int, void                           | int, void, $                                                 |
| DECLARATION′         | ✖         | (, ;, [                             | int, void, $                                                 |
| VAR_DECLARATION      | ✖         | int                                 | }, {, if, while, return, (, id, num, int                     |
| VAR_DECLARATION'     | ✖         | ;, [                                | int, void, $, }, {, if, while, return, (, id, num            |
| TYPE_SPECIFIER       | ✖         | int, void                           | id                                                           |
| PARAMS               | ✖         | void, int                           | )                                                            |
| PARAM_LIST           | ✖         | int                                 | )                                                            |
| PARAM_LIST′          | ✔         | ,                                   | )                                                            |
| PARAM                | ✖         | int                                 | ), ,                                                         |
| PARAM′               | ✔         | [                                   | ), ,                                                         |
| COMPOUND_STMT        | ✖         | {                                   | int, void, }, {, if, while, return, (, id, num, $, else      |
| LOCAL_DECLARATIONS   | ✔         | int                                 | }, {, if, while, return, (, id, num                          |
| STATEMENT_LIST       | ✔         | {, if, while, return, (, id, num    | }                                                            |
| STATEMENT            | ✖         | {, if, while, return, (, id, num    | }, {, if, while, return, (, id, num, else                    |
| SELECTION_STMT       | ✖         | if                                  | }, {, if, while, return, (, id, num, else                    |
| SELECTION_STMT′      | ✔         | else                                | }, {, if, while, return, (, id, num, else                    |
| EXPRESSION_STMT      | ✖         | (, id, num                          | }, {, if, while, return, (, id, num, else                    |
| ITERATION_STMT       | ✖         | while                               | }, {, if, while, return, (, id, num, else                    |
| RETURN_STMT          | ✖         | return                              | }, {, if, while, return, (, id, num, else                    |
| RETURN_STMT′         | ✖         | ;, (, id, num                       | }, {, if, while, return, (, id, num, else                    |
| EXPRESSION           | ✖         | (, id, num                          | ), ;, ], ,                                                   |
| EXPRESSION′          | ✔         | =, &lt;=, &lt;, &gt;, &gt;=, ==, != | ), ;, ], ,                                                   |
| RELOP                | ✖         | &lt;=, &lt;, &gt;, &gt;=, ==, !=    | (, id, num                                                   |
| ADDITIVE_EXPRESSION  | ✖         | (, id, num                          | ), ;, =, &lt;=, &lt;, &gt;, &gt;=, ==, !=, ], ,              |
| ADDITIVE_EXPRESSION′ | ✔         | +, -                                | ), ;, =, &lt;=, &lt;, &gt;, &gt;=, ==, !=, ], ,              |
| ADDOP                | ✖         | +, -                                | (, id, num                                                   |
| TERM                 | ✖         | (, id, num                          | ), ;, =, &lt;=, &lt;, &gt;, &gt;=, ==, !=, +, -, ], ,        |
| TERM′                | ✔         | \*, /                               | ), ;, =, &lt;=, &lt;, &gt;, &gt;=, ==, !=, +, -, ], ,        |
| MULOP                | ✖         | \*, /                               | (, id, num                                                   |
| FACTOR               | ✖         | (, id, num                          | ), ;, =, &lt;=, &lt;, &gt;, &gt;=, ==, !=, +, -, \*, /, ], , |
| FACTOR′              | ✔         | [, (                                | ), ;, =, &lt;=, &lt;, &gt;, &gt;=, ==, !=, +, -, \*, /, ], , |
| ARGS                 | ✔         | (, id, num                          | )                                                            |
| ARG_LIST             | ✖         | (, id, num                          | )                                                            |
| ARG_LIST′            | ✔         | ,                                   | )                                                            |

## Transition Table

|                      | $   | id                              | (                               | )   | int                                | ;                | [                | num                             | ]   | void                           | ,                      | {                                     | }   | if                                          | else | while                          | return                   | =            | &lt;=                     | &lt;                      | &gt;                      | &gt;=                     | ==                        | !=                        | +                               | -                               | \*                 | /                  |
| -------------------- | --- | ------------------------------- | ------------------------------- | --- | ---------------------------------- | ---------------- | ---------------- | ------------------------------- | --- | ------------------------------ | ---------------------- | ------------------------------------- | --- | ------------------------------------------- | ---- | ------------------------------ | ------------------------ | ------------ | ------------------------- | ------------------------- | ------------------------- | ------------------------- | ------------------------- | ------------------------- | ------------------------------- | ------------------------------- | ------------------ | ------------------ |
| PROGRAM              |     |                                 |                                 |     | DECLARATION_LIST $                 |                  |                  |                                 |     | DECLARATION_LIST $             |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| DECLARATION_LIST     |     |                                 |                                 |     | DECLARATION DECLARATION_LIST′      |                  |                  |                                 |     | DECLARATION DECLARATION_LIST′  |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| DECLARATION_LIST′    | ε   |                                 |                                 |     | DECLARATION DECLARATION_LIST′      |                  |                  |                                 |     | DECLARATION DECLARATION_LIST′  |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| DECLARATION          |     |                                 |                                 |     | TYPE_SPECIFIER id DECLARATION′     |                  |                  |                                 |     | TYPE_SPECIFIER id DECLARATION′ |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| DECLARATION′         |     |                                 | ( PARAMS ) COMPOUND_STMT        |     |                                    | VAR_DECLARATION' | VAR_DECLARATION' |                                 |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| VAR_DECLARATION      |     |                                 |                                 |     | int id VAR_DECLARATION'            |                  |                  |                                 |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| VAR_DECLARATION'     |     |                                 |                                 |     |                                    | ;                | [ num ] ;        |                                 |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| TYPE_SPECIFIER       |     |                                 |                                 |     | int                                |                  |                  |                                 |     | void                           |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| PARAMS               |     |                                 |                                 |     | PARAM_LIST                         |                  |                  |                                 |     | void                           |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| PARAM_LIST           |     |                                 |                                 |     | PARAM PARAM_LIST′                  |                  |                  |                                 |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| PARAM_LIST′          |     |                                 |                                 | ε   |                                    |                  |                  |                                 |     |                                | , PARAM PARAM_LIST′    |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| PARAM                |     |                                 |                                 |     | int id PARAM′                      |                  |                  |                                 |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| PARAM′               |     |                                 |                                 | ε   |                                    |                  | [ ]              |                                 |     |                                | ε                      |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| COMPOUND_STMT        |     |                                 |                                 |     |                                    |                  |                  |                                 |     |                                |                        | { LOCAL_DECLARATIONS STATEMENT_LIST } |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| LOCAL_DECLARATIONS   |     | ε                               | ε                               |     | VAR_DECLARATION LOCAL_DECLARATIONS |                  |                  | ε                               |     |                                |                        | ε                                     | ε   | ε                                           |      | ε                              | ε                        |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| STATEMENT_LIST       |     | STATEMENT STATEMENT_LIST        | STATEMENT STATEMENT_LIST        |     |                                    |                  |                  | STATEMENT STATEMENT_LIST        |     |                                |                        | STATEMENT STATEMENT_LIST              | ε   | STATEMENT STATEMENT_LIST                    |      | STATEMENT STATEMENT_LIST       | STATEMENT STATEMENT_LIST |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| STATEMENT            |     | EXPRESSION_STMT                 | EXPRESSION_STMT                 |     |                                    |                  |                  | EXPRESSION_STMT                 |     |                                |                        | COMPOUND_STMT                         |     | SELECTION_STMT                              |      | ITERATION_STMT                 | RETURN_STMT              |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| SELECTION_STMT       |     |                                 |                                 |     |                                    |                  |                  |                                 |     |                                |                        |                                       |     | if ( EXPRESSION ) STATEMENT SELECTION_STMT′ |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| SELECTION_STMT′      |     | ε                               | ε                               |     |                                    |                  |                  | ε                               |     |                                |                        | ε                                     | ε   | ε                                           | ε    | ε                              | ε                        |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| EXPRESSION_STMT      |     | EXPRESSION ;                    | EXPRESSION ;                    |     |                                    |                  |                  | EXPRESSION ;                    |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| ITERATION_STMT       |     |                                 |                                 |     |                                    |                  |                  |                                 |     |                                |                        |                                       |     |                                             |      | while ( EXPRESSION ) STATEMENT |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| RETURN_STMT          |     |                                 |                                 |     |                                    |                  |                  |                                 |     |                                |                        |                                       |     |                                             |      |                                | return RETURN_STMT′      |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| RETURN_STMT′         |     | EXPRESSION ;                    | EXPRESSION ;                    |     |                                    | ;                |                  | EXPRESSION ;                    |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| EXPRESSION           |     | ADDITIVE_EXPRESSION EXPRESSION′ | ADDITIVE_EXPRESSION EXPRESSION′ |     |                                    |                  |                  | ADDITIVE_EXPRESSION EXPRESSION′ |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| EXPRESSION′          |     |                                 |                                 | ε   |                                    | ε                |                  |                                 | ε   |                                | ε                      |                                       |     |                                             |      |                                |                          | = EXPRESSION | RELOP ADDITIVE_EXPRESSION | RELOP ADDITIVE_EXPRESSION | RELOP ADDITIVE_EXPRESSION | RELOP ADDITIVE_EXPRESSION | RELOP ADDITIVE_EXPRESSION | RELOP ADDITIVE_EXPRESSION |                                 |                                 |                    |                    |
| RELOP                |     |                                 |                                 |     |                                    |                  |                  |                                 |     |                                |                        |                                       |     |                                             |      |                                |                          |              | &lt;=                     | &lt;                      | &gt;                      | &gt;=                     | ==                        | !=                        |                                 |                                 |                    |                    |
| ADDITIVE_EXPRESSION  |     | TERM ADDITIVE_EXPRESSION′       | TERM ADDITIVE_EXPRESSION′       |     |                                    |                  |                  | TERM ADDITIVE_EXPRESSION′       |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| ADDITIVE_EXPRESSION′ |     |                                 |                                 | ε   |                                    | ε                |                  |                                 | ε   |                                | ε                      |                                       |     |                                             |      |                                |                          | ε            | ε                         | ε                         | ε                         | ε                         | ε                         | ε                         | ADDOP TERM ADDITIVE_EXPRESSION′ | ADDOP TERM ADDITIVE_EXPRESSION′ |                    |                    |
| ADDOP                |     |                                 |                                 |     |                                    |                  |                  |                                 |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           | +                               | -                               |                    |                    |
| TERM                 |     | FACTOR TERM′                    | FACTOR TERM′                    |     |                                    |                  |                  | FACTOR TERM′                    |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| TERM′                |     |                                 |                                 | ε   |                                    | ε                |                  |                                 | ε   |                                | ε                      |                                       |     |                                             |      |                                |                          | ε            | ε                         | ε                         | ε                         | ε                         | ε                         | ε                         | ε                               | ε                               | MULOP FACTOR TERM′ | MULOP FACTOR TERM′ |
| MULOP                |     |                                 |                                 |     |                                    |                  |                  |                                 |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 | \*                 | /                  |
| FACTOR               |     | id FACTOR′                      | ( EXPRESSION )                  |     |                                    |                  |                  | num                             |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| FACTOR′              |     |                                 | ( ARGS )                        | ε   |                                    | ε                | [ EXPRESSION ]   |                                 | ε   |                                | ε                      |                                       |     |                                             |      |                                |                          | ε            | ε                         | ε                         | ε                         | ε                         | ε                         | ε                         | ε                               | ε                               | ε                  | ε                  |
| ARGS                 |     | ARG_LIST                        | ARG_LIST                        | ε   |                                    |                  |                  | ARG_LIST                        |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| ARG_LIST             |     | EXPRESSION ARG_LIST′            | EXPRESSION ARG_LIST′            |     |                                    |                  |                  | EXPRESSION ARG_LIST′            |     |                                |                        |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |
| ARG_LIST′            |     |                                 |                                 | ε   |                                    |                  |                  |                                 |     |                                | , EXPRESSION ARG_LIST′ |                                       |     |                                             |      |                                |                          |              |                           |                           |                           |                           |                           |                           |                                 |                                 |                    |                    |

