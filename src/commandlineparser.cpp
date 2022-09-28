#include "commandlineparser.h"

#include <stdexcept>

CommandLineParser::Args CommandLineParser::Parse(int argc, char** argv)
{
    constexpr int kInputFileArgPosition = 1;
    constexpr int kOutputFileArgPosition = 2;
    constexpr int kBlockSizeArgPosition = 3;
    constexpr int kMinimumArgumentsCount = 3;
    constexpr int kMaximumArgumentsCount = 4;

    Args args;

    if (argc < kMinimumArgumentsCount) {
        throw std::invalid_argument("Specify the command line arguments!\n"
                                    "Required:\n"
                                    "  path to input file to be signed\n"
                                    "  path to output file with signature\n"
                                    "Optional:\n"
                                    "  block size in mebibytes, by default 1 MiB\n"
                                    "Example:\n"
                                    "  sign /path/to/input/file/for/sign /path/to/output/file/with/sign 2\n");
    } else if (argc == kMinimumArgumentsCount) {
        args.input_file = argv[kInputFileArgPosition];
        args.output_file = argv[kOutputFileArgPosition];
    } else if (argc == kMaximumArgumentsCount) {
        try {
            args.block_size = std::stoul(argv[kBlockSizeArgPosition]) * 1024 * 1024;
        } catch (const std::exception& e) {
            throw std::invalid_argument("Cannot convert block size to integer");
        }

    }

    return args;
}
