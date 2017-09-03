#include "cloudflare_hook.hpp"
#include "platform.hpp"
#include "../settings.hpp"

namespace util {
    static bool install_phantomjs() {
        static bool phantomjs_installed;
        if (phantomjs_installed)
            return true;

        if (!platform::print_version("phantomjs")) {
#ifdef __MINGW32__
            return false;
#elif defined(__APPLE__)
            platform::brew_install("phantomjs");
#else
            return false;
#endif
        }
        
        return phantomjs_installed = true;
    }

    string cloudflare_hook::user_agent() {
        return settings::get("cloudflare_user_agent");
    }
    
    bool cloudflare_hook::has_cloudflare(const http::response& response) {
        return response.get_body().find(settings::get("cloudflare_sel")) != string::npos;
    }
    
    string cloudflare_hook::fetch_clearance(http::request& request) {
        if (!has_clearance(request)) {
            if (install_phantomjs()) {
                cout << "Cloudflare protection detected, wait a few seconds ..." << endl;
                try {
                    string script_file_name = settings::instance().resource_file("cloudflare_hook.js");
                    if (!boost::filesystem::exists(script_file_name))
                        throw runtime_error(string("script file \"") + script_file_name + "\" does not exist");
                    get_clearance(request) =
                        util::platform::exec(string("phantomjs \"") + script_file_name + "\" \"" +
                                             request.get_url() + "\" \"" + user_agent() + "\"");
                } catch (platform::exec_failed) {
                    cerr << "phantomjs error" << endl;
                }
            } else
                cerr << "Cloudflare protection detected, install phantomjs to bypass it." << endl;
        } else
            clog << "Reusing an existing Cloudflare clearance." << endl;
        return get_clearance(request);
    }

    void cloudflare_hook::modify_request(http::request& request) {
        http::headers& headers = request.get_headers();
        headers["Cookie"] = fetch_clearance(request);
        headers["User-Agent"] = user_agent();
    }

    void cloudflare_hook::operator()(http::request& request) {
        if (has_clearance(request))
            modify_request(request);
    }

    http::response cloudflare_hook::operator()(http::request request, http::response response) {
        if (has_cloudflare(response)) {
            if (is_running_hook) {
                cerr << "Cloudflare challenge failed." << endl;
                return response;
            }
            modify_request(request);
            if (has_clearance(request)) {
                is_running_hook = true;
                response = request();
                is_running_hook = false;
            }
        }
        return response;
    }
}
