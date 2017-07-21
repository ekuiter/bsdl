#pragma once

#include <iostream>
#include <map>
#include "../../http/client.hpp"
#include "../aggregator.hpp"
#include "exception.hpp"
#include "season.hpp"
#include "../bs/merge_transform.hpp"
#include "../../util/platform.hpp"
#include "../../settings.hpp"

using namespace std;

namespace aggregators {
    class aggregator;
    
    namespace mk {
        class series : public aggregators::series {
        private:
            void load(const http::response& response) const override;
            string language;

        public:
            static string to_language_string(const string& src) {
                if (src == settings::get("mk_english_src")) return "Eng";
                if (src == settings::get("mk_german_src")) return "Ger";
                return "";
            }
            
            series(const aggregator& aggregator, const string& _title, const string& _language, const http::request& _request):
                aggregators::series(aggregator, _title, _request) {
                language = _language;
                title = util::platform::encode(title);
            }
            
            ostream& print(ostream& stream) const override;
            unique_ptr<aggregators::episode::file> get_file(const string& old_file_name,
                const string& pattern_str) override;
        };
    }
}
