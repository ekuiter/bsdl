#include "episode_file.hpp"
#include "../../util/addressed.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace aggregators {
    namespace kx {
        string episode::file::get_file_name() const {            
            if (file_name != "")
                return file_name;
            
            const episode* kx_episode = dynamic_cast<const episode*>(_episode);            
            int i = -1;
            do {
                string file_format = kx_episode->get_preferred_video_file()->get_provider().get_file_format(),
                    counter = ++i > 0 ? " (" + to_string(i) + ")" : "";
                file_name = kx_episode->get_series_title() + " " + get_id() + counter + "." + file_format;
                for (auto special_char : "<>:\"/\\|?*")
                    replace(file_name.begin(), file_name.end(), special_char, '_');
            } while (boost::filesystem::exists(file_name));
            
            return file_name;
        }
    }
}
