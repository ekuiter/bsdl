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

        int button_height = 3;
        menu_wrapper.reset(new window::sub(window, window.top_left_rectangle(0, -button_height)));
        menu::vertical<season>::multi* menu_ptr = new menu::vertical<season>::multi(*menu_wrapper, _season);
        menu.reset(menu_ptr);

        auto make_button_descriptor = [](const string& action, int ch, function<bool ()> fn) {
            return button_group::button_descriptor(action, input::instance().keyboard_event(ch, fn),
                                                   input::instance().mouse_event(BUTTON1_PRESSED, fn));
        };

        auto button_descriptors = {
            make_button_descriptor("Copy URI", 'u', []() {
                    util::platform::copy(util::bsdl_uri(*app::instance().get_current_series()).get_uri());
                    return true;
                }),
            make_button_descriptor("Mark", '\n', [menu_ptr]() {
                    auto selected_episode = menu_ptr->get_selected_pointer();
                    if (selected_episode)
                        menu_ptr->toggle_marked_pointer(selected_episode);
                    return true;
                }),
            make_button_descriptor("Mark all", 'a', [menu_ptr]() {
                    menu_ptr->toggle_all();
                    return true;
                }),
            make_button_descriptor("Start download", 's', [menu_ptr]() {
                    download_selection _download_selection;
                    _download_selection.add(menu_ptr->get_marked_pointers());
                    app::instance().download_episodes(_download_selection);
                    return true;
                })
        };
        
        buttons_wrapper.reset(new window::sub(window, window.bottom_right_rectangle(button_group::get_width(button_descriptors), button_height)));
        buttons.reset(new button_group(*buttons_wrapper, button_descriptors));
    }
}
