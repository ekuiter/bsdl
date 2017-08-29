#include "video_file.hpp"
#include "../../settings.hpp"
#include "../../curses/terminal.hpp"

namespace aggregators {
    namespace pr {
        void video_file::load_download_request() const {
            if (message != "")
                cout << curses::stream::colored("Note") << ": " << message << endl;
            
            download_request = _provider.fetch(request);
        }
    }
}
