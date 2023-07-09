#include "networking.hpp"
#include "mapData.hpp"
#include "mapInfo.hpp"
#include "fstream"
#include "filesystem"
#include "stb_image.h"
#include "format"
#include <array>
#include <iterator>
#include <vector>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace squi;

Map::MapData::MapData() {
    loader = std::async(std::launch::async, &MapData::load, this);
}

std::vector<std::vector<unsigned char>> createTiles(const unsigned char* data, int width, int height, int channels) {
    std::vector<std::vector<unsigned char>> tiles;
    // Level 4
    for (int y = 0; y < height; y += 512) {
        for (int x = 0; x < width; x += 512) {
            // Create tile
            std::vector<unsigned char> tile(512 * 512 * 4);
            for (int ty = 0; ty < 512; ty++) {
                for (int tx = 0; tx < 512; tx++) {
                    // Copy pixel
                    tile[(ty * 512 + tx) * 4 + 0] = data[((y + ty) * width + (x + tx)) * 4 + 0];
                    tile[(ty * 512 + tx) * 4 + 1] = data[((y + ty) * width + (x + tx)) * 4 + 1];
                    tile[(ty * 512 + tx) * 4 + 2] = data[((y + ty) * width + (x + tx)) * 4 + 2];
                    tile[(ty * 512 + tx) * 4 + 3] = data[((y + ty) * width + (x + tx)) * 4 + 3];
                }
            }
            // Save tile
            tiles.push_back(tile);
        }
    }
    return tiles;
}

