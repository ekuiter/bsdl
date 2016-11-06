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
            
            const episode* _episode = dynamic_cast<const episode*>(_episode);
            int i = -1;
            do {
                string file_format = _episode->get_preferred_video_file()->get_provider().get_file_format(),
                    title_de = _episode->get_german_title(), title_en = _episode->get_english_title(),
                    title = title_de == title_en ? title_de :
                            title_de == "" ? title_en :
                            title_en == "" ? title_de : title_de + " (" + title_en + ")",
                    counter = ++i > 0 ? " (" + to_string(i) + ")" : "";
                file_name = _episode->get_series_title() + " " + get_id() + " " + title + counter + "." + file_format;
                for (auto special_char : "<>:\"/\\|?*")
                    replace(file_name.begin(), file_name.end(), special_char, '_');
            } while (boost::filesystem::exists(file_name));

            return file_name;
        }

        int episode::file::get_number_of_seasons() const {
            if (!app::instance().get_current_series())
                throw exception("there is no current series");
            return app::instance().get_current_series()->season_number();
        }
    }
}
