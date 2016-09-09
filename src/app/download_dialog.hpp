#pragma once

#include "../curses/terminal.hpp"
#include "../http/download_queue.hpp"
#include <iostream>

using namespace std;

class download_dialog {
public:
    template <typename T1, typename T2>
    static void run(window& window, const vector<T1*>& downloadables,
                    const color& highlight_color = color::get_accent_color()) {

        http::download_queue<T2> download_queue;
        menu::vertical<http::download_queue<T2>>* menu_ptr = nullptr;

        auto refresh_callback = [&menu_ptr](T2& download) {
            menu_ptr->refresh_object(download);
        };

        for (auto& downloadable : downloadables)
            download_queue.push(downloadable->get_download(refresh_callback));

        string abort_all_action = "Abort all",
                abort_selected_action = "Abort selected";
        int abort_all_width = abort_all_action.length() + 4,
                abort_selected_width = abort_selected_action.length() + 4,
                button_height = 3;
        point abort_all_pos = window.get_dimensions() - point(abort_all_width, button_height),
                abort_selected_pos = abort_all_pos - point(abort_selected_width, 0);

        window::sub menu_wrapper(window, rectangle(0, 0, window.get_dimensions() - point(0, button_height)));
        window::sub abort_all_wrapper(window, rectangle(abort_all_pos, abort_all_width, button_height));
        window::sub abort_selected_wrapper(window, rectangle(abort_selected_pos, abort_selected_width, button_height));
        button abort_all(abort_all_wrapper, abort_all_action);
        button abort_selected(abort_selected_wrapper, abort_selected_action);
        menu::vertical<http::download_queue<T2>> menu(menu_wrapper, download_queue, nullptr, highlight_color);
        menu_ptr = &menu;

        abort_all_wrapper.set_mouse_callback(input::instance().mouse_event(BUTTON1_PRESSED, [&download_queue]() {
            for (auto& download : download_queue)
                download_queue.abort(download);
            return true;
        }));

        abort_selected_wrapper.set_mouse_callback(input::instance().mouse_event(BUTTON1_PRESSED, [&menu, &download_queue]() {
            T2* selected_download = menu.get_selected_object();
            if (selected_download)
                download_queue.abort(*selected_download);
            return true;
        }));

        input::instance().set_blocking(false);
        download_queue();
        input::instance().set_blocking(true);
    }
};
