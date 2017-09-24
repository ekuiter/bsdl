#include "main_app.hpp"
#include "../option.hpp"
#include "../util/bsdl_uri.hpp"
#include "../util/platform.hpp"
#include "../aggregators/episode_download.hpp"
#include "../curses/platform.hpp"
#include "../util/download_dialog.hpp"
#include "../util/addressed.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <fstream>

void main_app::initialize() {
    if (settings.is_set("action")) {
        if (settings["action"] == "version")
            version_message();
        else
            throw runtime_error(settings["action"] + " is not available in app mode");
        exit(EXIT_SUCCESS);
    }

    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
    aggregators::subtitle::set_preferred_subtitles(settings.get_preferred_subtitles());
}

void main_app::version_message() {
    window::framed message_window(get_centered_bounds());
    message_dialog::run(message_window, [&message_window](stream& _stream) {
        _stream.set_wrap(false);
        _stream << "bsdl 1.12" << endl <<
                stream::write(stream::ext_char(ACS_HLINE), message_window.get_bounds().width) <<
                "Source code: https://github.com/ekuiter/bsdl" << endl;
        _stream.set_wrap(true);
        _stream << "Made with " << stream::colored("<3", color(COLOR_RED)) <<
                " (and Boost, curses, curl, curlcpp, gumbo-query, gumbo-parser, nlohmann/json, youtube-dl, PhantomJS)." << endl <<
                "Compiled for " << stream::colored(util::platform::get_name()) <<
                " on " << stream::colored(string(__DATE__) + " " + __TIME__) << ".";
    }, "Okay");
}

string main_app::run_start_window(const rectangle& bounds) {
    window::framed start_window(bounds);
    stream _stream(start_window);
    _stream << color::get_accent_color() << "Welcome to bsdl!";
    window::sub search_dialog(start_window, rectangle(0, 1, bounds.width - 4, 5));
    text_box* text_box;

    auto make_mouse_callback = [&text_box](function<void ()> fn) {
        return input::instance().mouse_event(BUTTON1_PRESSED, [&text_box, &fn]() {
                platform::curs_set(0);
                fn();
                text_box->refresh();
                platform::curs_set(1);
                return true;
            });
    };
    
    button_group::button_descriptor options_button = {
        "Options", nullptr, make_mouse_callback([this]() {
                auto modifiable_options = option::get_modifiable_options();
                option break_option("break", {}, nullptr, nullptr, nullptr, "(Back to main menu)");
                modifiable_options.insert(modifiable_options.begin(), &break_option);
                option* last_option = &break_option;
                while (1) {
                    window::framed options_window(get_centered_bounds());
                    option* option = menu_dialog::run(options_window, "Choose an option to modify:", modifiable_options, last_option, "Choose");
                    if (option == &break_option)
                        break;
                    if (option)
                        option->modify();
                    last_option = option;
                }
            })
    }, info_button = {
        "Info", nullptr, make_mouse_callback(bind(&main_app::version_message, this))
    };
    
    window::sub buttons_window(start_window, start_window.bottom_left_rectangle(0, 3));
    button_group buttons(buttons_window, { options_button, info_button });
    
    return input_dialog::run(search_dialog, "Enter series to search:", "Search", color::get_accent_color(), "", &text_box);
}

vector<aggregators::series*> main_app::search_series() {
    set_title("Search series");
    rectangle centered_bounds = get_centered_bounds(-1, 8);
    series_search = settings["series_search"];

    auto show_message = [&centered_bounds](const string& msg){
        window::framed message_window(centered_bounds);
        message_dialog::run(message_window, msg);
    };

    do {
        util::search_query query(series_search);
        
        while (query.is_empty()) {
            series_search = run_start_window(centered_bounds);
            try {
                query = util::search_query(series_search);
                if (query.is_empty())
                    show_message("Please enter a series title.");
            } catch (util::uri_error e) {
                show_message(e.what());
            }
        }

        {
            window::plain loading_window(get_centered_bounds(-1, -1));
            stream _stream(loading_window, color::get_accent_color());
            series_search = query.get_search_string();
            _stream << stream::write_centered(string("Searching for series ") + series_search + " ...") << stream::refresh();
            search_results = query.fetch_results();
        }

        if (search_results.size() == 0) {
            window::framed message_window(centered_bounds);
            message_dialog::run(message_window, [this](stream& _stream) {
                    _stream << "Nothing was found for " << stream::colored(series_search) << ".";
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
    set_series_title(series);
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
