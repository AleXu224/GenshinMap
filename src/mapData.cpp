#include "mapData.hpp"
#include "networking.hpp"
#include "mapInfo.hpp"
#include "fstream"

using namespace squi;

Map::MapData::MapData() {
	const auto response = Networking::get("https://sg-public-api-static.hoyolab.com/common/map_user/ys_obc/v1/map/info?map_id=2&app_sn=ys_obc&lang=en-us");
	if (!response.success) {
        printf("Error loading map info: %s\n", response.error.c_str());
        return;
    }

	const auto mapInfo = MapInfo::parse(response.body);

    const auto &details = mapInfo.data.info.detail;
    totalSize[0] = details.total_size[0];
    totalSize[1] = details.total_size[1];
    chunkSize = details.total_size[0] / details.slices.size();

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
            fileOutput << response.body;
            fileOutput.close();
        }
	}
}