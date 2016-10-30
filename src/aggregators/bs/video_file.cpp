#include "video_file.hpp"
#include "../../settings.hpp"

namespace aggregators {
    namespace bs {
        void video_file::load_download_request() const {
            unique_ptr<CDocument> document = request().parse();
            download_request = _provider.fetch(
                    document->find(settings::get("video_file_sel")).assertNum(1).nodeAt(0).attribute("href"));
        }
    }
}
