//==============================================================================
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: MIT
//==============================================================================

#include <quill/LogMacros.h>
#include <quill/SimpleSetup.h>

#include "OpenMaVisApplication.hpp"

int main(const int argc, char *argv[]) {
    mavis::beginLogging();

    MAVIS_INFO("Running OpenMaVis v{}", mavis::APPLICATION_VERSION);

    try {
        mavis::OpenMaVisApplication app(argc, argv);
        app.run();
        app.terminate();
    } catch (std::exception& e) {
        MAVIS_ERROR("Exception occurred during runtime: {}", e.what());
        mavis::endLogging();
        return EXIT_FAILURE;
    }
    mavis::endLogging();
    return EXIT_SUCCESS;
}
