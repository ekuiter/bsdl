#include "episode_file.hpp"
#include "../../util/addressed.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace aggregators {
    namespace mk {
        string episode::file::get_file_name() const {            
            const episode* mk_episode = dynamic_cast<const episode*>(_episode);
            if (!bs_episode) {
                bs_episode = mk_episode->get_bs_episode();
                if (bs_episode)
                    file_name = bs_episode->get_file()->get_file_name();
            }
            if (file_name != "")
                return file_name;
            
            int i = -1;
            do {
                string id = mk_episode->get_id() != "" ? string(" ") + mk_episode->get_id() : "",
                    counter = ++i > 0 ? " (" + to_string(i) + ")" : "";
                file_name = mk_episode->get_series_title() + id + counter;
                for (auto special_char : "<>:\"/\\|?*")
                    replace(file_name.begin(), file_name.end(), special_char, '_');
            } while (similar_file_exists(file_name));
            
            return file_name;
        }
    }
}
