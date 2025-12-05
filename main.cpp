#include <cstring>
#include <filesystem>
#include <iostream>

#include "configuration.hpp"

int main(int argc, char* argv[])
{
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        //TODO: print help page
        return 0;
    }

    configuration config = configuration::parse(argc, argv);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
