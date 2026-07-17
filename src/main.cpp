////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include <quill/LogMacros.h>
#include <quill/SimpleSetup.h>
#include <AMORA/config.hpp>

#include "OpenMaVisApplication.hpp"

int main(const int argc, char *argv[]) {
    mavis::OpenMaVisApplication::beginLogging();

    MAVIS_INFO("Running OpenMaVis v{}", amora::config::info::APPLICATION_VERSION);

    try {
        mavis::OpenMaVisApplication app(argc, argv);
        app.run();
    } catch (std::exception& e) {
        MAVIS_ERROR("Exception occurred during runtime: {}", e.what());
        mavis::OpenMaVisApplication::endLogging();
        return EXIT_FAILURE;
    }
    mavis::OpenMaVisApplication::endLogging();
    return EXIT_SUCCESS;
}
