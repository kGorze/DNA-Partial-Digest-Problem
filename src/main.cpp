#include <iostream>
#include "../include/instance_generator.h"
#include "../include/test_framework.h"
#include "../include/map_solver.h"
#include "../include/global_paths.h"

int main() {
    GlobalPaths::createGlobalDirectories();

    InstanceGenerator generator;
    TestFramework framework(generator);

    framework.runInteractiveMode();
    return 0;
}
