#include "performanceOverlay.hpp"
#include "row.hpp"
#define WIN32_LEAN_AND_MEAN

#include "box.hpp"
#include "glt/include/window.hpp"
#include "image.hpp"
#include "networking.hpp"
#include "scrollableFrame.hpp"
#include <iostream>
#include "mapData.hpp"

int main(int, char **) {
	using namespace squi;

	// const auto response = Networking::get("https://cdn.discordapp.com/attachments/301985131676303362/1125794921400520765/image.png");
	// const auto response = Networking::get("https://sg-public-api.hoyolab.com/common/map_user/"
	// 									  "ys_obc/v1/map/list?map_id=2&app_sn=ys_obc&lang=en-us");

	// if (response.success) std::cout << response.body << std::endl;
	// else
	// 	std::cout << "Error: " << response.error << std::endl;

	// const auto mapList = MapList::parse(response.body);
	// for (const auto &item : mapList.data.list)
	// 	std::cout << item.id << " " << item.is_refresh << std::endl;

	auto mapData = Map::MapData();

	Window window{};
	window.addChild(ScrollableFrame{
		.children{[&mapData]() -> Children {
			Children ret{};
			for (const auto &slice : mapData.chunks) {
				Children ret2{};
				for (const auto &item : slice)
					ret2.children.push_back(Image{
						.widget{
							.width = 512.f,
							.height = 512.f,
						},
						.fit = Image::Fit::none,
						.image{Image::Data::fromFileAsync(item.fileName)}
					});
				ret.children.push_back(Row{
					.widget{
						.width = Size::Shrink,
						.height = Size::Shrink,
					},
					.children{ret2},
				});
			}
			return ret;
		}()}});
	window.addChild(PerformanceOverlay{});
	window.run();
}
