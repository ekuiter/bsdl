#pragma once

#include "color.hpp"
#include "stream.hpp"
#include "stream_modifier.hpp"
#include "window.hpp"
#include "exception.hpp"
#include <iostream>
#include <sstream>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <curses.h>

using namespace std;

namespace curses {
    namespace menu {
        template <typename T>
        class base {
        protected:
            window& _window;
            stream _stream;
            T& items;
            int postfix_length, max_entries, top_item, selected_item, item_number;
            color highlight_color;

            typedef typename remove_reference<decltype(*items.begin())>::type object_type;

            int get_entries() {
                return min<int>(item_number - top_item, max_entries);
            }

            int get_bottom_item() {
                return top_item + get_entries() - 1;
            }

            void select_item(int item, bool should_refresh = true) {
                if (item < -1 || item >= item_number)
                    throw exception(string("item ") + to_string(item) + " does not exist");
                if (item == selected_item)
                    return;
                selected_item = item;
                if (selected_item == -1);
                else if (selected_item < top_item)
                    top_item = selected_item;
                else if (selected_item > get_bottom_item())
                    top_item = selected_item - max_entries + 1;
                if (should_refresh)
                    refresh();
            }

            object_type* object_from(int item) {
                int current_item = 0;
                for (auto& obj : items) {
                    if (current_item == item)
                        return &obj;
                    current_item++;
                }
                return nullptr;
            }

            int item_from(const object_type& obj) {
                int current_item = 0;
                for (auto& current_obj : items) {
                    if (&current_obj == &obj)
                        return current_item;
                    current_item++;
                }
                return -1;
            }

            void refresh_items(function<void (int, int, const object_type&)> fn) {
                int entry = 0, i = -1;
                for (auto& obj : items) {
                    if (++i < top_item || i > get_bottom_item())
                        continue;
                    fn(entry, i, obj);
                    entry++;
                }
            }

            void refresh_item(window& window, const string& prefix, const object_type& obj,
                              int max_length, function<void (int, int)> fn = nullptr) {
                point p_before, p_after;
                _stream << stream::get(p_before) << stream::write(" ", max_length) <<
                        stream::move(p_before) << prefix << obj << stream::get(p_after);

                if (p_after.y > p_before.y ||
                        (p_before.y == window.get_bounds().height - 1 && p_after.x > max_length - postfix_length)) {
                    if (!fn || p_after.x != 0)
                        _stream << stream::move(point(max_length - postfix_length, p_before.y)) << "...";
                    if (fn)
                        fn(p_before.y, p_after.y);
                } else
                    _stream << stream::advance(max_length - p_after.x);
            }

            void refresh_selected_item(function<void (int)> fn) {
                if (selected_item > -1)
                    fn(selected_item - top_item);
            }

            string toggle_highlight(window& window, bool sel = true) {
                static int i;
                if (sel) {
                    if (i % 2 == 0)
                        wattron(window.get(), highlight_color.get());
                    if (i % 2 == 1)
                        wattroff(window.get(), highlight_color.get());
                    i++;
                }
                return "";
            }

        public:
            base(window& window, T& _items, const typename base<T>::object_type* selected_object,
                 int default_selected_item, int _max_entries, const color& _highlight_color):
                    _window(window), _stream(window), items(_items), postfix_length(3), max_entries(_max_entries),
                    top_item(0), selected_item(-1), item_number(0), highlight_color(_highlight_color) {
                for (auto& obj : items)
                    item_number++;

                _stream.set_refresh(false);
                window.set_scrolling(false);

                if (selected_object) {
                    select_item(item_from(*selected_object), false);
                    if (selected_item == -1)
                        throw exception("selected object is not in the collection");
                } else
                    this->selected_item = default_selected_item;
            }

            void change_selection(int entries) {
                for (; entries != 0; entries < 0 ? entries++ : entries--)
                    if ((entries < 0 && selected_item - 1 >= 0) ||
                        (entries > 0 && selected_item + 1 < static_cast<int>(item_number)))
                        select_item(selected_item + (entries < 0 ? -1 : 1));
            }

            void select_entry(int entry) {
                if (entry >= 0 && entry < get_entries())
                    select_item(top_item + entry);
            }

            object_type* get_selected_object() {
                return object_from(selected_item);
            }

            virtual void refresh() = 0;
        };
    }
}
