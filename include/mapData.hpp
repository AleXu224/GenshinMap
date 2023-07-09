#pragma once
#include <array>
#include <fstream>
#include <future>
#include <unordered_map>
#include <vector>
#include "string"
#include "image.hpp"
#include "rect.hpp"

namespace Map {
	constexpr std::array<uint16_t, 4> zoomLevels = {1, 2, 3, 4};
	constexpr std::array<uint16_t, 4> tileRowCount = {1, 2, 4, 8};

	struct MapChunk {
        std::string fileName;
    };

	struct Chunk {
		std::string fileName;
		squi::Rect rect;
	};

	struct MapData {
        std::vector<std::vector<MapChunk>> chunks{};
        // A 2d array of chunks for each zoom level
        std::unordered_map<uint16_t, std::vector<std::vector<Chunk>>> tiles{};
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