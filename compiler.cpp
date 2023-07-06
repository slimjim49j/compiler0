#include <windows.h>
#include <stdio.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long s64;

struct Arena
{
    void *memory;
    u64 pos;
    u64 total;
};

bool createArena(Arena *arena, u64 total)
{
    void *memory = VirtualAlloc(0, total, MEM_COMMIT, PAGE_READWRITE);
    if (memory)
    {
        arena->memory = memory;
        arena->pos = 0;
        arena->total = total;
        return true;
    }
    else
    {
        // error handle
        return false;
    }
}

bool freeArena(Arena arena)
{
    return VirtualFree(arena.memory, 0, MEM_RELEASE);
}

void *pushArena(Arena *arena, u64 size)
{
    u64 startingPos = arena->pos;
    arena->pos += size;
    if (arena->pos < arena->total)
    {
        return (u8 *)arena->memory + startingPos;
    }
    else
    {
        return 0;
    }
}

void *popArena(Arena *arena, u64 size)
{
    u64 newPos = arena->pos - size;
    // test overflow
    if (newPos <= arena->pos)
    {
        arena->pos = newPos;
        return (u8 *)arena->memory + size;
    }
    else
    {
        return 0;
    }
}

// #define pushArenaArray(arena, type, count) (type *)pushArena(arena, sizeof(type) * count)
#define pushArenaStruct(arena, type) (type *)pushArena(arena, sizeof(type))

enum TokenEnum
{
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    NUMBER_LITERAL,
    ERR,
};

struct Token
{
    TokenEnum type;
    u64 integerValue;
    u8 extra[30];
    u64 length;
};

// handle single character symbol
// handle multiple character symbols
bool attemptSymbol(u8 *input, Token *token)
{
    u8 character = *input;
    switch (character)
    {
    case '+':
    {
        token->type = TokenEnum::PLUS;
        token->length = 1;
        OutputDebugStringA("Found plus\n");
        return true;
    }
    break;
    case '-':
    {
        token->type = TokenEnum::MINUS;
        token->length = 1;
        OutputDebugStringA("Found minus\n");
        return true;
    }
    break;
    case '*':
    {
        token->type = TokenEnum::MULTIPLY;
        token->length = 1;
        OutputDebugStringA("Found mult\n");
        return true;
    }
    break;
    case '/':
    {
        token->type = TokenEnum::DIVIDE;
        token->length = 1;
        OutputDebugStringA("Found div\n");
        return true;
    }
    break;
    }
    return false;
}

bool attemptNumberLiteral(u8 *input, u64 inputLength, Token *token)
{
    u8 character = *input;
    if (character >= '0' && character <= '9')
    {
        // walk the input until it stops
        // init number to be first digit
        u64 number = character - '0';
        character = *(++input);
        u64 i = 1;
        for (; i < inputLength && (character >= '0' && character <= '9'); ++i)
        {
            u8 nextNumber = character - '0';

            // number update
            number *= 10;
            number += nextNumber;
            character = *(++input);
        }
        token->type = TokenEnum::NUMBER_LITERAL;
        token->integerValue = number;
        token->length = i;
        {
            char buffer[255];
            wsprintfA(buffer, "found num: %d\n", token->integerValue);
            OutputDebugStringA(buffer);
        }
        return true;
    }
    else
    {
        return false;
    }
}

// if (character is ascii) {
//     determine if keyword
//     determine if type
//     determine if ident
// }
bool attemptAlphanumeric(u8 *input, u64 length, Token *token)
{
    u8 character = *input;
    // if ()
    return false;
}

bool lexer(Arena *arena, u8 *input, u64 inputLength)
{
    u8 *reader = input;

    for (u64 i = 0; i < inputLength; i++)
    {
        char character = *reader;

        if (character == ' ' || character == '\n')
        {
            reader++;
            continue;
        }

        Token newToken = {};
        if (
            attemptSymbol(reader, &newToken) ||
            attemptNumberLiteral(reader, inputLength, &newToken) ||
            attemptAlphanumeric(reader, inputLength, &newToken))
        {
            if (newToken.type == TokenEnum::ERR)
            {
                // log err
                return false;
            }
            else
            {
                // we got a token
                Token *writer = pushArenaStruct(arena, Token);
                *writer = newToken;
                reader += newToken.length;
            }
        }
    }
    return true;
}

// enum Operator {
//     PLUS, MINUS, MULTIPLY, DIVIDE,
// };

struct Expr {
    Expr *left;
    Expr *right;
    TokenEnum op;
    // or
    u64 value;
    
    bool error;
};

Token * peekNext(Arena *lexIn, u64 lexIndex) {
    u64 idx = lexIndex + sizeof(Token);
    if (idx < lexIn->pos) {
        u8 *reader = (u8 *)lexIn->memory + idx;
        return (Token *)reader;
    } else {
        return 0;
    }
}

Expr parseSubexpr(Arena *lexIn, u64 lexIndex, Arena *parseOut, s64 priority) {
    Expr expr = {};
    
    u8 * reader = (u8 *)lexIn->memory;
    Token token = *(Token *)(reader + lexIndex);
    Token * nextToken = peekNext(lexIn, lexIndex);
    // todo test token.type
    if (nextToken) {
        Expr expr = {};
        switch (nextToken->type) {
            case TokenEnum::NUMBER_LITERAL: {
                // next token should be a binary op
                expr.error = true;
                return expr;
            } break;
            case TokenEnum::MULTIPLY: {
                if (priority <= 100) {
                    // todo parse right hand of expr
                    // peek next, if valid, parse right subexpr
                } else {
                    // todo return just token
                }
            } break;
            case TokenEnum::DIVIDE: {

            } break;
            case TokenEnum::PLUS: {

            } break;
            case TokenEnum::MINUS: {

            } break;
        }
    }
    return expr;
}

bool parser(Arena *lexIn, Arena *parseOut) {
    Token * reader = (Token *)lexIn->memory;
    u64 length = lexIn->pos;

    Expr topExpr = {};
    Expr parentExpr = topExpr;

    u64 i = 0;
    while (i < length) {
    }
    return false;
}

int main()
{
    while (true)
    {
        u8 input[1000] = {};
        scanf_s("\n%s", &input[0], 1000);

        Arena lexArena = {};
        if (createArena(&lexArena, 1 << 20))
        {
            if (lexer(&lexArena, input, 1000)) {
                // todo parser();
            }
            freeArena(lexArena);
        }
    }

    return 0;
}
// todo:
// implement attemptAlpha
// test token stream
// alloc token stream to arena
// build parse tree from token array
// convert parse tree to instruction stream
// read input from file
// output to file
// link

// planned features:
// math ops
// OOP and parens
// variable declaration
// if / while loops
