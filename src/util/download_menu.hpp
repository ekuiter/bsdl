#pragma once

#include "../curses/terminal.hpp"
#include "../curses/widgets.hpp"
#include <iostream>

using namespace std;
using namespace curses;

namespace util {
    inline button_group::button_descriptor make_button_descriptor(const string& action, int ch, function<bool ()> fn) {
        return { action, input::instance().keyboard_event(ch, fn), input::instance().mouse_event(BUTTON1_PRESSED, fn) };
    }

    template<typename T>
    void make_download_menu(window& window, unique_ptr<window::sub>& menu_wrapper, unique_ptr<window::sub>& buttons_wrapper,
                            unique_ptr<menu::vertical<T>>& menu, unique_ptr<button_group>& buttons, T& pointers,
                            function<bool (const vector<aggregators::episode*>&)> fn,
                            vector<button_group::button_descriptor> button_descriptors = {}, int y_offset = 0) {
        int button_height = 3;
        rectangle _rectangle = window.top_left_rectangle(0, -button_height - y_offset);
        _rectangle.p.y += y_offset;
        menu_wrapper.reset(new window::sub(window, _rectangle));
        typename menu::vertical<T>::multi* menu_ptr = new typename menu::vertical<T>::multi(*menu_wrapper, pointers);
        menu.reset(menu_ptr);

        button_descriptors.push_back(make_button_descriptor("Mark", '\n', [menu_ptr]() {
                    auto selected_episode = menu_ptr->get_selected_pointer();
                    if (selected_episode)
                        menu_ptr->toggle_marked_pointer(selected_episode);
                    return true;
                }));
        button_descriptors.push_back(make_button_descriptor("Mark all", 'a', [menu_ptr]() {
                    menu_ptr->toggle_all();
                    return true;
                }));
        button_descriptors.push_back(make_button_descriptor("Start download", 's', [menu_ptr, fn]() {
                    return fn(menu_ptr->get_marked_pointers());
                }));
        
        buttons_wrapper.reset(new window::sub(window, window.bottom_right_rectangle(button_group::get_width(button_descriptors), button_height)));
        buttons.reset(new button_group(*buttons_wrapper, button_descriptors));
    }
}
