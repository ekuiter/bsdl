#include <iostream>
#include "main_app.hpp"
#include "json_app.hpp"
#include "batch_app.hpp"
#include "monitor_app.hpp"
#include "http/client.hpp"
#include "util/cloudflare_hook.hpp"

using namespace std;

static terminal& choose_terminal() {
    if (settings::get("app") == "main" || settings::get("app") == "batch" || settings::get("app") == "monitor")
        return curses::main_terminal::reset_instance(setlocale(LC_ALL, ""));
    else if (settings::get("app") == "json")
        return curses::terminal::instance();
    else
        throw runtime_error("invalid app type");
}

static app& choose_app() {
    if (settings::get("app") == "main")
        return app::instance();
    else if (settings::get("app") == "json")
        return json_app::instance();
    else if (settings::get("app") == "batch")
        return batch_app::instance();
    else if (settings::get("app") == "monitor")
        return monitor_app::instance();
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

        http::client::instance().
            set_timeout(stoi(settings::get("timeout"))).
            add_hook(&util::cloudflare_hook::instance());
        
        app& app = choose_app();
        app.initialize();
        vector<aggregators::series*> search_results = app.search_series();
        aggregators::series& series = app.choose_series(search_results);
        app.set_current_series(series);
        app.display_series(series);
    });
}
