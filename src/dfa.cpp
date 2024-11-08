#include "dfa.h"

void DFA::set_initial_state(unsigned int s)
{
    initial_state = s;
}

void DFA::push_final_state(unsigned int s)
{
    final_states.push_back(s);
}

void DFA::push_transition(unsigned int from, unsigned int to, char symbol)
{
    transitions[{from, symbol}] = to;
}

bool DFA::check(std::string &str)
{
    unsigned int current_state = initial_state;

    for (int i = 0; i < static_cast<int>(str.size()); i++)
    {
        char symbol = str[i];

        try
        {
            current_state = transitions.at({current_state, symbol});
        }
        catch (std::exception e)
        {
            return false;
        }
    }

    for (int i = 0; i < static_cast<int>(final_states.size()); i++)
    {
        if (final_states[i] == current_state)
            return true;
    }

    return false;
}
