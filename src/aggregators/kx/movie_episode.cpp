#include "movie_episode.hpp"
#include "kx.hpp"
#include "../episode_download.hpp"
#include "../../curses/terminal.hpp"
#include "../../settings.hpp"
#include "episode_file.hpp"

namespace aggregators {
    namespace kx {        
        string movie_episode::get_id() const {
            return "";
        }
        
        ostream& movie_episode::print(ostream& stream) const {
            return stream << series_title;
        }
    }
}
