#include <iostream>
#include "app/app.hpp"

using namespace std;

int main(int argc, char* argv[]) {
#ifndef __MINGW32__
    setlocale(LC_ALL, "");
#endif

    curses::terminal::instance().run([argc, argv](curses::terminal& terminal) {
        settings::instance().read(vector<string>(argv, argv + argc));
        app& app = app::instance();
        unique_ptr<vector<bs::series>> search_results = app.search_series();
        bs::series& series = app.choose_series(*search_results);
        app.display_series(series);
    });

    return EXIT_SUCCESS;
}
