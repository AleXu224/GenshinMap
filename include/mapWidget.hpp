#pragma once
#include "gestureDetector.hpp"
#include "image.hpp"
#include "mapData.hpp"
#include "rect.hpp"
#include "widget.hpp"
#include <optional>
#include <unordered_map>


namespace Map {
	struct MapWidget {
		// Args
		squi::Widget::Args widget;

		class Impl : public squi::Widget {
			// Data
			MapData mapData{};
			squi::vec2 offset{};
			struct Tile {
				std::shared_ptr<squi::Image::Impl> image;
				squi::Rect rect;
			};
			std::unordered_map<std::string, Tile> tiles{};
			bool loaded = false;
			uint16_t zoomIndex = 3;
			float zoom = 0;

		public:
			Impl(const MapWidget &args);

            void onUpdate() override;
			void updateChildren() override;
			void drawChildren() override;
		};

		operator squi::Child() const {
			return squi::GestureDetector{
				.child{std::make_shared<Impl>(*this)},
			};
		}
	};
}// namespace Map