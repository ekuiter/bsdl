#include "episode_download.hpp"
#include "../curses/terminal.hpp"
#include "../curses/widgets.hpp"
#include "../settings.hpp"
#include "../providers/provider.hpp"
#include <cmath>
#include <boost/filesystem.hpp>

using namespace curses;
using namespace boost::filesystem;

namespace bs {
    http::request::download& episode::download::get_download_request() {
        if (loaded)
            return download_request;

        const video_file& preferred_video_file = _episode.get_preferred_video_file();
        const providers::provider& provider = preferred_video_file.get_provider();

        message = provider.get_name();
        _refresh_callback(*this);
        download_request = preferred_video_file.get_download_request();

        cout << "Downloading " << color::get_accent_color() << _episode.get_id() <<
                color::previous << " from " << color::get_accent_color() <<
                provider.get_name() << color::previous << "." << endl;

        using namespace placeholders;
        download_request.set_file_name(get_file_path(settings::get("output_files_directory")))
                .set_callback(bind(&download::http_callback, this, _1, _2, _3, _4, _5));
        loaded = true;
        return download_request;
    }

    string episode::download::get_file_name() const {
        return _file.get_file_name();
    }

    string episode::download::get_file_path(string directory_name) const {
        path directory(directory_name);
        create_directory(directory);
        directory /= get_file_name();
        return directory.string();
    }

    int episode::download::http_callback(http::request::status status, const http::request& request,
                      curl_off_t _now_bytes, curl_off_t _total_bytes, curl::curl_easy_exception* e) {
        now_bytes = _now_bytes;
        total_bytes = _total_bytes;
        if (total_bytes > 0 && now_bytes == total_bytes)
            message = "Done";
        _refresh_callback(*this);
        input::instance().read();
        if (abort)
            download_request.remove_file();
        return abort ? -1 : 0;
    }

    ostream& operator<<(ostream& stream, const episode::download& episode_download) {
        window* _window = terminal::instance().get_stream(stream).get_window();
        int width = (_window ? _window->get_bounds().width : COLS) - 2, progress_bar_width = 20;

        const episode& _episode = episode_download.get_episode();
        double percentage = (double) episode_download.get_now_bytes() / episode_download.get_total_bytes();
        percentage = isnan(percentage) ? 0 : percentage;
        string message = episode_download.get_message();

        string file_name = util::platform::strip_chars(episode_download.get_file_name()),
                now = boost::str(boost::format("%.2f") % ((double) episode_download.get_now_bytes() / 1024 / 1024)),
                percentage_string = boost::str(boost::format("%.0f") % (percentage * 100));

        stream << stream::write_truncated(file_name, width - progress_bar_width - 1) << " ";
        if (episode_download.get_download_request_pointer()
                && message != "Done" && message != "Aborted" && message != "Unavailable")
            progress_bar::write(stream, progress_bar_width, percentage, percentage_string + "% (" + now + " MB)");
        else
            progress_bar::write(stream, progress_bar_width, message == "Done", message);
        return stream;
    }
}