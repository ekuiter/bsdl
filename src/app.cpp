#include "app.hpp"
#include "util/platform.hpp"
#include "aggregators/episode_download.hpp"
#include "curses/platform.hpp"
#include "util/download_dialog.hpp"
#include "util/addressed.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <fstream>

unique_ptr<app> app::_instance = nullptr;

app& app::instance() {
    if (!_instance)
        _instance.reset(new main_app());
    return *_instance;
}

main_app::main_app():
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
    terminal.get_input().register_keyboard_callback(bind(&main_app::keyboard_callback, this, _1));
    http::client::instance().set_callback(bind(&main_app::http_callback, this, _1, _2, _3, _4, _5)).
        set_timeout(stoi(settings::get("timeout")));

    set_title("bsdl");
    cout << "bsdl initialized (" << terminal.get_locale() << ")." << endl;
    clog << endl << "Settings:" << endl << settings << endl;
}

void main_app::initialize() {
    if (settings.is_set("action")) {
        window::framed message_window(get_centered_bounds());
        if (settings["action"] == "help")
            help_message();
        else if (settings["action"] == "version")
            version_message();
        else
            run_tests();
        exit(EXIT_SUCCESS);
    }

    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
    aggregators::subtitle::set_preferred_subtitles(settings.get_preferred_subtitles());
}

void main_app::help_message() {
    window::framed message_window(get_centered_bounds());
    message_dialog::run(message_window,
        "usage: bsdl [series] [--download [season [episode]]] [--aggregator aggregator] "
                "[--provider provider] [--subtitle subtitle] [--output-files dir] "
                "[--rename-files [dir [pattern]]] [--log-file [file]] [--config-file file] "
                "[--version] [--help] [--test [test]]", "Okay");
}

void main_app::version_message() {
    window::framed message_window(get_centered_bounds());
    message_dialog::run(message_window, [&message_window](stream& _stream) {
        _stream.set_wrap(false);
        _stream << "bsdl 1.9" << endl <<
                stream::write(stream::ext_char(ACS_HLINE), message_window.get_bounds().width) <<
                "Source code: https://github.com/ekuiter/bsdl" << endl;
        _stream.set_wrap(true);
        _stream << "Made with " << color(COLOR_RED) << "<3" << color::previous <<
                " (and Boost, curses, curl, curlcpp, gumbo-query, gumbo-parser, nlohmann/json)." << endl <<
                                        "Compiled for " << color::get_accent_color() << util::platform::get_name() <<
                                        color::previous << " on " << color::get_accent_color() <<
                                        __DATE__ << " " << __TIME__ << color::previous << ".";
    }, "Okay");
}

void main_app::run_tests() {
    terminal.~terminal();
    char buffer[128];
    string args = settings["action"] == "test" ? "" :
        string(" --run_test=") + (settings["action"] == "quick" ? "!@long_running" : settings["action"]);
    if (settings["action"] == "list")
        args = " --list_content";
    FILE* pipe = popen((settings::instance().resource_file({ "bsdl" }, "bsdltest") + args).c_str(), "r");
    if (!pipe) {
        fprintf(stderr, "popen failed");
        exit(EXIT_FAILURE);
    }
    try {
        while (!feof(pipe))
            if (fgets(buffer, 128, pipe) != NULL)
                fprintf(stderr, "%s", buffer);
    } catch (...) {
        fprintf(stderr, "reading pipe failed");
        exit(EXIT_FAILURE);
    }
    exit(pclose(pipe) / 256);
}

