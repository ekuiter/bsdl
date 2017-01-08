#include "episode.hpp"
#include "../episode_download.hpp"
#include "episode_file.hpp"
#include "../../curses/terminal.hpp"

namespace aggregators {
    namespace bs {
        void episode::load(const http::response& response) const {
            loaded = true;
        }
        
        string episode::get_id() const {
            return file(*this).get_id();
        }
        
        ostream& episode::print(ostream& stream) const {
            curses::window* window = curses::terminal::instance().get_stream(stream).get_window();
            int width = window ? window->get_bounds().width : COLS;

            stream << get_id() << curses::stream::move(max_id_width);

            if (get_german_title() == "" || get_english_title() == "")
                return stream << util::platform::strip_chars(
                                    get_german_title() == "" ? get_english_title() : get_german_title());

            return stream <<
                   curses::stream::write_truncated(util::platform::strip_chars(get_german_title()),
                        (width - max_id_width) / 2) << " " << util::platform::strip_chars(get_english_title());
        }
        
        unique_ptr<aggregators::episode::file> episode::get_file() const {
            return unique_ptr<aggregators::episode::file>(new episode::file(*this));
        }
    }
}
