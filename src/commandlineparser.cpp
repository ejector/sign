#include "commandlineparser.h"

#include <stdexcept>

CommandLineParser::Args CommandLineParser::Parse(int argc, char** argv)
{
    Args args;
    if (argc < 3) {
        throw std::invalid_argument("Specify the command line arguments!\n"
                                    "Required:\n"
                                    "  path to input file to be signed\n"
                                    "  path to output file with signature\n"
                                    "Optional:\n"
                                    "  block size in mebibytes, by default 1 MiB\n"
                                    "Example:\n"
                                    "  sign /path/to/input/file/for/sign /path/to/output/file/with/sign 2\n");
    }
    if (argc == 3) {
        args.input_file = argv[1];
        args.output_file = argv[2];
    } else if (argc == 4) {
        args.block_size = std::stoul(argv[3]) * 1024 * 1024;
    }

    return args;
}
