#include "video_file.hpp"
#include "../../settings.hpp"
#include <regex>

namespace aggregators {
    namespace mk {        
        void video_file::load_download_request() const {
            http::request provider_request;
            for (int mirror = 0; mirror < requests.size(); mirror++) {
                unique_ptr<CDocument> document = requests[mirror]().parse();
                provider_request = document->find(settings::get("mk_video_file_url_sel")).
                    assertNum(1).nodeAt(0).attribute("href");
                try {
                    download_request = _provider.fetch(provider_request);
                    return;
                } catch (providers::provider::not_found) {}
            }
            
            throw providers::provider::not_found(provider_request);
        }
    }
}
