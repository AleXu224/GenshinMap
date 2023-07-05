#pragma once
#include "widget.hpp"

namespace Map {
    struct MapWidget {
        // Args
        squi::Widget::Args widget;
    
        class Impl : public squi::Widget {
            // Data
    
        public:
            Impl(const MapWidget &args);

            void onArrange(squi::vec2 &pos) override;
        };
    
        operator squi::Child() const {
            return std::make_shared<Impl>(*this);
        }
    };
}