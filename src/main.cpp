#include <iostream>
#include "../include/instance_generator.h"
#include "../include/test_framework.h"
#include "../include/map_solver.h"
#include "../include/global_paths.h"

/**
 * Main entry point:
 * - Creates the InstanceGenerator
 * - Creates TestFramework
 * - Calls the interactive mode
 */
int main() {
    // Initialize global directories
    GlobalPaths::createGlobalDirectories();

    // Create the generator and the framework
    InstanceGenerator generator;
    TestFramework framework(generator);

    framework.runInteractiveMode();
    return 0;
}