void Map::MapData::load() {
	const auto response = Networking::get("https://sg-public-api-static.hoyolab.com/common/map_user/ys_obc/v1/map/info?map_id=2&app_sn=ys_obc&lang=en-us");
	if (!response.success) {
		printf("Error loading map info: %s\n", response.error.c_str());
		return;
	}

	const auto mapInfo = MapInfo::parse(response.body);
    offset[0] = mapInfo.data.info.detail.origin[0];
    offset[1] = mapInfo.data.info.detail.origin[1];

	const auto &details = mapInfo.data.info.detail;
	totalSize[0] = details.total_size[0];
	totalSize[1] = details.total_size[1];
	chunkSize = details.total_size[0] / details.slices.size();

    std::filesystem::create_directory("tiles");

	for (const auto &slice: mapInfo.data.info.detail.slices) {
		if (slice.empty()) continue;
		chunks.emplace_back();
		chunks.back().reserve(slice.size());
		// From this
		// https://act.hoyoverse.com/map_manage/20230316/e9df2d03fdc71af6641c9e5b909e04d8_2660054586494739343.png
		// To this
		// 20230316_e9df2d03fdc71af6641c9e5b909e04d8_2660054586494739343.png
		for (const auto &item: slice) {
			auto fileName = item.url.substr(37);
			fileName = fileName.replace(fileName.find_first_of('/'), 1, "_");
			// printf("%s\n", fileName.c_str());
			chunks.back().emplace_back(fileName);
			// Check if file exists
			std::ifstream file(fileName, std::ios::binary);
			if (file.good()) {
				file.close();
				printf("File %s already exists\n", fileName.c_str());
				continue;
			}
			file.close();

			std::ofstream fileOutput(fileName, std::ios::binary);


			// Download file
			printf("Downloading %s\n", item.url.c_str());
			const auto response = Networking::get(item.url);
			if (!response.success) {
				printf("Error downloading map chunk: %s\n", response.error.c_str());
				continue;
			}

            // Setup tiles for this chunk
            std::filesystem::create_directory("tiles/" + fileName.substr(0, fileName.find_last_of('.')));

            // Chunks are 4096x4096 images
            // They will be resized to 512x512 tiles
            // There will also be a varying amount of tiles per chunk based on the zoom level
            // zool level 1: 1 tile
            // zoom level 2: 4 tiles
            // zoom level 3: 16 tiles
            // zoom level 4: 64 tiles
            // The name format will be:
            // zoom_y_x.png

            // Load image
            printf("Generating tiles\n");
            int width, height, channels;
            stbi_uc *data = stbi_load_from_memory((const stbi_uc*)response.body.data(), (int) response.body.size(), &width, &height, &channels, 0);

            std::string directoryName = std::format("tiles/{}", fileName.substr(0, fileName.find_last_of('.')));
            // Level 4
            {
                printf("1/4\n");
                auto tiles = createTiles(data, width, height, channels);
                for (size_t i = 0; i < tiles.size(); i++) {
					std::string fileName = std::format("{}/4_{}_{}.png", directoryName, i / 8, i % 8);
					stbi_write_png(fileName.c_str(), 512, 512, 4, tiles[i].data(), 512 * 4);
				}
			}

            // Level 3
            {
                printf("2/4\n");
                std::vector<stbi_uc> datal3{};
                datal3.resize(2048 * 2048 * channels);
                stbir_resize_uint8(data, width, height, 0, datal3.data(), 2048, 2048, 0, channels);
                auto tiles = createTiles(datal3.data(), 2048, 2048, channels);
                for (size_t i = 0; i < tiles.size(); i++) {
                    std::string fileName = std::format("{}/3_{}_{}.png", directoryName, i / 4, i % 4);
                    stbi_write_png(fileName.c_str(), 512, 512, channels, tiles[i].data(), 512 * channels);
                }
            }

            // Level 2
            {
                printf("3/4\n");
                std::vector<stbi_uc> datal2{};
                datal2.resize(1024 * 1024 * channels);
                stbir_resize_uint8(data, width, height, 0, datal2.data(), 1024, 1024, 0, channels);
                auto tiles = createTiles(datal2.data(), 1024, 1024, channels);
                for (size_t i = 0; i < tiles.size(); i++) {
                    std::string fileName = std::format("{}/2_{}_{}.png", directoryName, i / 2, i % 2);
                    stbi_write_png(fileName.c_str(), 512, 512, channels, tiles[i].data(), 512 * channels);
                }
            }

            // Level 1
            {
                printf("4/4\n");
                std::vector<stbi_uc> datal1{};
                datal1.resize(512 * 512 * channels);
                stbir_resize_uint8(data, width, height, 0, datal1.data(), 512, 512, 0, channels);
                auto tiles = createTiles(datal1.data(), 512, 512, channels);
                for (size_t i = 0; i < tiles.size(); i++) {
                    std::string fileName = std::format("{}/1_{}_{}.png", directoryName, i / 1, i % 1);
                    stbi_write_png(fileName.c_str(), 512, 512, channels, tiles[i].data(), 512 * channels);
                }
            }

            stbi_image_free(data);
            

			fileOutput << response.body;
			fileOutput.close();
		}
	}

    // Generate tiles for the map
    for (size_t index = 0; index < zoomLevels.size() && index< zoomLevels.size(); index++) {
        const auto& zoomLevel = zoomLevels[index];
        const auto& rowCount = tileRowCount[index];

        // Reserve enough space for the tiles
        tiles[zoomLevel] = std::vector<std::vector<Chunk>>(rowCount * totalSize[1] / chunkSize, std::vector<Chunk>(rowCount * totalSize[0] / chunkSize, Chunk{{0, 0}, {0, 0}}));
        const auto& tileSize = chunkSize / rowCount;


        for (auto chunkRow = chunks.begin(); chunkRow != chunks.end(); chunkRow++) {
            const auto yDistance = std::distance(chunks.begin(), chunkRow);
            for (auto chunk = chunkRow->begin(); chunk != chunkRow->end(); chunk++) {
                const auto xDistance = std::distance(chunkRow->begin(), chunk);
                const std::string path = "tiles/" + chunk->fileName.substr(0, chunk->fileName.find_last_of('.'));

                for (size_t y = 0; y < rowCount; y++) {
                    for (size_t x = 0; x < rowCount; x++) {
                        const std::string fileName = std::format("{}/{}_{}_{}.png", path, zoomLevel, y, x);
						tiles[zoomLevel][yDistance * rowCount + y][xDistance * rowCount + x] = Chunk{
							.fileName = fileName,
							.rect = Rect{
								{
									static_cast<float>(xDistance * chunkSize + x * tileSize),
									static_cast<float>(yDistance * chunkSize + y * tileSize),
								},
								{
									static_cast<float>(xDistance * chunkSize + (x + 1) * tileSize),
									static_cast<float>(yDistance * chunkSize + (y + 1) * tileSize),
								}},
						};
					}
                }
            }
        }
    }

    
    ready = true;
}