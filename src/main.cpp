#include <iostream>

#include "OpenMaVisApplication.h"

int main(const int argc, char *argv[]) {
    std::cout << "Running OpenMaVis v0.1" << std::endl;

    try {
        OpenMaVisApplication app(argc, argv);
        app.run();
    } catch (std::exception& e) {
        std::cerr << "Exception occurred during runtime: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
