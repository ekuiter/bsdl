#pragma once

#include <iostream>
#include <map>
#include "../../http/client.hpp"
#include "season.hpp"
#include "exception.hpp"
#include "../../util/platform.hpp"
#include "../subtitle.hpp"

using namespace std;

namespace aggregators {
    class aggregator;
    
    namespace pr {        
        class series : public aggregators::series {
        private:
            void load(const http::response& response) const override;
            string language;
            vector<aggregators::subtitle*> get_subtitles(const http::response& response) const;

        public:            
            series(const aggregator& aggregator, const string& _title, const string& _language, const http::request& _request):
                aggregators::series(aggregator, _title, _request) {
                language = _language;
            }
            
            ostream& print(ostream& stream) const override;
            unique_ptr<aggregators::episode::file> get_file(const string& old_file_name,
                const string& pattern_str) override;
        };
    }
}
