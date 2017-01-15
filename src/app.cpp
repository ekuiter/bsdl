#include "app.hpp"
#include "util/platform.hpp"
#include "aggregators/episode_download.hpp"
#include "curses/platform.hpp"
#include "util/download_dialog.hpp"
#include "util/addressed.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <fstream>

app::app():
        terminal(terminal::instance()), settings(settings::instance()),
        status_window(rectangle(0, 0, COLS / 3, LINES)),
        title_window(status_window, rectangle(0, 0, status_window.get_bounds().width, 2)),
        title_height(0, title_window.get_bounds().height),
        log_window(status_window, rectangle(title_height, status_window.get_dimensions() - title_height)),
        spinner(log_window), current_series(nullptr) {

    log_window.set_scrolling(true);
    terminal.get_stream(cout).set_window(log_window).set_wrap(true);
    terminal.get_stream(cerr).set_color(color(COLOR_RED)).set_window(log_window).set_wrap(true);
    terminal.get_stream(clog).set_visible(false);

    if (settings.is_set("log_file")) {
        log_file.open(settings["log_file"]);
        terminal.get_stream(cout).set_log_file(&log_file);
        terminal.get_stream(cerr).set_log_file(&log_file);
        terminal.get_stream(clog).set_log_file(&log_file);
    }

    using namespace placeholders;
    terminal.get_input().register_keyboard_callback(bind(&app::keyboard_callback, this, _1));
    http::client::instance().set_callback(bind(&app::http_callback, this, _1, _2, _3, _4, _5));

    set_title("bsdl");
    cout << "bsdl initialized." << endl;
    clog << endl << "Settings:" << endl << settings << endl;

    if (settings.is_set("show_info")) {
        window::framed message_window(get_centered_bounds());
        if (settings["show_info"] == "help") {
            message_dialog::run(message_window,
                                "usage: bsdl [series] [--download [season [episode]]] [-d [season [episode]]] "
                                        "[--aggregator aggregator] [-a aggregator] "
                                        "[--provider provider] [-p provider] "
                                        "[--output-files dir] [-o dir] "
                                        "[--rename-files [dir [pattern]]] [-r [dir [pattern]]] "
                                        "[--log-file [file]] [-l [file]] "
                                        "[--config-file file] [-c file]", "Exit");
        }
        if (settings["show_info"] == "version") {
            message_dialog::run(message_window, [&message_window](stream& _stream) {
                _stream.set_wrap(false);
                _stream << "bsdl 1.5.0" << endl <<
                        stream::write(stream::ext_char(ACS_HLINE), message_window.get_bounds().width) <<
                        "Source code: https://github.com/ekuiter/bsdl" << endl;
                _stream.set_wrap(true);
                _stream << "Made with " << color(COLOR_RED) << "<3" << color::previous <<
                        " (and Boost, curses, curl, curlcpp, gumbo-query, gumbo-parser, nlohmann/json)." << endl <<
						"Compiled for " << color::get_accent_color() << util::platform::get_name() <<
						color::previous << " on " << color::get_accent_color() <<
                                                __DATE__ << " " << __TIME__ << color::previous << ".";
            }, "Exit");
        }
        exit(EXIT_SUCCESS);
    }

    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
}

void app::set_title(const string& _title, bool set_notice, string notice) {
    if (set_notice) {
        auto left_bracket = title.find_last_of('['), right_bracket = title.find_last_of(']');
        if (left_bracket && right_bracket && left_bracket < right_bracket)
            title = title.substr(0, left_bracket - 1);
        if (notice != "")
            title += " [" + notice + "]";
    } else {
        title = _title;
        clog << endl << title << endl << stream::write("-", title.length()) << endl;
    }
    stream stream(title_window);
    stream << stream::clear() << title << endl <<
		stream::write(stream::ext_char(ACS_HLINE), title_window.get_bounds().width) << stream::refresh();
}

bool app::keyboard_callback(int ch) {
    if (ch == 27) // ESC
        exit(EXIT_SUCCESS);
    return true;
}

