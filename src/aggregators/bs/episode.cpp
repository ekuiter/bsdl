#include "episode.hpp"
#include "episode_download.hpp"
#include "episode_file.hpp"
#include "../../curses/terminal.hpp"

namespace aggregators {
    namespace bs {
        string episode::get_id() const {
            return file(*this).get_id();
        }

        episode::download episode::get_download(episode::download::refresh_callback _refresh_callback) const {
            return episode::download(*this, _refresh_callback);
        }

        ostream& operator<<(ostream& stream, const episode& episode) {
            curses::window* window = curses::terminal::instance().get_stream(stream).get_window();
            int width = window ? window->get_bounds().width : COLS, max_id_width = 13;

            stream << episode.get_id() << curses::stream::move(max_id_width);

            if (episode.get_german_title() == "" || episode.get_english_title() == "")
                return stream << util::platform::strip_chars(
                                    episode.get_german_title() == "" ? episode.get_english_title() : episode.get_german_title());

            return stream <<
                   curses::stream::write_truncated(util::platform::strip_chars(episode.get_german_title()),
                                       (width - max_id_width) / 2) << " " << util::platform::strip_chars(episode.get_english_title());
        }
    }
}
