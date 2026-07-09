////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers                                            /
// SPDX-License-Identifier: MIT                                                    /
// Full license text available in the LICENSE file at the root of this repository. /
////////////////////////////////////////////////////////////////////////////////////

#include <quill/LogMacros.h>
#include <quill/SimpleSetup.h>

#include "OpenMaVisApplication.hpp"

constexpr auto VERSION = "0.1";

int main(const int argc, char *argv[]) {
    const auto logger = quill::simple_logger();
    LOG_INFO(logger, "Running OpenMaVis v{}", VERSION);

    try {
        MaVis::OpenMaVisApplication app(argc, argv);
        app.run();
    } catch (std::exception& e) {
        LOG_ERROR(logger, "Exception occurred during runtime: {}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
