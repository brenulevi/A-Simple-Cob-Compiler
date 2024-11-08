#pragma once

#include <stack>

#include "lexer.h"

struct Production
{
    std::string head;
    std::vector<std::string> body;
};

struct TableAction
{
    char action;
    std::string value;
};

class Parser
{
private:
    
public:
    Parser();

    bool parse();

private:
    Lexer lexer;

    Token* lookahead;

    std::vector<std::string> symbols;
    std::vector<Production> productions;
    std::unordered_map<std::pair<std::string, std::string>, TableAction, glob::pair_hash> parse_table;

    // pushdown automata stack to parse algorithm
    std::stack<std::string> PA_stack;
};