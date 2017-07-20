#include "series.hpp"
#include "../curses/terminal.hpp"

namespace aggregators {
    int series_base::max_aggregator_width = 13;
    
    void series_base::load() const {
        if (!loaded) {
            cout << "Loading series " << curses::color::get_accent_color() <<
                    title << curses::color::previous << "." << endl;
            load(request());
        }
    }
}
