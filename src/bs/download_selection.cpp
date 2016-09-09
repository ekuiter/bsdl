#include "download_selection.hpp"

namespace bs {
    ostream& operator<<(ostream& stream, download_selection& download_selection) {
        int i = 0;
        for (auto& download_selector : download_selection) {
            stream << *download_selector;
            if (++i < download_selection.size())
                stream << ", ";
        }
        return stream;
    }
}
