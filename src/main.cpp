#include <iostream>

#include "glob.h"
#include "parser.h"

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        std::cout << "invalid ascc usage: use 'ascc source.cbb'" << std::endl;
        return 1;
    }

    glob::file.open(argv[1]);
    if(!glob::file.is_open())
    {
        std::cout << "error when opening file: " << argv[1] << std::endl;
        return 1;
    }

    Parser parser;
    parser.parse();

    return 0;
}