#include "video_file.hpp"
#include "../../settings.hpp"

namespace aggregators {
    namespace pr {
        void video_file::load_download_request() const {
            if (message != "")
                cout << curses::color::get_accent_color() << "Note" <<
                    curses::color::previous << ": " << message << endl;
            
            download_request = _provider.fetch(request);
        }
    }
}
