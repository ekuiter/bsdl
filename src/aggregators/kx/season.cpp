#include "season.hpp"
#include "kx.hpp"
#include "../../providers/provider.hpp"
#include "../../settings.hpp"
#include "merge_with_bs.hpp"
#include <unordered_map>
#include <Node.h>

namespace aggregators {
    namespace kx {
        void season::load(const http::response& response) const {
            for (auto episode_number : episode_numbers)
                add_episode(new episode(series_title, number, episode_number,
                        http::request(mirrors_request.get_url() + settings::get("kx_season_param") +
                            to_string(number) + settings::get("kx_episode_param") + to_string(episode_number))));
            
            loaded = true;
            merge_with_bs::instance()(const_cast<season*>(this));
        }

        ostream& season::print(ostream& stream) const {
            return stream << "Season #" << get_number();
        }
    }
}
