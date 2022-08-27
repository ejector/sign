#include "commandlineapp.h"

#include "commandlineparser.h"

CommandLineApp::CommandLineApp(int argc, char** argv)
    : argc_(argc),
      argv_(argv)
{
}

int CommandLineApp::Exec()
{
    auto&& args = CommandLineParser::Parse(argc_, argv_);
    return 0;
}
