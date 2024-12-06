#include "parser.h"
#include <fstream>
#include <sstream>
#include <string>
#include <pugixml.hpp>
#include <iostream>
#include <cassert>
#include <list>

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
    bool syntax_error = false;

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
        else{
            // Syntax error treatment
            syntax_error = true;

            std::stringstream error_message;

            error_message << "SYNTAX ERROR: Line " << lexer.current_line << ", state " << PA_stack.top() << " - ";

            // Identify error token
            if (la_tag == Tag::END) {
                error_message << "Unexpected EOF found.";
            } else if (la_tag == Tag::ID) {
                Id* id_token = reinterpret_cast<Id*>(lookahead);
                error_message << "Identifier '" << id_token->lexem << "' not expected.";
            } else if (la_tag == Tag::KEYWORD) {
                Keyword* k_token = reinterpret_cast<Keyword*>(lookahead);
                error_message << "Keyword '" << k_token->lexem << "' not expected.";
            } else if (la_tag == Tag::NUMBER) {
                error_message << "Number '" << reinterpret_cast<Number*>(lookahead) << "' not expected.";
            } else { 
                error_message << "Unexpected symbol '" << static_cast<char>(la_tag) << "' found.";
            }
            
            // Expected tokens
            error_message << " Are expected:";
            
            TableAction EOF_action = parse_table[{PA_stack.top(), "EOF"}]; 
            if ((EOF_action.action)) error_message <<" EOF";

            TableAction id_action = parse_table[{PA_stack.top(), "id"}]; 
            if ((id_action.action)) error_message <<" id";

            TableAction number_action = parse_table[{PA_stack.top(), "number"}]; 
            if ((number_action.action)) error_message <<" number";

            std::vector<std::string> keywords = {"int", "-", "(", ")", "*", "/", ";", "+", "="};
            for (const auto& token : keywords)
            {
                TableAction keyword_action = parse_table[{PA_stack.top(), token}]; 
                if((keyword_action.action)) error_message <<" keyword";
            }

            // Print error
            std::cerr << error_message.str() << std::endl;

            // Error treatment - discard token and continue LOL
            if (la_tag == Tag::END){
                break;
            } else{
                lookahead = lexer.get_next_token(); 
            }
            
        }
    }

    return !syntax_error;
}
