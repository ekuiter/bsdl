#include "video_file.hpp"
#include "../../settings.hpp"

namespace aggregators {
    namespace bs {
        void video_file::load_download_request() const {
            unique_ptr<CDocument> document = request().parse();
            string sel = settings::get("bs_video_file_sel");
            http::request provider_request;
            try {
               provider_request = document->find(sel + " a").assertNum(1).nodeAt(0).attribute("href");
            } catch (runtime_error) {
                provider_request = document->find(sel + " iframe").assertNum(1).nodeAt(0).attribute("src");
            }
            download_request = _provider.fetch(provider_request);
        }
    }
}
