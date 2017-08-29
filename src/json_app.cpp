#include "app.hpp"

void json_app::initialize() {
    if (settings.is_set("log_file")) {
        log_file.open(settings["log_file"]);
        terminal.get_stream(cout).set_log_file(&log_file);
        terminal.get_stream(cerr).set_log_file(&log_file);
        terminal.get_stream(clog).set_log_file(&log_file);
    }

    http::client::instance().set_timeout(stoi(settings::get("timeout")));
    set_title("bsdl");
    clog << "bsdl initialized in JSON mode." << endl
         << endl << "Settings:" << endl << settings << endl;
    if (settings.is_set("action"))
        throw runtime_error(settings["action"] + " is not available in JSON mode");

    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
    aggregators::subtitle::set_preferred_subtitles(settings.get_preferred_subtitles());
}

void json_app::set_title(const string& title, bool set_notice, string notice) {
    if (!set_notice)
        clog << endl << title << endl << stream::write("-", title.length()) << endl;
}

vector<aggregators::series*> json_app::search_series() {
    set_title("Search series");
    series_search = settings["series_search"];
    if (series_search == "")
        throw runtime_error("no series given to search, run 'bsdl <series> ...'");
    vector<aggregators::series*> search_results = aggregators::aggregator::search(series_search);
    if (search_results.size() == 0)
        throw runtime_error(string("no series found for '") + series_search + "'");
    return search_results;
}

aggregators::series& json_app::choose_series(vector<aggregators::series*>& search_results, const string& prompt, const string& action) {
    set_title("Choose series");
    return *search_results[0]; // TODO
}

void json_app::display_series(aggregators::series& series) {
    series.load();
    set_title(series.get_title());
    if (settings.is_set("rename_files_directory"))
        throw runtime_error("renaming files is not available in JSON mode");
    if (settings.get_download_selection().size() > 0)
        terminal.with_output(cout, [this]() {
                cout << "I shall download: " << settings.get_download_selection() << endl; // TODO
            });
}
