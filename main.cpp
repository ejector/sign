#include "commandlineapp.h"

#include <exception>
#include <iostream>

int main(int argc, char** argv)
{
    try {
        CommandLineApp app(argc, argv);
        return app.Exec();
    } catch (std::exception& e) {
        std::cout << "An errror occured: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "An errror occured. Exit." << std::endl;
    }
    return CommandLineApp::kExitWithError;
}
