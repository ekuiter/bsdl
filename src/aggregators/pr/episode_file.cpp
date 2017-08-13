#include "episode_file.hpp"
#include "../../app.hpp"
#include "../../util/addressed.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace aggregators {
    namespace pr {
        string episode::file::get_file_name() const {            
            if (file_name != "")
                return file_name;
            
            const episode* pr_episode = dynamic_cast<const episode*>(_episode);            
            int i = -1;
            do {
                string id = pr_episode->get_id() != "" ? string(" ") + pr_episode->get_id() : "",
                    counter = ++i > 0 ? " (" + to_string(i) + ")" : "";
                file_name = pr_episode->get_series_title() + id + counter;
                for (auto special_char : "<>:\"/\\|?*")
                    replace(file_name.begin(), file_name.end(), special_char, '_');
            } while (similar_file_exists(file_name));
            
            return file_name;
        }
    }
}