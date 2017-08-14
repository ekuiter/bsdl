#include <iostream>
#include "app.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    curses::terminal::instance(setlocale(LC_ALL, "")).run([argc, argv](curses::terminal& terminal) {
        settings::instance().read(vector<string>(argv, argv + argc));
        app& app = app::instance();
        app.initialize();
        vector<aggregators::series*> search_results = app.search_series();
        aggregators::series& series = app.choose_series(search_results);
        app.set_current_series(series);
        app.display_series(series);
    });
}
