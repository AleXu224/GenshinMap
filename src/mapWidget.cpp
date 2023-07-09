#include "mapWidget.hpp"
#include "align.hpp"
#include "child.hpp"
#include "column.hpp"
#include "container.hpp"
#include "gestureDetector.hpp"
#include "image.hpp"
#include "mapData.hpp"
#include "rect.hpp"
#include "renderer.hpp"
#include "row.hpp"
#include "text.hpp"
#include "vec2.hpp"
#include <algorithm>
#include <any>
#include <format>
#include <limits>
#include <memory>


using namespace squi;

Map::MapWidget::Impl::Impl(const MapWidget &args) : Widget(args.widget, Widget::Flags::Default()) {
	setChildren({Align{
		.child{
			Text{
				.text{"Loading..."},
			},
		},
	}});
}

void Map::MapWidget::Impl::onUpdate() {
	if (!mapData.ready) return;
	if (!loaded) {
		offset = {
			-static_cast<float>(mapData.offset[0]),
			-static_cast<float>(mapData.offset[1]),
		};
		loaded = true;
		setChildren({});
	}

	const auto inverseLerp = [](const vec2 &a, const vec2 &b, const vec2 &value) {
		return (value - a) / (b - a);
	};

	const auto &gd = std::any_cast<GestureDetector::Storage>(state.properties.at("gestureDetector"));
	if (gd.hovered) {
		const auto oldZoom = zoom;
		zoom -= gd.scrollDelta.y / 2;
		zoom = std::clamp(zoom, -1.f, 3.f);

		if (gd.scrollDelta.y != 0) {
			uint16_t newZoomIndex;
			if (zoom < 1)
				newZoomIndex = 3;
			else if (zoom < 2)
				newZoomIndex = 2;
			else if (zoom < 3)
				newZoomIndex = 1;
			else
				newZoomIndex = 0;

			// Zoom to mouse position
			const auto zoomMultiplier = static_cast<float>(std::pow(2.f, oldZoom));
			const auto newZoomMultiplier = static_cast<float>(std::pow(2.f, zoom));
			const auto sizeMultiplier = zoomMultiplier / newZoomMultiplier;
			const auto mousePos = GestureDetector::getMousePos();
			const auto rect = getRect();
			offset += inverseLerp(rect.getTopLeft(), rect.size(), mousePos) * rect.size() * (1 - sizeMultiplier) * newZoomMultiplier;

			if (newZoomIndex != zoomIndex) {
				zoomIndex = newZoomIndex;
				tiles.clear();
			}
		}
	}
	const auto zoomMultiplier = std::pow(2.f, zoom);
	if (gd.active) {
		offset += gd.getDragDelta() * zoomMultiplier;
	}

	const auto &clipRect = getRect();
	const auto offsetClipRect = [&]() -> Rect {
		auto ret = Rect{
			clipRect.getTopLeft() * zoomMultiplier,
			clipRect.size() * zoomMultiplier,
		};
		ret.offset(-offset);
		return ret;
	}();
	uint16_t tileSize = mapData.chunkSize / Map::tileRowCount[zoomIndex];

	auto left = static_cast<uint16_t>(offsetClipRect.left / static_cast<float>(tileSize));
	auto top = static_cast<uint16_t>(offsetClipRect.top / static_cast<float>(tileSize));
	auto right = static_cast<uint16_t>(offsetClipRect.right / static_cast<float>(tileSize));
	auto bottom = static_cast<uint16_t>(offsetClipRect.bottom / static_cast<float>(tileSize));

	const uint16_t maxTileCountX = mapData.totalSize[0] / tileSize - 1;
	const uint16_t maxTileCountY = mapData.totalSize[1] / tileSize - 1;
	left = std::clamp(left, (uint16_t) 0, maxTileCountX);
	top = std::clamp(top, (uint16_t) 0, maxTileCountY);
	right = std::clamp(right, (uint16_t) 0, maxTileCountX);
	bottom = std::clamp(bottom, (uint16_t) 0, maxTileCountY);

	for (uint16_t x = left; x <= right; ++x) {
		for (uint16_t y = top; y <= bottom; ++y) {
			auto &tile = mapData.tiles.at(zoomLevels[zoomIndex]).at(y).at(x);
			if (tiles.contains(tile.fileName)) continue;
			tiles.emplace(tile.fileName, Tile{
											 .image{
												 std::make_shared<Image::Impl>(Image{
													 .image{Image::Data::fromFileAsync(tile.fileName)},
												 }),
											 },
											 .rect = tile.rect,
										 });
		}
	}

	for (auto it = tiles.begin(); it != tiles.end();) {
		if (!it->second.rect.intersects(offsetClipRect)) {
			it = tiles.erase(it);
		} else {
			++it;
		}
	}
}

void Map::MapWidget::Impl::updateChildren() {
	for (auto &[key, child]: tiles) {
		child.image->state.parent = this;
		child.image->update();
	}
}

void Map::MapWidget::Impl::drawChildren() {
	auto &renderer = Renderer::getInstance();
	const auto zoomMultiplier = std::pow(2.f, zoom);

	for (auto &[key, child]: tiles) {
		auto &quad = child.image->getQuad();
		quad.setPos((child.rect.getTopLeft() + offset) / zoomMultiplier);
		quad.setSize(child.rect.size() / zoomMultiplier);

		renderer.addQuad(quad);
	}
}