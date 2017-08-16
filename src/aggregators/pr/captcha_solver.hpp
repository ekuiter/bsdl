#pragma once

#include <iostream>
#include "../../app.hpp"
#include "../../settings.hpp"
#include "../../util/platform.hpp"
#include "exception.hpp"

using namespace std;

namespace aggregators {
    namespace pr {
        class captcha_solver {
            chrono::seconds sleep_duration;
            
            captcha_solver(): sleep_duration(3) {}
            
        public:
            static captcha_solver& instance() {
                static captcha_solver captcha_solver;
                return captcha_solver;
            }

            bool has_captcha(const http::response& response) {
                unique_ptr<CDocument> document = response.parse();
                return document->find(settings::get("pr_captcha_sel")).nodeNum() > 0;
            }

            http::response operator()(const http::request& request, http::response response) {
                if (has_captcha(response)) {
                    cerr << "You need to visit " << settings::get("aggregator_pr") << " in your browser and solve the captcha once." << endl;
                    util::platform::sleep(sleep_duration);
                    cerr << "Visiting " << settings::get("aggregator_pr") << " ..." << endl;
                    util::platform::browse(request.get_url());
                    cerr << "Waiting until the captcha is solved ..." << endl;
                    util::platform::sleep(sleep_duration);
                    
                    while (has_captcha(response = request())) {
                        cerr << "The captcha is still unsolved." << endl;
                        util::platform::sleep(sleep_duration);
                    }
                }
                return response;
            }
        };
    }
}
