#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <string>


class CommandLineParser
{
private:
    struct Args
    {
        std::string input_file;
        std::string output_file;
        size_t block_size = 1024 * 1024;
    };
public:
    [[nodiscard]] Args GetArgs(int argc, char** argv) const;
};

#endif // COMMANDLINEPARSER_H
