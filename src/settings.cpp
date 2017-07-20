#include "settings.hpp"
#include "app.hpp"
#include "util/platform.hpp"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

set<string> settings_base::allowed_settings = {"series_search", "output_files_directory", "rename_files_directory",
    "rename_files_pattern", "show_info", "log_file", "config_file", "bs_root_url", "bs_search_path",
    "bs_series_sel", "bs_season_sel", "bs_episode_sel", "bs_video_file_sel", "bs_movies_text",
    "kx_root_url", "kx_search_path", "kx_series_sel", "kx_title_sel", "kx_language_sel", "kx_season_sel",
    "kx_mirror_by_episode_path", "kx_mirror_path", "kx_season_param", "kx_episode_param", "kx_video_file_sel",
    "kx_stream_key", "kx_replacement_key", "providers", "aggregators", "aggregator_bs", "aggregator_kx", "aggregator_mk",
    "mk_root_url", "mk_search_path", "mk_series_sel", "mk_title_sel", "mk_language_sel", "mk_season_sel", "mk_episode_sel",
    "mk_video_file_sel", "mk_video_file_title_sel", "mk_video_file_url_sel", "mk_video_file_script_sel",
    "mk_video_file_script_regex", "mk_video_file_movie_regex", "mk_english_src", "mk_german_src"};

template <typename T>
static void print_vector(ostream& stream, vector<T*>& vector) {
    int i = 0;
    for (auto element : vector) {
        stream << *element;
        if (++i < vector.size())
            stream << ", ";
    }
}

template <typename T>
static vector<string> build_vector(const string& setting, vector<T*>& vector) {
    std::vector<string> elements;
    if (vector.size() == 0) {
        boost::split(elements, settings::get(setting), boost::is_any_of(","));
        for (auto& element : elements)
            boost::trim(element);
    }
    return elements;
}

ostream& operator<<(ostream& stream, settings_base& settings) {
    stream << "download_selection = " << settings.get_download_selection() << endl;
    stream << "preferred_aggregators = ";
    print_vector(stream, settings.preferred_aggregators);
    stream << endl << "preferred_providers = ";
    print_vector(stream, settings.preferred_providers);
    stream << endl;
    for (auto pair : settings.get_container())
        if (pair.second != "")
            stream << pair.first << " = " << pair.second << endl;
    return stream;
}

