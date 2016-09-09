#include "response.hpp"

namespace http {
    unique_ptr<CDocument> response::parse() const {
        unique_ptr<CDocument> doc(new CDocument);
        doc->parse(get_body());
        return doc;
    }

    ostream& operator<<(ostream& stream, const response& response) {
        return stream << response.get_body();
    }
}