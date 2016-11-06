#include "episode.hpp"
#include "episode_download.hpp"

namespace aggregators {
    unique_ptr<episode::download> episode::get_download(episode::download::refresh_callback _refresh_callback) const {
        return unique_ptr<episode::download>(new episode::download(*this, _refresh_callback));
    }
}
