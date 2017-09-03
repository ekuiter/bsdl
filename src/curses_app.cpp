#include "curses_app.hpp"

curses_app::curses_app():
    terminal(terminal::instance()), settings(settings::instance()),
    status_window(rectangle(0, 0, COLS / 3, LINES)),
    title_window(status_window, rectangle(0, 0, status_window.get_bounds().width, 2)),
    title_height(0, title_window.get_bounds().height),
    log_window(status_window, rectangle(title_height, status_window.get_dimensions() - title_height)),
    spinner(log_window) {

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
    terminal.get_input().register_keyboard_callback(bind(&curses_app::keyboard_callback, this, _1));
    http::client::instance().set_callback(bind(&curses_app::http_callback, this, _1, _2, _3, _4, _5));

    set_title("bsdl");
    cout << "bsdl initialized (" << terminal.get_locale() << ")." << endl;
    clog << endl << "Settings:" << endl << settings << endl;
}

void curses_app::set_title(const string& _title, bool set_notice, string notice) {
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

bool curses_app::keyboard_callback(int ch) {
    if (ch == 27) // ESC
        exit(EXIT_SUCCESS);
    return true;
}

int curses_app::http_callback(http::request::status status, const http::request& request,
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
