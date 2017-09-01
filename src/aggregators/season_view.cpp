#include "season_view.hpp"
#include "season.hpp"
#include "download_selection.hpp"
#include "../app.hpp"
#include "../util/bsdl_uri.hpp"
#include <sstream>

using namespace curses;

namespace aggregators {
    season_view::season_view(season& _season, window& window) {
        stringstream s;
        s << "Loading " << _season << " ...";
        stream _stream(window, color::get_accent_color());
        _stream << stream::write_centered(s.str()) << stream::refresh();
        _season.load();
        _stream << stream::clear();

        string start_download_action = "Start download",
                download_all_action = "Mark all",
            download_selected_action = "Mark selected",
            copy_uri_action = "Copy URI";
        int start_download_width = start_download_action.length() + 4,
                download_all_width = download_all_action.length() + 4,
                download_selected_width = download_selected_action.length() + 4,
                copy_uri_width = copy_uri_action.length() + 4,
                button_height = 3;
        point start_download_pos = window.get_dimensions() - point(start_download_width + 1, button_height),
                download_all_pos = start_download_pos - point(download_all_width, 0),
                download_selected_pos = download_all_pos - point(download_selected_width, 0),
                copy_uri_pos = point(0, window.get_dimensions().y - button_height);

        menu_wrapper.reset(new window::sub(window, rectangle(1, 0, window.get_dimensions() - point(2, button_height))));
        start_download_wrapper.reset(new window::sub(window,
                                                     rectangle(start_download_pos, start_download_width, button_height)));
        download_all_wrapper.reset(new window::sub(window,
                                                   rectangle(download_all_pos, download_all_width, button_height)));
        download_selected_wrapper.reset(new window::sub(window,
                                                        rectangle(download_selected_pos, download_selected_width, button_height)));
        copy_uri_wrapper.reset(new window::sub(window,
                                               rectangle(copy_uri_pos, copy_uri_width, button_height)));
        menu::vertical<season>::multi* menu_ptr = new menu::vertical<season>::multi(*menu_wrapper, _season);
        menu.reset(menu_ptr);
        start_download.reset(new curses::button(*start_download_wrapper, start_download_action));
        download_all.reset(new curses::button(*download_all_wrapper, download_all_action));
        download_selected.reset(new curses::button(*download_selected_wrapper, download_selected_action));
        copy_uri.reset(new curses::button(*copy_uri_wrapper, copy_uri_action));

        auto start_download_callback = [menu_ptr]() {
            download_selection _download_selection;
            _download_selection.add(menu_ptr->get_marked_pointers());
            app::instance().download_episodes(_download_selection);
            return true;
        };

        start_download_wrapper->set_keyboard_callback(input::instance().keyboard_event('s', start_download_callback));
        start_download_wrapper->set_mouse_callback(input::instance().mouse_event(BUTTON1_PRESSED, start_download_callback));

        auto download_all_callback = [menu_ptr]() {
            menu_ptr->toggle_all();
            return true;
        };

        download_all_wrapper->set_keyboard_callback(input::instance().keyboard_event('a', download_all_callback));
        download_all_wrapper->set_mouse_callback(input::instance().mouse_event(BUTTON1_PRESSED, download_all_callback));

        auto download_selected_callback = [menu_ptr]() {
            auto selected_episode = menu_ptr->get_selected_pointer();
            if (selected_episode)
                menu_ptr->toggle_marked_pointer(selected_episode);
            return true;
        };

        download_selected_wrapper->set_keyboard_callback(input::instance().keyboard_event('\n', download_selected_callback));
        download_selected_wrapper->set_mouse_callback(input::instance().mouse_event(BUTTON1_PRESSED, download_selected_callback));

        auto copy_uri_callback = [menu_ptr]() {
            util::platform::copy(util::bsdl_uri(*app::instance().get_current_series()).get_uri());
            return true;
        };

        copy_uri_wrapper->set_keyboard_callback(input::instance().keyboard_event('u', copy_uri_callback));
        copy_uri_wrapper->set_mouse_callback(input::instance().mouse_event(BUTTON1_PRESSED, copy_uri_callback));
    }
}
