#include <stdio.h>
#include <windows.h>

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

// #define pushArenaArray(arena, type, count) (type *)pushArena(arena,
// sizeof(type) * count)
#define pushArenaStruct(arena, type) (type *)pushArena(arena, sizeof(type))

enum TokenEnum
{
    NOTHING_TO_SEE_HERE,
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
        for (; i < inputLength && (character >= '0' && character <= '9');
             ++i)
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

    while (*reader)
    {
        char character = *reader;

        if (character == ' ' || character == '\n')
        {
            reader++;
            continue;
        }

        Token newToken = {};
        if (attemptSymbol(reader, &newToken) ||
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

// struct PointerArray
// {
//     // length in elements (1 element = 8 bytes)
//     u32 length;
//     // capacity in elements
//     u32 capacity;
//     void **start;
// };

// // get size in bytes for n elements
// u32 getTotalArraySize(u32 size)
// {
//     return size * sizeof(void *);
// }

// // capacity in elements
// PointerArray createPointerArray(Arena *arena, u32 capacity)
// {
//     pushArena(arena, getTotalArraySize(capacity));
// }

// void *getPointerArrayPointer(PointerArray *array, u32 index)
// {
//     void *result = *(array->start + getTotalArraySize(index));
//     return result;
// }

// // this reallocs without freeing?
// void pushPointerArray(Arena *arena, PointerArray *array, void *pointerElement)
// {
//     if (array->length == array->capacity)
//     {
//         // we're at capacity, resize array
//         PointerArray newArray = createPointerArray(arena, array->length + 1);

//         // copy to new array
//         void **oldArrayItemPointer = array->start;
//         for (u32 i = 0; i < array->length; ++i)
//         {
//             // push pointer
//             void *oldArrayItem = *oldArrayItemPointer;
//             pushPointerArray(arena, &newArray, oldArrayItem);
//             oldArrayItemPointer++;
//         }

//         // copy new array struct to old struct
//         array->capacity = newArray.capacity;
//         array->length = newArray.length;
//         array->start = newArray.start;
//     }
//     else
//     {
//         // we're under capacity, continue as normal
//         void **insertionPoint = array->start + getTotalArraySize(array->length);
//         *insertionPoint = pointerElement;
//         array->length++;
//     }
// }

struct Expr
{
    Expr *left;
    Expr *right;
    TokenEnum op;
    // or
    u64 value;
    // this might be better
    // PointerArray children;

    bool error;
};

u64 addTokenIndex(u64 lexIndex, u64 advanceThisManyTokens)
{
    return lexIndex + (advanceThisManyTokens * sizeof(Token));
}

Token *getToken(Arena *lexIn, u64 lexIndex)
{
    if (lexIndex < lexIn->pos)
    {
        u8 *reader = (u8 *)lexIn->memory + lexIndex;
        return (Token *)reader;
    }
    else
    {
        return 0;
    }
}

u64 MULTIPLY_PRIORITY = 100;
u64 ADD_PRIORITY = 50;

// startLexingAt is an offset in bytes
// expr gets filled in
// ret new index in lexIn arena
u64 parseSubexpr(Arena *lexIn, u64 startLexingAt, Arena *parseOut, s64 priority, Expr *expr)
{
    u64 lexingMemoryIndex = startLexingAt;
    u64 endOfLexingIndex = lexIn->pos;
    Token *currentToken;

    u64 operand1;
    bool operand1Filled = false;

    Expr *leftExpr = 0;
    Expr *rightExpr = 0;

    while (lexingMemoryIndex < endOfLexingIndex)
    {
        currentToken = getToken(lexIn, lexingMemoryIndex);

        bool isBinaryOp = false;
        TokenEnum binaryOp;
        s32 binaryOpPriority = 0;

        switch (currentToken->type)
        {
        case TokenEnum::NUMBER_LITERAL:
        {
            if (!operand1Filled)
            {
                operand1 = currentToken->integerValue;
                operand1Filled = true;
            }
            else
            {
                // throw
            }
        }
        break;
        case TokenEnum::MULTIPLY:
        {
            isBinaryOp = true;
            binaryOp = TokenEnum::MULTIPLY;
            binaryOpPriority = MULTIPLY_PRIORITY;
        }
        break;
        case TokenEnum::PLUS:
        {
            isBinaryOp = true;
            binaryOp = TokenEnum::PLUS;
            binaryOpPriority = ADD_PRIORITY;
        }
        break;
        }

        if (isBinaryOp)
        {
            if (operand1Filled || leftExpr)
            {
                if (binaryOpPriority > priority)
                {
                    // parse right side of multiply
                    u64 continueLexingAt = addTokenIndex(lexingMemoryIndex, 1);
                    rightExpr = pushArenaStruct(parseOut, Expr);
                    *rightExpr = {};
                    lexingMemoryIndex = parseSubexpr(lexIn, continueLexingAt, parseOut, binaryOpPriority, rightExpr);

                    // aggregate left and right sides back into left
                    // not sure if this is right
                    if (operand1Filled)
                    {
                        leftExpr = pushArenaStruct(parseOut, Expr);
                        *leftExpr = {};
                        leftExpr->value = operand1;
                        operand1Filled = false;
                    }

                    // left and right is now populated, aggregate back to left
                    Expr *aggregateExpr = pushArenaStruct(parseOut, Expr);
                    *aggregateExpr = {};
                    aggregateExpr->left = leftExpr;
                    aggregateExpr->right = rightExpr;
                    aggregateExpr->op = binaryOp;
                    leftExpr = aggregateExpr;

                    continue;
                }
                else
                {
                    // pack up and return
                    expr->value = operand1;
                    return lexingMemoryIndex;
                }
            }
            else
            {
                // throw
            }
        }

        // increment lexing index
        lexingMemoryIndex = addTokenIndex(lexingMemoryIndex, 1);
    }

    // cpy into expr
    // expr->children = leftExpr->children
    if (operand1Filled)
    {
        expr->value = operand1;
    }
    else
    {
        expr->error = leftExpr->error;
        expr->left = leftExpr->left;
        expr->op = leftExpr->op;
        expr->right = leftExpr->right;
        expr->value = leftExpr->value;
    }

    return lexingMemoryIndex;
}

// do I need toplevelexprs? or should there be a single top level node? plan more
// topLevelExprs can probably be used for multiple exprs
Expr *parser(Arena *lexIn, Arena *parseOut)
{
    Expr *expr = pushArenaStruct(parseOut, Expr);
    *expr = {};
    parseSubexpr(lexIn, 0, parseOut, -9999, expr);
    return expr;
}

// num high num med num low num
// ------------|
//      expr
void logParseNode(Expr *expr)
{
    TokenEnum op = expr->op;
    LPSTR str;
    switch (op)
    {
    case TokenEnum::MULTIPLY:
    {
        str = "multiply";
    }
    break;
    case TokenEnum::PLUS:
    {
        str = "add";
    }
    break;
    case TokenEnum::NOTHING_TO_SEE_HERE:
    {
        str = "";
    }
    break;
    default:
    {
        str = "unknown op";
    }
    break;
    }

    char buffer[255];
    wsprintfA(buffer, "op: %s, value: %d\n", str, expr->value);
    OutputDebugStringA(buffer);
}

void traverseParseTree(Expr *expr, Expr *parent)
{
    OutputDebugStringA("===============\n");

    if (parent)
    {
        OutputDebugStringA("Parent: ");
        logParseNode(parent);
    }

    OutputDebugStringA("Child: ");
    logParseNode(expr);

    OutputDebugStringA("===============\n");

    if (expr->left)
    {
        traverseParseTree(expr->left, expr);
    }
    if (expr->right)
    {
        traverseParseTree(expr->right, expr);
    }
}

u64 doMath(Expr *expr)
{
    if (expr->op)
    {
        u64 left = doMath(expr->left);
        u64 right = doMath(expr->right);
        switch (expr->op)
        {
        case TokenEnum::MULTIPLY:
        {
            return left * right;
        }
        break;
        case TokenEnum::PLUS:
        {
            return left + right;
        }
        break;
        default:
        {
            OutputDebugStringA("Something went wrong");
            return 0;
        }
        break;
        }
    }
    else
    {
        return expr->value;
    }
}

int main()
{
    while (true)
    {
        u8 input[1000] = {};
        fgets((s8 *)input, sizeof input, stdin);

        Arena lexArena = {};
        Arena parseArena = {};
        if (createArena(&lexArena, 1 << 20) && createArena(&parseArena, 1 << 20))
        {
            if (lexer(&lexArena, input, 1000))
            {
                Expr *expr = parser(&lexArena, &parseArena);
                u64 result = doMath(expr);
                char buffer[255];
                wsprintfA(buffer, "Result: %d\n", result);
                OutputDebugStringA(buffer);

                // traverseParseTree(expr, 0);
            }
            freeArena(lexArena);
            // free parse arena?
        }
    }

    return 0;
}
