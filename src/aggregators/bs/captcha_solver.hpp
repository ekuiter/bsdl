#pragma once

#include <iostream>
#include "../../app.hpp"
#include "../../settings.hpp"
#include "../../util/platform.hpp"
#include "../../providers/provider.hpp"

using namespace std;

namespace aggregators {
    namespace bs {
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
                return document->find(settings::get("bs_captcha_sel")).nodeNum() > 0;
            }

            http::request operator()(http::request request, const providers::provider& provider) {
                if (has_captcha(request())) {
                    if (!util::platform::can_get_captcha_solution()) {
                        cerr << "Captcha detected, but can not be solved on " << util::platform::get_name() << "." << endl;
                        return request;
                    }
                    if (!app::instance().confirm("Captcha detected, solve it?", true))
                        return request;

                    cerr << "You need to visit " << settings::get("aggregator_bs") << " in your browser and solve the captcha once." << endl <<
                        "Visiting " << settings::get("aggregator_bs") << " ..." << endl;
                    util::platform::browse(request.get_url());
                    cerr << "Waiting until the captcha is solved ..." << endl;
                    util::platform::sleep(sleep_duration);

                    string solution;
                    while ((solution = util::platform::get_captcha_solution(request, provider)) == "") {
                        cerr << "The captcha is still unsolved." << endl;
                        util::platform::sleep(sleep_duration);
                    }
                    request = http::request(solution);
                }
                return request;
            }
        };
    }
}
