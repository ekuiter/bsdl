#include "episode_file.hpp"
#include "../../app.hpp"
#include "../../util/addressed.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace aggregators {
    namespace pr {
        episode::file::file(series& _series, const string& _old_file_name, string pattern_str): aggregators::episode::file(_old_file_name) {
            regex pattern(" (\\d+)");
            smatch results;
            string msg = string("could not recognize \"") + old_file_name + "\"";

            if (!regex_search(old_file_name, results, pattern))
                throw aggregators::exception(msg);

            int number = stoi(results[1]);
            for (auto season : _series)
                for (auto episode : *season)
                    if (episode->get_number() == number)
                        _episode = episode;

            if (!_episode)
                throw aggregators::exception(msg);
        }
        
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
            } while (similar_file_exists(file_name, pr_episode->get_series_title()));
            
            return file_name;
        }
    }
}
