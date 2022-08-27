#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <string>

class CommandLineParser final
{
private:
    struct Args
    {
        std::string input_file;
        std::string output_file;
        size_t block_size = 1024 * 1024;
    };
public:
    [[nodiscard]] static Args Parse(int argc, char** argv);
};

#endif // COMMANDLINEPARSER_H
