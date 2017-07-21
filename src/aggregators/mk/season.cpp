#include "season.hpp"
#include "mk.hpp"
#include "../../providers/provider.hpp"
#include "../../settings.hpp"
#include "../bs/merge_transform.hpp"
#include <unordered_map>
#include <Node.h>

namespace aggregators {
    namespace mk {
        void season::load(const http::response& response) const {
            for (auto pair : episode_requests)
                add_episode(new episode(series_title, number, pair.first, pair.second));
            
            loaded = true;
            bs::merge_transform::instance()(const_cast<season*>(this));
        }

        ostream& season::print(ostream& stream) const {
            return stream << "Season #" << get_number();
        }
    }
}
