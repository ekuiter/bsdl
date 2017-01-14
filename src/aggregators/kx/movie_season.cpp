#include "movie_season.hpp"
#include "movie_episode.hpp"
#include "kx.hpp"
#include "../../providers/provider.hpp"
#include "../../settings.hpp"
#include <unordered_map>
#include <Node.h>

namespace aggregators {
    namespace kx {
        void movie_season::load(const http::response& response) const {
            add_episode(new movie_episode(series_title, response));
            loaded = true;
        }

        ostream& movie_season::print(ostream& stream) const {
            return stream << "Movie";
        }
    }
}
