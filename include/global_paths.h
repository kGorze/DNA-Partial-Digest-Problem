#ifndef GLOBAL_PATHS_H
#define GLOBAL_PATHS_H

#include <filesystem>

namespace fs = std::filesystem;

/**
 * Central place to define all directory and file paths.
 * Adjust BASE_DIR if you want a different root folder.
 */
namespace GlobalPaths {
    // Base directory for all output
    inline const fs::path BASE_DIR = "output";

    // Subdirectories
    inline const fs::path INSTANCES_DIR = BASE_DIR / "instances";
    inline const fs::path BENCHMARK_DIR = BASE_DIR / "benchmark";
    inline const fs::path TEMP_DIR      = BASE_DIR / "temp";

    // A common temp file for newly generated instances
    inline const fs::path TEMP_INSTANCE_FILE = TEMP_DIR / "temp_instance.txt";

    /**
     * Create all required directories (safe to call multiple times).
     */
    inline void createGlobalDirectories() {
        fs::create_directories(BASE_DIR);
        fs::create_directories(INSTANCES_DIR);
        fs::create_directories(BENCHMARK_DIR);
        fs::create_directories(TEMP_DIR);
    }
}

#endif // GLOBAL_PATHS_H
