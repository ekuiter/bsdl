#include "json_app.hpp"
#include "../option.hpp"
#include "../util/bsdl_uri.hpp"

using namespace nlohmann;

void json_app::initialize() {    
    if (settings.is_set("action") && settings["action"] != "uri") {
        if (settings["action"] == "version")
            throw runtime_error("version is not available in json mode");
        else if (settings["action"] == "help")
            help_message();
        else if (settings["action"] == "show-log")
            show_log();
        else if (settings["action"] == "edit-file")
            util::platform::edit(settings["config_file"]);
        else
            run_tests();
        exit(EXIT_SUCCESS);
    }
    
    aggregators::aggregator::set_preferred_aggregators(settings.get_preferred_aggregators());
    providers::provider::set_preferred_providers(settings.get_preferred_providers());
    aggregators::subtitle::set_preferred_subtitles(settings.get_preferred_subtitles());
}

void json_app::help_message() {
    terminal.with_output(cout, []() {
            auto bold_on = "\033[1m", bold_off = "\033[0m";
            cout << bold_on << "USAGE" << bold_off << endl << "    bsdl [series] [options]" <<
                endl << endl << bold_on << "OPTIONS" << bold_off << endl;
            for (auto& option : option::get_options())
                cout << "    " << bold_on << option.get_usage() << bold_off << endl <<
                    "        " << option.get_details() << endl << endl;
        });
}

void json_app::show_log() {
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

void json_app::run_tests() {
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

vector<aggregators::series*> json_app::search_series() {
    set_title("Search series");
    util::search_query query(settings["series_search"]);
    if (query.is_empty())
        throw runtime_error("no series given to search, run 'bsdl <series> ...'");
    series_search = query.get_search_string();
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

aggregators::series& json_app::choose_series(vector<aggregators::series*>& search_results, const string& prompt, const string& action) {
    set_title("Choose series");
    return *search_results[0];
}

void json_app::display_series(aggregators::series& series) {
    set_series_title(series);
    if (settings.is_set("rename_files_directory"))
        throw runtime_error("renaming files is not available in json mode");

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
