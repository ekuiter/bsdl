#pragma once

#include "menu_base.hpp"
#include "window_plain.hpp"
#include "window_sub.hpp"
#include "platform.hpp"
#include <vector>
#include <unordered_map>
#include <memory>

using namespace std;

namespace curses {
    namespace menu {
        template <typename T>
        class horizontal : public base<T> {
            vector<unique_ptr<window::sub>> tabs;
            unordered_map<int, unique_ptr<window::plain>> tab_content_windows;
            int tab_width, tab_height;
            window::sub content_window;

            int get_tabbing_width() {
                return this->get_entries() * tab_width;
            }

            bool should_draw_closing_line() {
                return get_tabbing_width() < this->_window.get_bounds().width;
            }

            void refresh_content_window() {
                wborder(content_window.get(), 0, 0, ' ', 0, ACS_VLINE, ACS_VLINE, 0, 0);
                this->_stream << content_window << stream::refresh();

                if (should_draw_closing_line()) {
                    this->_stream << this->_window << stream::move(point(get_tabbing_width(), tab_height - 1)) <<
                            stream::write(stream::ext_char(ACS_HLINE),
                                          this->_window.get_bounds().width - get_tabbing_width() - 1) <<
                            stream::ext_char(ACS_URCORNER) << stream::refresh();
                }
            }

            void refresh_tab_content_window(int item) {                
                for (auto& pair : tab_content_windows)
                    pair.second->hide();

                if (tab_content_windows[item])
                    tab_content_windows[item]->show();
                else {
                    point p;
                    this->_stream << content_window << stream::get(p, stream::get::coord::BEG);
                    unique_ptr<window::plain> tab_content_window(
                            new window::plain(rectangle(p + point(2, 0), content_window.get_dimensions() - point(4, 1)))
                    );
                    tab_content_window->override_layer(this->_window.get_paneled_window().get_layer());
                    this->pointer_from(item)->create_view(*tab_content_window);
                    this->_stream << *tab_content_window << stream::refresh();
                    tab_content_windows[item] = move(tab_content_window);
                }
            }

            void tab_border(window& tab, bool sel, bool neigh,
                            chtype ls, chtype rs, chtype ts, chtype bs, chtype tl, chtype tr, chtype bl, chtype br) {
                point max = tab.get_dimensions() - point(1, 1);
                ls = ls ? ls : ACS_VLINE,    rs = rs ? rs : ACS_VLINE,    ts = ts ? ts : ACS_HLINE,    bs = bs ? bs : ACS_HLINE;
                tl = tl ? tl : ACS_ULCORNER, tr = tr ? tr : ACS_URCORNER, bl = bl ? bl : ACS_LLCORNER, br = br ? br : ACS_LRCORNER;

                auto vertical_line = [this, &max](int x, chtype c) {
                    for (int i = 1; i < max.y; i++)
                        this->_stream << stream::move(point(x, i)) << stream::write(stream::ext_char(c));
                    return "";
                };

                this->_stream << tab;
                this->toggle_highlight(tab, sel);
                this->_stream <<
                        stream::move(point(1, 0)) << stream::write(stream::ext_char(ts), max.x - 1) <<
                        stream::move(point(1, max.y)) << stream::write(stream::ext_char(bs), max.x - 1) <<
                        stream::move(point(max.x, 0)) << stream::ext_char(tr) <<
                        stream::move(point(max.x, max.y)) << stream::ext_char(br) << vertical_line(max.x, rs);
                this->toggle_highlight(tab, neigh);
                this->_stream << vertical_line(0, ls) <<
                        stream::move(point(0, 0)) << stream::ext_char(tl) <<
                        stream::move(point(0, max.y)) << stream::ext_char(bl);
                this->toggle_highlight(tab, neigh);
                this->toggle_highlight(tab, sel);
            }

