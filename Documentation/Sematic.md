# Semantic Analysis

## 1. Symbol Table

The symbol table is implemented as a tree of scopes, where each `SymbolTable` node represents a scope. The root is the **Global** scope.

```cpp
struct Symbol {
    std::string name;            // Identifier name
    std::string type;            // Type (e.g., "int", "void")
    int line;                    // Declaration line number
    size_t size = 0;             // Array size (0 if not an array)
    std::vector<std::string> args; // Parameter types (empty if not a function)
};

struct SymbolTable {
    std::string name;                         // Scope name (e.g., "Global", function name)
    std::unordered_map<std::string, Symbol> symbols; // Map of symbols in this scope
    std::vector<SymbolTable> children;       // Nested scopes
    SymbolTable *parent;                     // Pointer to parent scope (NULL for global)
    int current_child = 0;                   // Index for traversal
};

```

### Key Components

-   **Scope Name**: Identifier for the scope (`Global`, function name, or block label).
-   **`symbols` Map**: Stores all `Symbol` entries declared in this scope.
-   **Child Scopes**: Nested scopes created for functions, compound statements, loops, and conditionals.
-   **Parent Link**: Each child scope holds a pointer to its parent scope for upward lookups.

### Scope Construction

-   **Global**: Initialized with built-in functions `input()` and `output(int)`.
-   **Function Declaration**: Creates a new child scope under `Global`, named after the function.
-   **Blocks (`{}`)**, **loops**, and **conditionals**: Each creates a nested scope with a unique name.

---

## 2. Logical Inference Rules

Semantic analysis is performed by $\textsf{typecheck}(AST, \Gamma)$, where $\Gamma$ is the current symbol table scope.

### 2.1. Variable Lookup

-   **Rule**: If an identifier $x$ appears, its type is $\tau$ if $x: \tau \in \Gamma$.

```text
─────── (Var)
Γ ⊢ id(x) : τ
   if lookup(Γ, x) = τ
```

### 2.2. Numeric Literals

-   **Rule**: Any numeric literal `num` has type `int`.

```text
─────────── (Num)
Γ ⊢ num(n) : int
```

### 2.3. Array Access

-   **Premises**: 1) $a: τ[ ] \in Γ$ 2) The index expression has type `int`.
-   **Conclusion**: Accessing $a[e]$ yields type $τ$.

```text
Γ ⊢ e : int     Γ ⊢ a : array of τ
────────────────────────────────── (Array)
      Γ ⊢ a[e] : τ
```

### 2.4. Binary Operations

-   **Premises**: Two subexpressions $E_1$ and $E_2$ both have the same type $τ$.
-   **Conclusion**: Operations like `+`, `-`, `*`, `/`, relational and equality yield type $τ$ or `int` for relational.

```text
Γ ⊢ E₁ : τ     Γ ⊢ E₂ : τ
───────────────────────── (BinOp)
   Γ ⊢ E₁ op E₂ : τ
```

For relational ops (`<`, `==`, etc.), the result type is `int` (0 or 1) but currently type-checked as same type.

### 2.5. Assignment

-   **Premises**: Left-hand side $E_1$ and right-hand side $E_2$ have the same type $τ$.
-   **Conclusion**: Assignment `E₁ = E₂` yields type $τ$.

```text
Γ ⊢ E₁ : τ     Γ ⊢ E₂ : τ
───────────────────────── (Assign)
   Γ ⊢ E₁ = E₂ : τ
```

### 2.6. Function Call

-   **Premises**: $f: τ (τ₁, …, τₙ) \in Γ$ and each argument expression $eᵢ$ has type $τᵢ$.
-   **Conclusion**: The call `f(e₁, …, eₙ)` yields type $τ$.

```text
Γ ⊢ e₁ : τ₁ … Γ ⊢ eₙ : τₙ     f: τ(τ₁,…,τₙ) ∈ Γ
────────────────────────────────────────────── (Call)
     Γ ⊢ f(e₁,…,eₙ) : τ
```

### 2.7. Return Statement

-   **Premises**: In function scope named $f$ with declared return type $τ$, the return expression $e$ has type $τ$.
-   **Conclusion**: `return e;` is valid.

```text
Γ(f) ⊢ e : τ     return type(f) = τ
───────────────────────────────── (Return)
      Γ(f) ⊢ return e : τ
```
