#include "commandlineapp.h"

#include <iostream>

#include "commandlineparser.h"
#include "filesigner.h"

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
              << ", block size is "          << args.block_size   << " mebibytes" << std::endl;

    FileSigner file_signer;
    if (!file_signer.GenerateSign(args.input_file, args.output_file, args.block_size)) {
        std::cout << "Cannot generate sign fo file: " << file_signer.GetErrorString() << std::endl;
        return kExitWithError;
    }

    std::cout << "Signature was generated successfully" << std::endl;

    return 0;
}
