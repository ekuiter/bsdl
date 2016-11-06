#include "series.hpp"
#include "../curses/terminal.hpp"

namespace aggregators {
    void series_base::load() const {
        if (!loaded) {
            cout << "Loading series " << curses::color::get_accent_color() <<
                    *this << curses::color::previous << "." << endl;
            load(request());
        }
    }
}
