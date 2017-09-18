#include "settings.hpp"
#include "option.hpp"
#include "app.hpp"
#include "util/platform.hpp"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

set<string> settings_base::allowed_settings =
    {"series_search", "output_files_directory", "rename_files_directory",
     "rename_files_pattern", "action", "log_file", "config_file", "bs_root_url", "bs_search_path",
     "bs_series_sel", "bs_season_sel", "bs_episode_sel", "bs_video_file_sel", "bs_movies_text",
     "kx_root_url", "kx_search_path", "kx_series_sel", "kx_title_sel", "kx_language_sel", "kx_season_sel",
     "kx_mirror_by_episode_path", "kx_mirror_path", "kx_season_param", "kx_episode_param", "kx_video_file_sel",
     "kx_stream_key", "kx_replacement_key", "providers", "aggregators", "aggregator_bs", "aggregator_kx", "aggregator_mk",
     "mk_root_url", "mk_search_path", "mk_series_sel", "mk_title_sel", "mk_language_sel", "mk_season_sel", "mk_episode_sel",
     "mk_video_file_sel", "mk_video_file_title_sel", "mk_video_file_url_sel", "mk_video_file_script_sel", "mk_exclude_regex",
     "mk_video_file_script_regex", "mk_video_file_movie_regex", "mk_english_src", "mk_german_src", "aggregator_pr",
     "pr_root_url", "pr_search_path", "pr_series_sel", "pr_season_sel", "pr_episode_sel", "pr_video_file_sel",
     "pr_title_sel", "pr_language_sel", "pr_list_path", "subtitles", "pr_subtitle_sel", "pr_subtitle_key", "pr_episode_row_sel_1",
     "pr_episode_row_sel_2", "pr_episode_row_img", "pr_video_file_script_regex", "pr_captcha_sel", "parallel_transfers",
     "timeout", "app", "cloudflare_sel", "cloudflare_user_agent", "output_files_mode", "monitor_file" };
unique_ptr<settings> settings_base::_instance(new settings());

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

vector<aggregators::aggregator*> settings_base::update_preferred_aggregators(bool clear_vector) {
    if (clear_vector)
        preferred_aggregators.clear();
    for (auto& aggregator : build_vector("aggregators", preferred_aggregators))
        preferred_aggregators.push_back(&aggregators::aggregator::instance(aggregator));
    return preferred_aggregators;
}

vector<providers::provider*> settings_base::update_preferred_providers(bool clear_vector) {
    if (clear_vector)
        preferred_providers.clear();
    for (auto& provider : build_vector("providers", preferred_providers))
        preferred_providers.push_back(&providers::provider::instance(provider, true));
    return preferred_providers;
}

vector<aggregators::subtitle*> settings_base::update_preferred_subtitles(bool clear_vector) {
    if (clear_vector)
        preferred_subtitles.clear();
    for (auto& subtitle : build_vector("subtitles", preferred_subtitles))
        preferred_subtitles.push_back(&aggregators::subtitle::instance(subtitle));
    return preferred_subtitles;
}

ostream& operator<<(ostream& stream, settings_base& settings) {
    stream << "download_selection = " << settings.get_download_selection() << endl;
    stream << "preferred_aggregators = ";
    print_vector(stream, settings.preferred_aggregators);
    stream << endl << "preferred_providers = ";
    print_vector(stream, settings.preferred_providers);
    stream << endl << "preferred_subtitles = ";
    print_vector(stream, settings.preferred_subtitles);
    stream << endl;
    for (auto pair : settings.get_container())
        if (pair.second != "")
            stream << pair.first << " = " << pair.second << endl;
    return stream;
}

void settings_base::set(const string& key, const string& value) {
    if ((key == "output_files_directory" || key == "rename_files_directory") && !boost::filesystem::exists(value))
        throw runtime_error(string("directory \"") + value + "\" does not exist");
    (*this)[key] = value;
}

template<typename T, typename U>
void settings_base::validate_usage(const vector<string>& args, int& i, int first_arg, T is_arg, U next_arg) {
    for (i = first_arg; i < args.size(); i++) {
        bool valid = false;
        for (auto& option : option::get_options())
            if (option.validate(is_arg, next_arg)) {
                valid = true;
                break;
            }
        if (!valid)
            throw runtime_error("illegal option, see bsdl --help");
    }
}

void settings_base::read_config_file() {
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
}

template<typename T, typename U>
void settings_base::process_args(const vector<string>& args, int& i, int first_arg, T is_arg, U next_arg) {
    for (i = first_arg; i < args.size(); i++)
        for (auto& option : option::get_options())
            if (option.process(is_arg, next_arg))
                break;
}

void settings_base::read(const vector<string>& args) {
    int first_arg = args.size() >= 2 && args[1][0] != '-' ? 2 : 1, i;
    auto next_arg = [&args, &i]() { return args[++i]; };
    auto is_arg = [&](string arg, int additional_args = 0) {
        if (i + additional_args >= args.size())
            return false;
        bool ret = args[i] == arg;
        for (int j = 1; j <= additional_args; j++)
            ret = ret && args[i + j][0] != '-';
        return ret;
    };

    (*this)["app"] = "main";
    set("output_files_directory", ".");
    (*this)["log_file"] = default_log_file();
    (*this)["monitor_file"] = default_monitor_file();

    option::setup_options();
    validate_usage(args, i, first_arg, is_arg, next_arg);
    if (!is_set("config_file"))
        (*this)["config_file"] = default_config_file();
    read_config_file();
    if (first_arg == 2)
        (*this)["series_search"] = args[1];
    process_args(args, i, first_arg, is_arg, next_arg);

    if ((*this)["output_files_directory"] != "")
        set("output_files_directory", (*this)["output_files_directory"]);
    if ((*this)["rename_files_directory"] != "")
        set("rename_files_directory", (*this)["rename_files_directory"]);

    update_preferred_aggregators();
    update_preferred_providers();
    update_preferred_subtitles();
}

string settings_base::resource_file(const string& filename) {
    return (util::platform::executable_path().remove_filename() /= filename).string();
}

string settings_base::default_config_file() {
    return resource_file("bsdl.cfg");
}

string settings_base::default_log_file() {
    return resource_file("bsdl.log");
}

string settings_base::default_monitor_file() {
    return resource_file("bsdlmon.cfg");
}

string settings_base::output_files_directory(const string& series_title, bool may_create) {
    using namespace boost::filesystem;
    path directory = (*this)["output_files_directory"];
    if ((*this)["output_files_mode"] == "subdirectories" && series_title != "") {
        directory /= series_title;
        if (may_create && !exists(directory) && !create_directory(directory))
            throw runtime_error(string("directory \"") + directory.string() + "\" could not be created");
    }
    return directory.string();
}
