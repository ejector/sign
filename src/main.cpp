#include "commandlineapp.h"

#include <exception>
#include <iostream>

int main(int argc, char** argv)
{
    try {
        CommandLineApp app(argc, argv);
        return app.Exec();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "An errror occured. Exit" << std::endl;
    }
    return CommandLineApp::kExitWithError;
}
