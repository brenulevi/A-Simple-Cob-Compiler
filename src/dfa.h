#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "glob.h"

class DFA
{
public:
    DFA() : initial_state{0} {};

    void set_initial_state(unsigned int s);
    void push_final_state(unsigned int s);
    void push_transition(unsigned int from, unsigned int to, char symbol);

    bool check(std::string& str);

private:
    std::unordered_map<std::pair<unsigned int, char>, unsigned int, glob::pair_hash> transitions;
    std::vector<unsigned int> final_states;
    unsigned int initial_state;
};