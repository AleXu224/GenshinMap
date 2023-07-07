#pragma once
#include <array>
#include <fstream>
#include <future>
#include <vector>
#include "string"

namespace Map {
    struct MapChunk {
        std::string fileName;
    };
    
    struct MapData {
        std::vector<std::vector<MapChunk>> chunks{};
        std::array<uint64_t, 2> totalSize{0, 0};
        uint64_t chunkSize = 0;
        std::array<uint64_t, 2> offset{0, 0};
        bool ready = false;

        MapData();
    private:
        void load();
        std::future<void> loader;
    };
}