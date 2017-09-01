#include "app.hpp"
#include "option.hpp"
#include "util/bsdl_uri.hpp"

using namespace nlohmann;

void batch_app::initialize() {
    if (settings.is_set("log_file") && settings["action"] != "show-log") {
        log_file.open(settings["log_file"]);
        terminal.get_stream(cout).set_log_file(&log_file);
        terminal.get_stream(cerr).set_log_file(&log_file);
        terminal.get_stream(clog).set_log_file(&log_file);
    }

    http::client::instance().set_timeout(stoi(settings::get("timeout")));
    set_title("bsdl");
    clog << "bsdl initialized in batch mode." << endl
         << endl << "Settings:" << endl << settings << endl;
    
    if (settings.is_set("action") && settings["action"] != "uri") {
        if (settings["action"] == "version")
            throw runtime_error("version is not available in batch mode");
        else if (settings["action"] == "help")
            help_message();
        else if (settings["action"] == "show-log")
            show_log();
        else
            run_tests();
        exit(EXIT_SUCCESS);
    }
    
    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
    aggregators::subtitle::set_preferred_subtitles(settings.get_preferred_subtitles());
}

void batch_app::help_message() {
    terminal.with_output(cout, []() {
            cout << "usage: bsdl [series]" << endl;
            for (auto& option : option::get_options())
                cout << "\t[" << option.get_usage() << "]" << endl;
        });
}

void batch_app::show_log() {
    string log_file_name = settings["log_file"];
    if (!boost::filesystem::exists(log_file_name))
        throw runtime_error(string("log file \"") + log_file_name + "\" does not exist");
    terminal.with_output(cout, [this]() {
            ifstream log_file;
            log_file.open(settings["log_file"]);
            string line;
            while (getline(log_file, line))
                cout << line << endl;
        });
}

void batch_app::run_tests() {
    string args = settings["action"] == "test" ? "" :
        string(" --run_test=") + (settings["action"] == "quick" ? "!@long_running" : settings["action"]);
    if (settings["action"] == "list")
        args = " --list_content";
    string bsdltest = settings::instance().resource_file("bsdltest");
    if (!boost::filesystem::exists(bsdltest))
        throw runtime_error("bsdltest not found, did you compile it?");
    FILE* pipe = popen((bsdltest + args).c_str(), "r");
    if (!pipe)
        throw runtime_error("popen failed");
    terminal.with_output(cout, [&pipe]() {
            char buffer[128];
            while (!feof(pipe))
                if (fgets(buffer, 128, pipe) != NULL)
                    cout << buffer;
        });
    exit(pclose(pipe) / 256);
}

void batch_app::set_title(const string& title, bool set_notice, string notice) {
    if (!set_notice)
        clog << endl << title << endl << stream::write("-", title.length()) << endl;
}

vector<aggregators::series*> batch_app::search_series() {
    set_title("Search series");
    util::search_query query(settings["series_search"]);
    series_search = query.get_search_string();
    if (query.is_empty())
        throw runtime_error("no series given to search, run 'bsdl <series> ...'");
    vector<aggregators::series*> _search_results = query.fetch_results();
    if (_search_results.size() == 0)
        throw runtime_error(string("no series found for '") + series_search + "'");

    if (settings["action"] == "uri") {
        terminal.with_output(cout, [&_search_results]() {
                for (auto series : _search_results) {
                    cout << util::platform::strip_chars(series->get_title()) << endl
                         << '\t' << util::bsdl_uri(*series).get_uri() << endl;
                }
            });
        exit(EXIT_SUCCESS);
    }
    
    return search_results = query.check_unambiguous(_search_results);
}

aggregators::series& batch_app::choose_series(vector<aggregators::series*>& search_results, const string& prompt, const string& action) {
    set_title("Choose series");
    return *search_results[0];
}

void batch_app::display_series(aggregators::series& series) {
    set_title(series.get_title());
    if (settings.is_set("rename_files_directory"))
        throw runtime_error("renaming files is not available in batch mode");

    auto& download_selection = settings.get_download_selection();
    auto episodes = download_selection.get_episodes(*current_series);
    auto _json = "[]"_json;
    transform(episodes.begin(), episodes.end(), back_inserter(_json), [](aggregators::episode* episode) {
            return episode->get_json();
        });
    
    terminal.with_output(cout, [&_json, &download_selection]() {
            if (download_selection.size() == 0)
                throw runtime_error("no download selectors given, use --download option");
            cout << _json << endl;
        });
}