void main_app::set_title(const string& _title, bool set_notice, string notice) {
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

bool main_app::keyboard_callback(int ch) {
    if (ch == 27) // ESC
        exit(EXIT_SUCCESS);
    return true;
}

int main_app::http_callback(http::request::status status, const http::request& request,
                  curl_off_t now_bytes, curl_off_t total_bytes, curl::curl_easy_exception* e) {
    static int i;
    bool old_visible = terminal.get_stream(cout).set_visible(true);

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
    
    terminal.get_stream(cout).set_visible(old_visible);
    return 0;
}

string main_app::run_start_window(const rectangle& bounds) {
    window::framed start_window(bounds);
    stream _stream(start_window);
    _stream << color::get_accent_color() << "Welcome to bsdl!";
    window::sub search_dialog(start_window, rectangle(0, 1, bounds.width - 4, 5));
    int x = 0;
    vector<window::sub*> button_wrappers;
    vector<button*> buttons;
    text_box* text_box;
    
    auto make_button = [&start_window, &_stream, &x, &text_box, &button_wrappers, &buttons]
        (const string& action, function<void ()> fn) {
        int row = 3, button_width = action.length() + (COLS >= 88 ? 4 : 2);
        window::sub* button_wrapper = new window::sub(start_window, rectangle(x, row, button_width, 3));
        button_wrappers.push_back(button_wrapper);
        buttons.push_back(new button(*button_wrapper, action));
        if (x > 0)
            _stream << color(COLOR_WHITE) <<
                    stream::move(point(x, row))     << stream::ext_char(ACS_TTEE) <<
                    stream::move(point(x, row + 2)) << stream::ext_char(ACS_BTEE) << stream::refresh();
        x += button_width - 1;
		button_wrapper->set_mouse_callback(input::instance().mouse_event(BUTTON1_PRESSED, [fn, &text_box]() {
			platform::curs_set(0);
			fn();
			text_box->refresh();
			platform::curs_set(1);
			return true;
		}));
    };

    make_button(COLS >= 112 ? "Output directory" : "Output dir", [this, &bounds]() {
        do {
            {
                window::framed input_window(bounds);
                settings["output_files_directory"] = input_dialog::run(input_window,
                    string(COLS >= 82 ? "\n" : "") + "Enter output directory for episodes:",
                    "Save", color::get_accent_color(), settings["output_files_directory"]);
            }
            boost::trim(settings["output_files_directory"]);
            if (!settings.is_set("output_files_directory"))
                cerr << "Please enter an output directory." << endl;
        } while (!settings.is_set("output_files_directory"));
        cout << "Episodes will be saved to " << color::get_accent_color() <<
                settings["output_files_directory"] << color::previous << "." << endl;
    });
    
    make_button(COLS >= 100 ? "Rename files" : "Rename", [this, &bounds]() {
        {
            window::framed input_window(bounds);
            settings["rename_files_directory"] = input_dialog::run(input_window,
                    string(COLS >= 89 ? "\n" : "") + "Enter directory with episodes to rename:",
                    "Continue", color::get_accent_color(), settings["rename_files_directory"]);
            boost::trim(settings["rename_files_directory"]);
        }
        {
            window::framed input_window(bounds);
            settings["rename_files_pattern"] = input_dialog::run(input_window, "\nEnter renaming pattern:",
                    "Save", color::get_accent_color(), settings["rename_files_pattern"]);
        }
        if (!settings.is_set("rename_files_directory"))
            cout << "No episodes will be renamed." << endl;
        else
            cout << "Episodes in " << color::get_accent_color() << settings["rename_files_directory"] << color::previous <<
                    " will be renamed using the pattern " << color::get_accent_color() <<
                    (settings["rename_files_pattern"] == "" ? "default" : settings["rename_files_pattern"]) <<
                    color::previous << "." << endl;
    });
    
    make_button("Info", bind(&main_app::version_message, this));
    
    string series_search = input_dialog::run(search_dialog, "Enter series to search:", "Search",
            color::get_accent_color(), "", &text_box);
    
    for (auto button : buttons)
        delete button;
    for (auto button_wrapper : button_wrappers)
        delete button_wrapper;
    return series_search;
}

vector<aggregators::series*> main_app::search_series() {
    set_title("Search series");
    rectangle centered_bounds = get_centered_bounds(-1, 8);
    series_search = settings["series_search"];

    do {
        while (series_search == "") {
            series_search = run_start_window(centered_bounds);
            boost::trim(series_search);
            if (series_search == "") {
                window::framed message_window(centered_bounds);
                message_dialog::run(message_window, "Please enter a series title.");
            }
        }

        {
            window::plain loading_window(get_centered_bounds(-1, -1));
            stream _stream(loading_window, color::get_accent_color());
            _stream << stream::write_centered(string("Searching for series ") + series_search + " ...") << stream::refresh();
            search_results = aggregators::aggregator::search(series_search);
        }

        if (search_results.size() == 0) {
            window::framed message_window(centered_bounds);
            message_dialog::run(message_window, [this](stream& _stream) {
                _stream << "Nothing was found for " << color::get_accent_color() <<
                        series_search << color::previous << ".";
            }, "Try again");
            series_search = "";
        }
    } while (search_results.size() == 0);

    return search_results;
}

aggregators::series& main_app::choose_series(vector<aggregators::series*>& search_results, const string& prompt, const string& action) {
    set_title("Choose series");
    if (search_results.size() == 1)
        return *search_results[0];
    else {
        window::framed results_window(get_centered_bounds());
        return *menu_dialog::run(results_window, prompt, search_results, *search_results.begin(), action);
    }
}

void main_app::display_series(aggregators::series& series) {
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

void main_app::download_episodes(aggregators::download_selection& download_selection) {
    if (!current_series)
        throw runtime_error("there is no current series");

    window::framed download_window(get_centered_bounds());
    util::download_dialog::run<aggregators::episode, aggregators::episode::download>(
        download_window, download_selection.get_episodes(*current_series));
    download_selection.clear();
}
