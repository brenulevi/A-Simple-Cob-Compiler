#pragma once

#include <string>

#include "dfa.h"

enum class Tag
{
    END = -1,
    NONE,
    ID,
    KEYWORD,
    NUMBER,
    LITERAL
};

struct Token
{
    Token() : tag{0} {}
    Token(int tag) : tag{tag} {}

    int tag;
};

struct Id : public Token
{
    Id() : Token(), lexem{""} {}
    Id(Tag tag, std::string lexem) : Token{(int)tag}, lexem{lexem} {}

    std::string lexem;
};

struct Keyword : public Token
{
    Keyword() : Token(), lexem{""} {}
    Keyword(Tag tag, std::string lexem) : Token{(int)tag}, lexem{lexem} {}

    std::string lexem;
};

struct Number : public Token
{
    Number() : Token(), lexem{""} {}
    Number(Tag tag, std::string lexem) : Token{(int)tag}, lexem{lexem} {}

    std::string lexem;
};

class Lexer
{
public:
    Lexer();

    Token *get_next_token();

public:
    int current_line;
    struct
    {
        Token t;
        Id i;
        Keyword k;
        Number n;
    } token;

private:
    char peek;

    DFA id_automata;
    DFA keyword_automata;
    DFA number_automata;
};