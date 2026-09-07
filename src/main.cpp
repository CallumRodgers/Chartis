////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2026 Callum L. Rodgers
// SPDX-License-Identifier: GPL-3.0-or-later
// See the file COPYING at the root of Chartis repository for the full license text.
////////////////////////////////////////////////////////////////////////////////

#include <quill/LogMacros.h>
#include <quill/SimpleSetup.h>

#include "ChartisApplication.hpp"

int main(const int argc, char *argv[]) {
    chartis::beginLogging();

    CHARTIS_INFO("Running Chartis v{}", chartis::APPLICATION_VERSION);

    try {
        chartis::ChartisApplication app(argc, argv);
        app.run();
    } catch (std::exception& e) {
        CHARTIS_ERROR("Exception occurred during runtime: {}", e.what());
        chartis::endLogging();
        return EXIT_FAILURE;
    }
    chartis::endLogging();
    return EXIT_SUCCESS;
}
