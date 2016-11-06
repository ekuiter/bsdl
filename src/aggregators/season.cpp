#include "season.hpp"
#include "season_view.hpp"

namespace aggregators {
    void season_base::load() const {
        if (!loaded) {
            cout << "Loading " << curses::color::get_accent_color() <<
                    *this << curses::color::previous << "." << endl;
            load(request());
        }
    }

    void season_base::create_view(curses::window& window) {
        if (view)
            throw exception("season view already exists");
        view.reset(new season_view(static_cast<season&>(*this), window));
    }

    void season_base::destroy_view() {
        view.reset();
    }
}
