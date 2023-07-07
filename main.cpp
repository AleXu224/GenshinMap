#include "mapWidget.hpp"
#include "performanceOverlay.hpp"
#define WIN32_LEAN_AND_MEAN

#include "box.hpp"
#include "glt/include/window.hpp"

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
	Window window{};
	window.addChild(Box{
		.color{Color::HEX(0x17181CFF)},
		.child{Map::MapWidget{}},
	});
	window.addChild(PerformanceOverlay{});
	window.run();
}
