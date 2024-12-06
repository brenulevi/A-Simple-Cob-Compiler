#include "lexer.h"
#include "glob.h"

#include <iostream>

Lexer::Lexer()
    : peek{' '}, current_line{1}
{
    // initialize keyword automata
    // int keyword
    keyword_automata.push_transition(0, 1, 'i');
    keyword_automata.push_transition(1, 2, 'n');
    keyword_automata.push_transition(2, 255, 't');

    keyword_automata.push_final_state(255);

    // initialize number automata
    for (char i = 48; i <= 57; i++)
    {
        number_automata.push_transition(0, 255, i);
        number_automata.push_transition(255, 255, i);
    }
    number_automata.push_final_state(255);

    // initialize id automata
    for (char i = 65; i <= 90; i++)
    {
        id_automata.push_transition(0, 255, i);
        id_automata.push_transition(0, 255, i + 32);

        id_automata.push_transition(255, 255, i);
        id_automata.push_transition(255, 255, i + 32);
    }

    id_automata.push_transition(0, 255, '_');
    id_automata.push_transition(255, 255, '_');

    for (char i = 48; i <= 57; i++)
    {
        id_automata.push_transition(255, 255, i);
    }

    id_automata.push_final_state(255);
}

Token *Lexer::get_next_token()
{
    // ignore white spaces [" ", "\t", "\n"]
    while (std::isspace(peek))
    {
        if (peek == '\n')
        {
            ++current_line;
        }
        
        peek = glob::file.get();
    }

    if (std::isdigit(peek))
    {
        std::string buf;

        do
        {
            buf.push_back(peek);
            peek = glob::file.get();
        } while (isdigit(peek));

        // verify if automata accept this
        if (number_automata.check(buf))
        {
            token.n = {Tag::NUMBER, buf};

            return &token.n;
        }

        std::cout << "lexical error: invalid token " << buf << std::endl;
    }

    if (isalpha(peek) || peek == '_')
    {
        std::string buf;

        do
        {
            buf.push_back(peek);
            peek = glob::file.get();
        } while (isalnum((peek)) || peek == '_');

        // verify with automatas if is an id or keyword
        if (keyword_automata.check(buf))
        {
            token.k = {Tag::KEYWORD, buf};

            return &token.k;
        }

        if (id_automata.check(buf))
        {
            token.i = {Tag::ID, buf};

            return &token.i;
        }

        std::cout << "lexical error: invalid token " << buf << std::endl;
    }

    // operator
    Token t{peek};

    peek = glob::file.get();
    token.t = t;

    return &token.t;
}