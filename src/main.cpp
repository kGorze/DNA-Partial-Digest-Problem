#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <string>
#include <sstream>
#include <ctime>
#include <map>

#include "../include/instance_generator.h"
#include "../include/test_framework.h"
#include "../include/map_solver.h"

int main() {
    InstanceGenerator generator;
    TestFramework framework(generator);
    framework.runInteractiveMode();
    return 0;
}