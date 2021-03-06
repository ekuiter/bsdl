#pragma once

#include <iostream>
#include <map>
#include "../../http/client.hpp"
#include "../aggregator.hpp"
#include "exception.hpp"
#include "season.hpp"
#include "../bs/merge_transform.hpp"
#include "../../util/platform.hpp"
#include "../bs/merge_transform.hpp"

using namespace std;

namespace aggregators {
    class aggregator;
    
    namespace kx {
        class series : public bs::mergeable_series {
        private:
            void load(const http::response& response) const override;
            string language;

        public:
            static string to_language_string(int language) {
                if (language == 1)  return "Ger";
                if (language == 2)  return "Eng";
                if (language == 15) return "Sub";
                return "";
            }
            
            series(const aggregator& aggregator, const string& _title, const string& _language, const http::request& _request):
                bs::mergeable_series(aggregator, _title, _request) {
                language = _language;
                title = util::platform::encode(title);
            }
            
            ostream& print(ostream& stream) const override;
            unique_ptr<aggregators::episode::file> get_file(const string& old_file_name,
                const string& pattern_str) override;
        };
    }
}
