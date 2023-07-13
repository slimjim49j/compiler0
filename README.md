# Compiler Notes

A basic compiler to demonstrate frontend compiler concepts

---

## Compilers 101

A 10000 foot view:

Writing machine code can be tedious and unintuitive.

Since most people work with language everyday, let's use language to specify instructions to the cpu.

The first languages would've been assemblers. These have very direct mappings to cpu instructions.

The design of most languages however have much more abstraction built in to their interfaces.

This increases the problem space the compiler has to cover in order to translate the language syntax into machine code.

As such the problem of compiling a language into machine code is divided into multiple smaller steps.

---

## Applications

- A lot of languages aren't great, make a better one
- Compilers, interpreters, DSLs, transpilers, preprocessors
- Editor tooling

---

## Compilation Steps

This is one way of breaking down the problem into smaller parts.

```
Lexing -> Parsing -> Type Checking -> IR -> Assembly -> Executable
|________________________________|    |__________________________|
            Frontend                            Backend
```

---

## Terms

- Token = a string that has some vague meaning to your language
```
// a comment
"a string"
1234
aFunctionCall()
int aVariable = 0
if () { ... } else { ... }
```

- Lexing = turn file bytes to tokens
```
This:
3 + 4

To this:
[NUMBER_LITERAL, PLUS_SYMBOL, NUMBER_LITERAL]
```
- Parsing = turn tokens into internal grammar, an AST
```
This:
[NUMBER_LITERAL, PLUS_SYMBOL, NUMBER_LITERAL]

To this:
        (Addition Node)
            /     \
           /       \
     (Value 3)   (Value 4)
```
- Expression = instructions that evaluate to a value
```
3 + 4;
doStuff();
```
- Statement = instructions that don't evaluate to a value
```
bool thing = true;
if (thing) {
    int test = 5;
}
```

---

## Problem Statement

Transform x file(s) with y syntax into z instructions.

---

## One Solution

Use a framework to do this.

ANTLR can be used in C# to create an interpreter / compiler for a grammar defined in BNF notation.

---

## A Look At Parsing

Recursive descent parsing.

```
Example input:
5 * 4 + 3

Output tree:
        +
       / \
      *   3
     / \
    5   4
```

---

## Parser design

https://youtu.be/MnctEW1oL-E

## Memory Allocation Strategies

https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator
