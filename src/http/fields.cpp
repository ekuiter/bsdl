#include "fields.hpp"
#include <curl_easy.h>

using namespace curl;

namespace http {
    ostream& operator<<(ostream& stream, const fields& fields) {
        curl_easy curl;
        int i = 0;
        for (auto& field_pair : fields) {
            string field = field_pair.first, value = field_pair.second;
            curl.escape(field);
            curl.escape(value);
            stream << field << "=" << value << (++i < fields.size() ? "&" : "");
        }
        return stream;
    }

    fields::operator string() const {
        ostringstream stream;
        stream << *this;
        return stream.str();
    }
}
