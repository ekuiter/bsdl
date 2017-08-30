#include <iostream>
#include "app.hpp"

using namespace std;

static terminal& choose_terminal() {
    if (settings::get("app") == "main")
        return curses::main_terminal::reset_instance(setlocale(LC_ALL, ""));
    else if (settings::get("app") == "batch")
        return curses::terminal::instance();
    else
        throw runtime_error("invalid app type");
}

static app& choose_app() {
    if (settings::get("app") == "main")
        return app::instance();
    else if (settings::get("app") == "batch")
        return batch_app::instance();
    else
        throw runtime_error("invalid app type");
}

int main(int argc, char* argv[]) {
    curses::plain_terminal::instance();

    string error;
    try {
        settings::instance().read(vector<string>(argv, argv + argc));
    } catch (const std::exception& e) {
        error = e.what();
    }
    
    choose_terminal().run([argc, argv, &error]() {
        if (error != "")
            throw runtime_error(error);
        app& app = choose_app();
        app.initialize();
        vector<aggregators::series*> search_results = app.search_series();
        aggregators::series& series = app.choose_series(search_results);
        app.set_current_series(series);
        app.display_series(series);
    });
}
