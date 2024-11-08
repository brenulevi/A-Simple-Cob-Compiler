#pragma once

#include <fstream>
#include <vector>

namespace glob
{
    // file passed via argument (source file)
    extern std::fstream file;

    // aux struct to unordered maps hash with pair
    struct pair_hash
    {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2> &pair) const
        {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };
}