void settings_base::read(const vector<string>& args) {
    int first_arg = args.size() >= 2 && args[1][0] != '-' ? 2 : 1, i;
    auto next_arg = [&args, &i]() { return args[++i]; };
    auto skip_arg = [&next_arg](int num = 1) { while (num-- > 0) next_arg(); };
    auto is_arg = [&](string arg, int additional_args = 0) {
        if (i + additional_args >= args.size())
            return false;
        bool ret = args[i] == arg;
        for (int j = 1; j <= additional_args; j++)
            ret = ret && args[i + j][0] != '-';
        return ret;
    };

    for (i = first_arg; i < args.size(); i++) {
        if (is_arg("--download", 2) || is_arg("-d", 2))          skip_arg(2);
        else if (is_arg("--download", 1) || is_arg("-d", 1))     skip_arg();
        else if (is_arg("--download") || is_arg("-d"));
        else if (is_arg("--aggregator", 1) || is_arg("-a", 1))   skip_arg();
        else if (is_arg("--provider", 1) || is_arg("-p", 1))     skip_arg();
        else if (is_arg("--output-files", 1) || is_arg("-o", 1)) skip_arg();
        else if (is_arg("--rename-files", 2) || is_arg("-r", 2)) skip_arg(2);
        else if (is_arg("--rename-files", 1) || is_arg("-r", 1)) skip_arg();
        else if (is_arg("--rename-files") || is_arg("-r"));
        else if (is_arg("--log-file", 1) || is_arg("-l", 1))     skip_arg();
        else if (is_arg("--log-file") || is_arg("-l"));
        else if (is_arg("--config-file", 1) || is_arg("-c", 1))
            (*this)["config_file"] = next_arg();
        else if (is_arg("--help") || is_arg("-h"));
        else if (is_arg("--version") || is_arg("-v"));
        else
            throw runtime_error("illegal option, see bsdl --help");
    }

    if (!is_set("config_file"))
        (*this)["config_file"] = (util::platform::executable_path(args[0]).remove_filename() /= "bsdl.cfg").string();

    string config_file_name = (*this)["config_file"];
    if (!boost::filesystem::exists(config_file_name))
        throw runtime_error(string("config file \"") + config_file_name + "\" does not exist");
    std::ifstream config_file;
    config_file.open(config_file_name);
    string line;
    while (getline(config_file, line)) {
        boost::trim(line);
        if (line == "" || line[0] == '#')
            continue;
        vector<string> parts;
        boost::split(parts, line, boost::is_any_of("="));
        if (parts.size() < 2)
            throw runtime_error("invalid config file syntax");
        string key = parts[0], value = parts[1];
        for (int i = 2; i < parts.size(); i++)
            value += "=" + parts[i];
        boost::trim(key);
        boost::trim(value);
        (*this)[key] = value;
    }

    if (first_arg == 2)
        (*this)["series_search"] = args[1];

    vector<string> parts;
    for (i = first_arg; i < args.size(); i++) {
        if (is_arg("--download", 2) || is_arg("-d", 2)) {
            string season_number = next_arg(), number = next_arg();
            boost::split(parts, number, boost::is_any_of("-"));
            if (parts.size() == 1)
                _download_selection.add(new aggregators::download_selector::episode(stoi(season_number), stoi(number)));
            else if (parts.size() == 2 && stoi(parts[0]) <= stoi(parts[1]))
                for (int i = stoi(parts[0]); i <= stoi(parts[1]); i++)
                    _download_selection.add(new aggregators::download_selector::episode(stoi(season_number), i));
            else
                throw runtime_error("illegal option, see bsdl --help");
        } else if (is_arg("--download", 1) || is_arg("-d", 1)) {
            string season_number = next_arg();
            boost::split(parts, season_number, boost::is_any_of("-"));
            if (parts.size() == 1) {
                if (season_number == "latest")
                    _download_selection.add(new aggregators::download_selector::latest_episode);
                else
                    _download_selection.add(new aggregators::download_selector::season(stoi(season_number)));
            } else if (parts.size() == 2 && stoi(parts[0]) <= stoi(parts[1]))
                for (int i = stoi(parts[0]); i <= stoi(parts[1]); i++)
                    _download_selection.add(new aggregators::download_selector::season(i));
            else
                throw runtime_error("illegal option, see bsdl --help");
        } else if (is_arg("--download") || is_arg("-d"))
            _download_selection.add(new aggregators::download_selector::series);
        else if (is_arg("--aggregator", 1) || is_arg("-a", 1))
            preferred_aggregators.push_back(&aggregators::aggregator::instance(next_arg()));
        else if (is_arg("--provider", 1) || is_arg("-p", 1))
            preferred_providers.push_back(&providers::provider::instance(next_arg(), true));
        else if (is_arg("--output-files", 1) || is_arg("-o", 1))
            (*this)["output_files_directory"] = next_arg();
        else if (is_arg("--rename-files", 2) || is_arg("-r", 2))
            (*this)["rename_files_directory"] = next_arg(), (*this)["rename_files_pattern"] = next_arg();
        else if (is_arg("--rename-files", 1) || is_arg("-r", 1))
            (*this)["rename_files_directory"] = next_arg();
        else if (is_arg("--rename-files") || is_arg("-r"))
            (*this)["rename_files_directory"] = ".";
        else if (is_arg("--log-file", 1) || is_arg("-l", 1))
            (*this)["log_file"] = next_arg();
        else if (is_arg("--log-file") || is_arg("-l"))
            (*this)["log_file"] = "bsdl.log";
        else if (is_arg("--config-file", 1) || is_arg("-c", 1))
            (*this)["config_file"] = next_arg();
        else if (is_arg("--help") || is_arg("-h"))
            (*this)["show_info"] = "help";
        else if (is_arg("--version") || is_arg("-v"))
            (*this)["show_info"] = "version";
    }

    for (auto& aggregator : build_vector("aggregators", preferred_aggregators))
        preferred_aggregators.push_back(&aggregators::aggregator::instance(aggregator));
    
    for (auto& provider : build_vector("providers", preferred_providers))
        preferred_providers.push_back(&providers::provider::instance(provider, true));

    if (!is_set("output_files_directory"))
        (*this)["output_files_directory"] = ".";
}
