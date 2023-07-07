#include <format>
#define NOMINMAX
#include "align.hpp"
#include "child.hpp"
#include "column.hpp"
#include "container.hpp"
#include "gestureDetector.hpp"
#include "image.hpp"
#include "mapWidget.hpp"
#include "row.hpp"
#include "text.hpp"
#include <any>
#include <limits>


using namespace squi;

Map::MapWidget::Impl::Impl(const MapWidget &args) : Widget(args.widget, Widget::Flags::Default()) {
	// setChildren({Column{
	// 	.widget{
	// 		.width = Size::Shrink,
	// 		.height = Size::Shrink,
	// 	},
	// 	.children{
	// 		[&]() -> Children {
	// 			Children ret{};
	// 			for (const auto &slice: mapData.chunks) {
	// 				Children ret2{};
	// 				for (const auto &item: slice)
	// 					ret2.children.push_back(Image{
	// 						.widget{
	// 							.width = 512.f,
	// 							.height = 512.f,
	// 						},
	// 						.fit = Image::Fit::none,
	// 						.image{Image::Data::fromFileAsync(item.fileName)},
	// 					});
	// 				ret.children.push_back(Row{
	// 					.widget{
	// 						.width = Size::Shrink,
	// 						.height = Size::Shrink,
	// 					},
	// 					.children{ret2},
	// 				});
	// 			}
	// 			return ret;
	// 		}(),
	// 	},
	// }});
	setChildren({Align{
		.child{
			Text{
				.text{"Loading..."},
			},
		},
	}});
}

void Map::MapWidget::Impl::onUpdate() {
	if (mapData.ready) {
		offset = {
			-static_cast<float>(mapData.offset[0]),
			-static_cast<float>(mapData.offset[1]),
		};
		setChildren({Column{
			.widget{
				.width = Size::Shrink,
				.height = Size::Shrink,
			},
			.children{
				[&]() -> Children {
					Children ret{};
					for (const auto &slice: mapData.chunks) {
						Children ret2{};
						for (const auto &item: slice) {
							Children columnChildren{};
							for (size_t y = 0; y < 8; y++) {
								Children rowChildren{};
								for (size_t x = 0; x < 8; x++) {
									rowChildren.children.push_back(Image{
										.widget{
											.width = 512.f,
											.height = 512.f,
											.sizeConstraints{
												.minWidth = 512.f,
												.minHeight = 512.f,
											},
										},
										.fit = Image::Fit::none,
										.image{Image::Data::fromFileAsync(std::format("tiles/{}/4_{}_{}.png", item.fileName.substr(0, item.fileName.find_last_of('.')), y, x))},
									});
								}
								columnChildren.children.push_back(Row{
									.widget{
										.width = Size::Shrink,
										.height = Size::Shrink,
									},
									.children{rowChildren},
								});
							}
							ret2.children.push_back(Column{
								.widget{
									.width = Size::Shrink,
									.height = Size::Shrink,
								},
								.children{columnChildren},
							});

							// ret2.children.push_back(Image{
							// 	.widget{
							// 		.width = 4096.f,
							// 		.height = 4096.f,
							// 	},
							// 	.fit = Image::Fit::none,
							// 	.image{Image::Data::fromFileAsync(item.fileName)},
							// });
						}
						ret.children.push_back(Row{
							.widget{
								.width = Size::Shrink,
								.height = Size::Shrink,
							},
							.children{ret2},
						});
					}
					return ret;
				}(),
			},
		}});
		mapData.ready = false;
	}
	const auto &gd = std::any_cast<GestureDetector::Storage>(state.properties.at("gestureDetector"));
	if (gd.active) {
		offset += gd.getDragDelta();
	}
}

void Map::MapWidget::Impl::layoutChildren(vec2 &maxSize, vec2 &minSize) {
	vec2 size{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
	Widget::layoutChildren(size, minSize);
}

void Map::MapWidget::Impl::arrangeChildren(vec2 &pos) {
	pos += offset;
	Widget::arrangeChildren(pos);
}