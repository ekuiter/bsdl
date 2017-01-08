#pragma once

#include <iostream>
#include <map>
#include "../../http/client.hpp"
#include "../aggregator.hpp"
#include "exception.hpp"
#include "season.hpp"
#include "../../util/platform.hpp"

using namespace std;

namespace aggregators {
    namespace kx {
        class series : public aggregators::series {
        private:
            void load(const http::response& response) const override;

        public:
            using aggregators::series::series;

            series(const string& _title, const http::response& response): series(_title, http::request()) {
                load(response);
            }
            
            ostream& print(ostream& stream) const override;
            unique_ptr<aggregators::episode::file> get_file(const string& old_file_name,
                const string& pattern_str) override;
        };
    }
}
