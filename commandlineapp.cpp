#include "commandlineapp.h"

#include <iostream>

#include "commandlineparser.h"

CommandLineApp::CommandLineApp(int argc, char** argv)
    : argc_(argc),
      argv_(argv)
{
}

int CommandLineApp::Exec()
{
    auto&& args = CommandLineParser::Parse(argc_, argv_);

    std::cout << "File to be signed is "     << args.input_file
              << ", file with signature is " << args.output_file
              << ", block size is "          << args.block_size   << " bytes" << std::endl;

    return 0;
}
