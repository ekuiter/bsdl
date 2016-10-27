#pragma once

#include "menu_base.hpp"
#include <set>

using namespace std;

namespace curses {
    namespace menu {
        template <typename T>
        class vertical : public base<T> {
            int max_length;

            virtual string prefix_for_item(int item, bool selected = false) {
                return selected ? "* " : "- ";
            }

            void _refresh_item(int entry, int i, const typename base<T>::pointer_type ptr) {
                this->refresh_item(this->_window, prefix_for_item(i), ptr, max_length,
                                   [this, &i](int y_before, int y_after) {
                    this->_stream << stream::move(point(0, y_before + 1));
                    if (i == this->get_bottom_item())
                        this->_stream << stream::write(" ", (y_after - y_before) * max_length);
                });
            }

            void _refresh_selected_item() {
                this->refresh_selected_item([this](int selected_entry) {
                    this->_stream << stream::move(point(0, selected_entry)) <<
                            stream::write(prefix_for_item(this->selected_item, true), 1, false) <<
                            stream::write_attribute(A_REVERSE, this->highlight_color);
                });
            }

        public:
            vertical(window& window, const T& pointers, const typename base<T>::pointer_type selected_ptr = nullptr,
                     const color& highlight_color = color::get_accent_color()):
                    base<T>(window, pointers, selected_ptr, -1, window.get_bounds().height, highlight_color),
                    max_length(window.get_bounds().width) {
                window.set_keyboard_callback([this](int ch) {
                    if (ch == KEY_UP)
                        this->change_selection(-1);
                    if (ch == KEY_DOWN)
                        this->change_selection(1);
                    return true;
                });

                window.set_mouse_callback([this](MEVENT e) {
                    if (e.bstate & BUTTON1_PRESSED)
                        this->select_entry(e.y);
                    if (e.bstate & BUTTON2_PRESSED)
                        this->change_selection(-2);
                    if (e.bstate & BUTTON4_PRESSED)
                        this->change_selection(2);
                    return true;
                });

                refresh();
            }

            void refresh() override {
                this->_stream << stream::move(point(0, 0));

                using namespace placeholders;
                this->refresh_items(bind(&vertical::_refresh_item, this, _1, _2, _3));

                _refresh_selected_item();

                this->_stream << stream::refresh();
            }

            void refresh_pointer(const typename base<T>::pointer_type ptr) {
                int item = this->item_from(ptr);
                if (item == -1)
                    throw exception("object is not in the collection");
                if (item < this->top_item || item > this->get_bottom_item())
                    return;
                int entry = item - this->top_item;

                this->_stream << stream::move(point(0, entry));
                _refresh_item(entry, item, ptr);
                if (this->selected_item == item)
                    _refresh_selected_item();
                this->_stream << stream::refresh();
            }

            class multi;
        };

	template <typename T>
        class vertical<T>::multi : public vertical<T> {
                set<int> marked_items;

                string prefix_for_item(int item, bool selected = false) {
                    return marked_items.find(item) == marked_items.end() ? "[ ] " : "[*] ";
                }

        public:
                multi(window& window, const T& pointers, const typename base<T>::pointer_type selected_ptr = nullptr,
                    const color& highlight_color = color::get_accent_color()) :
                    vertical<T>(window, pointers, selected_ptr, highlight_color) {
                    refresh();
                }

                void toggle_marked_pointer(const typename base<T>::pointer_type ptr) {
                    int item = this->item_from(ptr);
                    if (item == -1)
                        throw exception("marked object is not in the collection");
                    if (marked_items.find(item) == marked_items.end())
                        marked_items.insert(item);
                    else
                        marked_items.erase(item);
                    refresh();
                }

                void toggle_all() {
                    for (auto ptr : this->pointers)
                            toggle_marked_object(ptr);
                }

                vector<typename base<T>::pointer_type> get_marked_pointers() {
                    vector<typename base<T>::pointer_type> pointers;
                    for (auto item : marked_items)
                            pointers.push_back(this->pointer_from(item));
                    return pointers;
                }
        };
    }
}