int app::http_callback(http::request::status status, const http::request& request,
                  curl_off_t now_bytes, curl_off_t total_bytes, curl::curl_easy_exception* e) {
    static int i;

    if (i++ % 2)
        spinner.spin();

    if (status & http::request::status::BEGIN) {
        terminal.get_input().override_blocking(false);
        terminal.get_stream(cout).set_log_file(nullptr);
        cout << stream::advance(spinner.length()) << stream::write(request.get_host(), 1, false) <<
                stream::allow_wrap() << stream::back(spinner.length()) << stream::refresh();
        terminal.get_stream(cout).set_log_file(&log_file);
    }

    terminal.get_input().read([this](int ch) {
        keyboard_callback(ch);
        return true;
    });

    if (status & http::request::status::END) {
        spinner.stop(!e);
        terminal.get_input().restore_blocking();
    }

    if (e) {
        auto pair = e->get_traceback().back();
        cerr << pair.first << " (" << pair.second << ")" << endl;
        cerr << "Press any key to continue." << endl;
        terminal.get_input().override_blocking(true);
        terminal.get_input().wait_any_key();
        terminal.get_input().restore_blocking();
    }

    return 0;
}

vector<aggregators::series*> app::search_series() {
    set_title("Search series");
    vector<aggregators::series*> search_results;
    rectangle centered_bounds = get_centered_bounds(-1, 7);
    string series_search = settings["series_search"];

    do {
        while (series_search == "") {
            window::framed search_window(centered_bounds);
            series_search = input_dialog::run(search_window, "Enter series to search:", "Search");
            boost::trim(series_search);
            if (series_search == "") {
                window::framed message_window(centered_bounds);
                message_dialog::run(message_window, "Please enter a series title.");
            }
        }

        {
            window::plain loading_window(get_centered_bounds(-1, -1, 3));
            stream _stream(loading_window, color::get_accent_color());
            _stream << stream::write_centered(string("Searching for series ") + series_search + " ...") << stream::refresh();
            search_results = aggregators::aggregator::search(series_search);
        }

        if (search_results.size() == 0) {
            window::framed message_window(centered_bounds);
            message_dialog::run(message_window, [&series_search](stream& _stream) {
                _stream << "Nothing was found for " << color::get_accent_color() <<
                        series_search << color::previous << ".";
            }, "Try again");
            series_search = "";
        }
    } while (search_results.size() == 0);

    return search_results;
}

aggregators::series& app::choose_series(vector<aggregators::series*>& search_results) {
    set_title("Choose series");
    if (search_results.size() == 1)
        current_series = search_results[0];
    else {
        window::framed results_window(get_centered_bounds());
        current_series = menu_dialog::run(results_window, "The following series were found:",
                                          search_results, *search_results.begin(), "Choose");
    }
    return *current_series;
}

void app::display_series(aggregators::series& series) {
    window::plain series_window(rectangle(status_window.get_full_bounds().width, 0,
                                          COLS - status_window.get_full_bounds().width, LINES));
    stream _stream(series_window, color::get_accent_color());
    _stream << stream::write_centered(string("Loading series ") + series.get_title() + " ...") << stream::refresh();
    series.load();

    _stream << stream::clear();
    set_title(series.get_title());
    menu::horizontal<aggregators::series> series_menu(series_window, series, *series.begin());
        
    if (settings.is_set("rename_files_directory"))
        aggregators::episode::file::rename_files(series, settings["rename_files_directory"], settings["rename_files_pattern"]);
    if (settings.get_download_selection().size() > 0)
        download_episodes(settings.get_download_selection());
    
    terminal.get_input().wait();
}

void app::download_episodes(aggregators::download_selection& download_selection) {
    if (!current_series)
        throw runtime_error("there is no current series");

    window::framed download_window(get_centered_bounds());
    util::download_dialog::run<aggregators::episode, aggregators::episode::download>(
        download_window, download_selection.get_episodes(*current_series));
    download_selection.clear();
}