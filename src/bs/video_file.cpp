#include "video_file.hpp"
#include "../app/settings.hpp"

namespace bs {
    video_file video_file::unavailable_video_file(provider::instance("Unavailable"), http::request());

    void video_file::load() const  {
        if (!loaded) {
            if (this == &unavailable_video_file)
                return;
            unique_ptr<CDocument> document = request().parse();
            download_request = _provider.fetch(
                    document->find(settings::get("video_file_sel")).assertNum(1).nodeAt(0).attribute("href"));
            loaded = true;
        }
    }

    ostream& operator<<(ostream& stream, const video_file& video_file) {
        if (video_file.loaded)
            return stream << video_file.get_provider() << " at " << video_file.get_download_request();
        else
            return stream << video_file.get_provider();
    }
}
