#include "series.hpp"
#include "bs.hpp"
#include "../curses/terminal.hpp"
#include "../settings.hpp"
#include <Node.h>

namespace bs {
    void series_base::load() const {
        if (!loaded) {
            cout << "Loading series " << curses::color::get_accent_color() <<
                    *this << curses::color::previous << "." << endl;
            load(request());
        }
    }

    void series_base::load(const http::response& response) const {
        unique_ptr<CDocument> document = response.parse();
        CSelection season_nodes = document->find(settings::get("season_sel")).assertAtLeast(1);

        for (auto& season_node : season_nodes) {
            string season_text = CNode(season_node).text();
            int season_number = season_text == settings::get("movies_text") ? 0 : stoi(season_text);
            string season_url = CNode(season_node).attribute("href");
            http::request season_request(bs::root().get_relative(season_url));
            add_season(season_number == 1 ? season(title, 1, response) : season(title, season_number, season_request));
        }

        loaded = true;
    }

    ostream& operator<<(ostream& stream, const series_base& series) {
        return stream << util::platform::strip_chars(series.get_title());
    }
}
