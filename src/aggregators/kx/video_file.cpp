#include "video_file.hpp"
#include "../../settings.hpp"
#include <json.hpp>
#include <regex>

using namespace nlohmann;

namespace aggregators {
    namespace kx {
        static int get_current_mirror(json _json) {
            CDocument doc;
            auto replacement = _json[settings::get("kx_replacement_key")];
            if (replacement.is_boolean())
                return 1;
            doc.parse(replacement);
            string mirror_text = doc.find(".Data").assertNum(1).nodeAt(0).text();
            smatch results;
            assert(regex_search(mirror_text, results, regex(": (.*)/")));
            return stoi(results[1]);
        }
        
        static bool get_next_mirror_request(json _json, http::request* _request) {
            CDocument doc;
            auto replacement = _json[settings::get("kx_replacement_key")];
            if (replacement.is_boolean())
                return false;
            doc.parse(replacement);
            *_request = _request->get_relative(doc.find("li").assertNum(1).nodeAt(0).attribute("rel"));
            return true;
        }
        
        void video_file::load_download_request() const {   
            json _json = json::parse(request().get_body());
            int current_mirror = get_current_mirror(_json);
            
            CDocument doc;
            doc.parse(_json[settings::get("kx_stream_key")]);
            http::request provider_request = doc.find("a").assertNum(1).nodeAt(0).attribute("href");
            
            if (first_mirror < 0)
                first_mirror = current_mirror;
            else if (first_mirror == current_mirror)
                throw providers::provider::not_found(provider_request);
            
            try {
                download_request = _provider.fetch(provider_request);
            } catch (providers::provider::not_found) {
                if (get_next_mirror_request(_json, &request)) {
                    clog << "Mirror " << current_mirror << " failed. Trying next ..." << endl;
                    load_download_request();
                } else
                    throw;
            }
        }
    }
}