            void refresh_border(window& tab, int entry, bool sel = false, bool neigh = false) {
                chtype closing_corner = should_draw_closing_line() ? (sel ? ACS_LLCORNER : ACS_BTEE) : (sel ? ACS_VLINE : ACS_RTEE);
                vector<vector<chtype>> borders = {
                //       rs,  tl,       tr,        bl,                                                     br
                /* 1 */ {0,   0,        0,         sel ? ACS_VLINE : ACS_LTEE,                             sel ? ACS_LLCORNER : ACS_BTEE},
                /* l */ {' ', 0,        ACS_HLINE, sel ? ACS_VLINE : ACS_LTEE,                             sel ? ' ' : ACS_HLINE},
                /* m */ {' ', ACS_TTEE, ACS_HLINE, sel ? ACS_LRCORNER : (neigh ? ACS_LLCORNER : ACS_BTEE), sel ? ' ' : ACS_HLINE},
                /* r */ {0,   ACS_TTEE, 0,         sel ? ACS_LRCORNER : (neigh ? ACS_LLCORNER : ACS_BTEE), closing_corner}
                };
                int set = this->get_entries() == 1        ? 0 :
                          entry == 0                      ? 1 :
                          entry < this->get_entries() - 1 ? 2 : 3;
                tab_border(tab, sel, neigh, 0,               borders[set][0], 0,               sel ? ' ' : 0,
                                            borders[set][1], borders[set][2], borders[set][3], borders[set][4]);
            }

        public:
            horizontal(window& window, T& pointers, const typename base<T>::pointer_type selected_ptr = nullptr,
                       int _tab_width = 20, const color& highlight_color = color::get_accent_color()):
                    base<T>(window, pointers, selected_ptr, 0, window.get_bounds().width / _tab_width, highlight_color),
                    tab_width(_tab_width), tab_height(3),
                    content_window(window, rectangle(0, tab_height, window.get_dimensions() - point(0, tab_height))) {
                if (this->item_number == 0)
                    throw exception("horizontal menu has no tabs");

                for (int i = 0; i < this->max_entries; i++)
                    tabs.push_back(unique_ptr<window::sub>(
                            new window::sub(this->_window, rectangle(i * tab_width, 0, tab_width, tab_height))
                    ));

                window.set_keyboard_callback([this](int ch) {
                    if (ch == KEY_LEFT)
                        this->change_selection(-1);
                    if (ch == KEY_RIGHT)
                        this->change_selection(1);
                    return true;
                });

                int entry = 0;
                for (auto& tab : tabs) {
                    tab->set_mouse_callback([this, entry](MEVENT e) {
                        if (entry >= this->item_number)
                            return true;
                        if (e.bstate & BUTTON1_PRESSED && e.x != 0)
                            this->select_entry(entry);
                        if (e.bstate & BUTTON2_PRESSED)
                            this->change_selection(-1);
                        if (e.bstate & BUTTON4_PRESSED)
                            this->change_selection(1);
                        return true;
                    });
                    entry++;
                }

                refresh();
            }

            ~horizontal() {
                for (auto ptr : this->pointers)
                    ptr->destroy_view();
            }

            void refresh() override {
                this->refresh_items([this](int entry, int i, const typename base<T>::pointer_type ptr) {
                    window& tab = *tabs[entry];
                    this->_stream << tab << stream::move(point(2, 1));
                    this->refresh_item(tab, "", ptr, tab_width - 1);
                    refresh_border(tab, entry);
                    this->_stream << stream::refresh();
                });

                this->refresh_selected_item([this](int selected_entry) {
                    window& tab = *tabs[selected_entry];
                    this->_stream << tab << stream::move(point(1, 1)) <<
                    stream::write_attribute(A_BOLD, this->highlight_color, tab_width - 2);
                    refresh_border(tab, selected_entry, true);
                    this->_stream << stream::refresh();
                    int neighbour_entry = selected_entry + 1;
                    if (neighbour_entry < this->get_entries()) {
                        window& neighbour_tab = *tabs[neighbour_entry];
                        refresh_border(neighbour_tab, neighbour_entry, false, true);
                        this->_stream << neighbour_tab << stream::refresh();
                    }
                });

                refresh_content_window();
                refresh_tab_content_window(this->selected_item);
            }
        };
    }
}
