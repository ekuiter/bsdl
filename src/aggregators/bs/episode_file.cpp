#include "episode_file.hpp"
#include "../../app.hpp"
#include "../../util/addressed.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace aggregators {
    namespace bs {
        string episode::file::get_file_name() const {            
            if (file_name != "")
                return file_name;
            
            const episode* bs_episode = dynamic_cast<const episode*>(_episode);            
            int i = -1;
            do {
                string title_de = bs_episode->get_german_title(), title_en = bs_episode->get_english_title(),
                    title = title_de == title_en ? title_de :
                            title_de == "" ? title_en :
                            title_en == "" ? title_de : title_de + " (" + title_en + ")",
                    counter = ++i > 0 ? " (" + to_string(i) + ")" : "";
                file_name = bs_episode->get_series_title() + " " + bs_episode->get_id() + " " + title + counter;
                for (auto special_char : "<>:\"/\\|?*")
                    replace(file_name.begin(), file_name.end(), special_char, '_');
            } while (similar_file_exists(file_name, bs_episode->get_series_title()));
            
            return file_name;
        }
    }
}
