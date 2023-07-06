# Compiler Notes

## Parser design

https://youtu.be/MnctEW1oL-E

## Memory Allocation Strategies

https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator

## 101

- Token = a string that has some meaning to your language
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
int thing = false;
if (thing) {
    doStuff();
}
```
