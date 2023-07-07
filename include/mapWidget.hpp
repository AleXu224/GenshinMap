#pragma once
#include "gestureDetector.hpp"
#include "image.hpp"
#include "mapData.hpp"
#include "rect.hpp"
#include "widget.hpp"
#include <optional>


namespace Map {
	struct MapWidget {
		// Args
		squi::Widget::Args widget;

		class Impl : public squi::Widget {
			// Data
			MapData mapData{};
            squi::vec2 offset{};

		public:
			Impl(const MapWidget &args);
			struct Chunk {
				std::string path;
				std::optional<squi::Image::Impl> image;
			};
			// Map of levels of vectors of vectors of chunks

            void onUpdate() override;
            void layoutChildren(squi::vec2 &maxSize, squi::vec2 &minSize) override;
            void arrangeChildren(squi::vec2 &pos) override;
		};

		operator squi::Child() const {
			return squi::GestureDetector{
				.child{std::make_shared<Impl>(*this)},
			};
		}
	};
}// namespace Map