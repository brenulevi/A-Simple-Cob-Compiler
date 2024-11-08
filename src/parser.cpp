#include "parser.h"
#include <fstream>
#include <sstream>
#include <string>
#include <pugixml.hpp>
#include <iostream>
#include <cassert>

#define assertm(exp, msg) assert(((void)msg, exp))

Parser::Parser()
{
    // initialize productions, symbols and parser table using gold parser xml file

    pugi::xml_document parser_tag;
    pugi::xml_parse_result result = parser_tag.load_file("./utils/lalrtable.xml");
    
    assertm(result, "failed to load table parser file");

    auto table_tag = parser_tag.child("Tables");
    auto symbols_tag = table_tag.child("m_Symbol");
    auto productions_tag = table_tag.child("m_Production");
    auto lalr_table_tag = table_tag.child("LALRTable");

    for(auto symbol : symbols_tag.children("Symbol"))
        symbols.push_back(symbol.attribute("Name").value());

    for(auto production : productions_tag.children("Production"))
    {
        Production p;
        p.head = symbols[production.attribute("NonTerminalIndex").as_uint()];
        for(auto prod_symbol : production.children("ProductionSymbol"))
            p.body.push_back(symbols[prod_symbol.attribute("SymbolIndex").as_uint()]);
        productions.push_back(p);
    }

    for(auto lalr_state : lalr_table_tag.children("LALRState"))
    {
        for(auto lalr_action : lalr_state.children("LALRAction"))
        {
            char action = ' ';
            switch (lalr_action.attribute("Action").as_uint())
            {
            case 1:
                action = 's';
                break;            
            case 2:
                action = 'r';
                break;
            case 3:
                action = 'g';
                break;
            case 4:
                action = 'a';
                break;
            }

            parse_table[{lalr_state.attribute("Index").as_string(), symbols[lalr_action.attribute("SymbolIndex").as_uint()]}] = {action, lalr_action.attribute("Value").as_string()};
        }
    }
}

bool Parser::parse()
{
    lookahead = lexer.get_next_token();
    PA_stack.push("0");

    while(true)
    {
        Tag la_tag = (Tag)lookahead->tag;

        TableAction action;
        std::string symbol;
        if(la_tag == Tag::ID)
        {
            action = parse_table[{PA_stack.top(), "id"}];
            symbol = "id";
        }
        else if(la_tag == Tag::KEYWORD)
        {
            Keyword* k_tag = reinterpret_cast<Keyword*>(lookahead);
            action = parse_table[{PA_stack.top(), k_tag->lexem}];
            symbol = k_tag->lexem;
        }
        else if(la_tag == Tag::NUMBER)
        {
            action = parse_table[{PA_stack.top(), "number"}];
            symbol = "number";
        }
        else    // is operator or eof
        {
            if(la_tag == Tag::END)
            {
                action = parse_table[{PA_stack.top(), "EOF"}];
            }
            else
            {
                std::stringstream ss;
                ss << (char)lookahead->tag;
                action = parse_table[{PA_stack.top(), ss.str()}];
                symbol = ss.str();
            }
        }

        if(action.action == 's')        // shift operation
        {
            PA_stack.push(symbol);
            PA_stack.push(action.value);

            lookahead = lexer.get_next_token();
        }
        else if (action.action == 'r')  // reduce operation
        {
            auto prod = productions[std::stoi(action.value)];
            for(int i = 0; i < prod.body.size() * 2; i++)
                PA_stack.pop();

            std::string current_state = PA_stack.top();

            PA_stack.push(prod.head);

            PA_stack.push(parse_table[{current_state, PA_stack.top()}].value);
        }
        else if (action.action == 'a')  // accept operation
        {
            break;
        }
    }


    
    return true;
}
