#pragma once

#include "../curses/terminal.hpp"
#include "../http/download_queue.hpp"
#include <iostream>

using namespace std;

namespace util {
    class download_dialog {
    public:
        template <typename T1, typename T2>
        static void run(window& window, const vector<T1*>& downloadables,
                        const color& highlight_color = color::get_accent_color()) {

            http::download_queue<T2> download_queue(stoi(settings::get("parallel_transfers")));
            menu::vertical<typename http::download_queue<T2>::addressed_type>* menu_ptr = nullptr;

            auto refresh_callback = [&menu_ptr](T2& download) {
                menu_ptr->refresh_pointer(&download);
            };

            for (auto& downloadable : downloadables)
                download_queue.push(*downloadable->get_download(refresh_callback));

            int button_height = 3;
            auto download_queue_addressed = download_queue.addressed();
            
            window::sub menu_wrapper(window, window.top_left_rectangle(0, -button_height));
            menu::vertical<typename http::download_queue<T2>::addressed_type>
                menu(menu_wrapper, download_queue_addressed, nullptr, highlight_color);
            menu_ptr = &menu;

            auto make_button_descriptor = [](const string& action, function<bool ()> fn) {
                return button_group::button_descriptor(action, nullptr, input::instance().mouse_event(BUTTON1_PRESSED, fn));
            };

            auto button_descriptors = {
                make_button_descriptor("Abort all", [&download_queue]() {
                        for (auto& download : download_queue)
                            download_queue.abort(download);
                        return true;
                    }),
                make_button_descriptor("Abort selected", [&menu, &download_queue]() {
                        T2* selected_download = menu.get_selected_pointer();
                        if (selected_download)
                            download_queue.abort(*selected_download);
                        return true;
                    })
            };
            
            window::sub buttons_wrapper(window, window.bottom_right_rectangle(button_group::get_width(button_descriptors), button_height));
            button_group buttons(buttons_wrapper, button_descriptors);

            input::instance().set_blocking(false);
            download_queue();
            input::instance().set_blocking(true);
        }
    };
}
