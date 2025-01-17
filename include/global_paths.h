#ifndef GLOBAL_PATHS_H
#define GLOBAL_PATHS_H

#include <filesystem>

namespace fs = std::filesystem;


namespace GlobalPaths {
    inline const fs::path BASE_DIR = "output";

    inline const fs::path INSTANCES_DIR = BASE_DIR / "instances";
    inline const fs::path BENCHMARK_DIR = BASE_DIR / "benchmark";
    inline const fs::path TEMP_DIR      = BASE_DIR / "temp";

    inline const fs::path TEMP_INSTANCE_FILE = TEMP_DIR / "temp_instance.txt";

    inline void createGlobalDirectories() {
        fs::create_directories(BASE_DIR);
        fs::create_directories(INSTANCES_DIR);
        fs::create_directories(BENCHMARK_DIR);
        fs::create_directories(TEMP_DIR);
    }
}

#endif // GLOBAL_PATHS_H
