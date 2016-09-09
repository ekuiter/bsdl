#pragma once

#include "menu.hpp"
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

            void _refresh_item(int entry, int i, const typename base<T>::object_type& obj) {
                this->refresh_item(this->_window, prefix_for_item(i), obj, max_length,
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
            vertical(window& window, T& items, const typename base<T>::object_type* selected_object = nullptr,
                     const color& highlight_color = color::get_accent_color()):
                    base<T>(window, items, selected_object, -1, window.get_bounds().height, highlight_color),
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

            void refresh_object(const typename base<T>::object_type& obj) {
                int item = this->item_from(obj);
                if (item == -1)
                    throw exception("object is not in the collection");
                if (item < this->top_item || item > this->get_bottom_item())
                    return;
                int entry = item - this->top_item;

                this->_stream << stream::move(point(0, entry));
                _refresh_item(entry, item, obj);
                if (this->selected_item == item)
                    _refresh_selected_item();
                this->_stream << stream::refresh();
            }

            class multi : public vertical<T> {
                set<int> marked_items;

                string prefix_for_item(int item, bool selected = false) {
                    return marked_items.find(item) == marked_items.end() ? "[ ] " : "[*] ";
                }

            public:
                multi(window& window, T& items, const typename base<T>::object_type* selected_object = nullptr,
                      const color& highlight_color = color::get_accent_color()):
                        vertical<T>(window, items, selected_object, highlight_color) {
                    refresh();
                }

                void toggle_marked_object(const typename base<T>::object_type& obj) {
                    int item = this->item_from(obj);
                    if (item == -1)
                        throw exception("marked object is not in the collection");
                    if (marked_items.find(item) == marked_items.end())
                        marked_items.insert(item);
                    else
                        marked_items.erase(item);
                    refresh();
                }

                void toggle_all() {
                    for (auto& obj : this->items)
                        toggle_marked_object(obj);
                }

                vector<typename base<T>::object_type*> get_marked_objects() {
                    vector<typename base<T>::object_type*> objects;
                    for (auto item : marked_items)
                        objects.push_back(this->object_from(item));
                    return objects;
                }
            };
        };
    }
}
