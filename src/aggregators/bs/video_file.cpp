#include "video_file.hpp"
#include "../../settings.hpp"
#include "captcha_solver.hpp"

namespace aggregators {
    namespace bs {
        void video_file::load_download_request() const {
            unique_ptr<CDocument> document = request().parse();
            http::request video_file_request =
                document->find(settings::get("bs_video_file_sel")).ASSERT_NUM(1).nodeAt(0).attribute("href");
            
            download_request = _provider.fetch(captcha_solver::instance()(video_file_request, _provider));
        }
    }
}
