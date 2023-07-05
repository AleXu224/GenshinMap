#include "panable.hpp"
#include "child.hpp"
#include "column.hpp"
#include "container.hpp"
#include <limits>

using namespace squi;

Map::MapWidget::Impl::Impl(const MapWidget &args) : Widget(args.widget, Widget::Flags::Default()) {
	setChildren({
		Column{
			.widget{
				.width = Size::Shrink,
				.height = Size::Shrink,
                .sizeConstraints{
                    .maxWidth = std::numeric_limits<float>::max(),
                    .maxHeight = std::numeric_limits<float>::max(),
                },
			},
		},
	});
}

void Map::MapWidget::Impl::onArrange(vec2 &pos) {
    
}