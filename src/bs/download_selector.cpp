#include "download_selector.hpp"

namespace bs {
    ostream& operator<<(ostream& stream, download_selector& _download_selector) {
        return stream << (string) _download_selector;
    }
}
