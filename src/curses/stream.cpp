#include "stream.hpp"
#include "stream_modifier.hpp"
#include <fstream>

namespace curses {
    void stream::cursesbuf::print_wrap(chtype c) {
        word = static_cast<char>(c) == ' ' || static_cast<char>(c) == '\n' ? "" : word + static_cast<char>(c);

        point p_before, p_after;
        get((point&) p_before)(_window);
        print(c);
        get((point&) p_after)(_window);

        if (word != "" && p_after.x == 0) {
            if (p_after.y == p_before.y)
                p_before.y--;
            for (auto c : word)
                waddch(window::dereference(_window), c | attribute);
            move(p_before - point(word.length() - 1, 0))(_window);
            for (int i = 0; i < word.length(); i++)
                waddch(window::dereference(_window), ' ');
            move(p_after + point(word.length(), 0))(_window);
        }
    }

    void stream::cursesbuf::print(chtype c) {
        if (static_cast<char>(c) == '\b')
            back()(_window, nullptr);
        else
            waddch(window::dereference(_window), c | attribute);
    }

    int stream::cursesbuf::overflow(int c) {
        if (log_file) {
            *log_file << static_cast<char>(c);
            if (static_cast<char>(c) == '\n')
                log_file->flush();
        }

        if (visible) {
            if (wrap)
                print_wrap(c);
            else
                print(c);
            if (refresh)
                stream::refresh()(_window);
        }

        return c;
    }
}
