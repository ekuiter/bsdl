#include "option.hpp"
#include "settings.hpp"
#include "app.hpp"
#include "curses/terminal.hpp"
#include "curses/widgets.hpp"

using namespace curses;

#define HANDLERS(on_validate, on_process, params)                       \
    [&settings] params { on_validate; }, [&settings] params { on_process; }
#define MODIFIABLE_OPTION_ARG0(key, on_validate, on_process, on_modify, description) \
    { #key, {}, HANDLERS(on_validate, on_process, (string, string)), on_modify, description }
#define MODIFIABLE_OPTION_ARG1(key, arg1, on_validate, on_process, on_modify, description) \
    { #key, { #arg1 }, HANDLERS(on_validate, on_process, (string arg1, string)), on_modify, description }
#define MODIFIABLE_OPTION_ARG2(key, arg1, arg2, on_validate, on_process, on_modify, description) \
    { #key, { #arg1, #arg2 }, HANDLERS(on_validate, on_process, (string arg1, string arg2)), on_modify, description }
#define OPTION_ARG0(key, on_validate, on_process) MODIFIABLE_OPTION_ARG0(key, on_validate, on_process, nullptr, "")
#define OPTION_ARG1(key, arg1, on_validate, on_process) MODIFIABLE_OPTION_ARG1(key, arg1, on_validate, on_process, nullptr, "")
#define OPTION_ARG2(key, arg1, arg2, on_validate, on_process) MODIFIABLE_OPTION_ARG2(key, arg1, arg2, on_validate, on_process, nullptr, "")

#define DOWNLOAD_OPTION(var, single_selector, selector_range) {         \
        auto& download_selection = settings._download_selection;        \
        vector<string> parts;                                           \
        boost::split(parts, var, boost::is_any_of("-"));                \
        if (parts.size() == 1) {                                        \
            single_selector;                                            \
        } else if (parts.size() == 2 && stoi(parts[0]) <= stoi(parts[1])) { \
            for (int i = stoi(parts[0]); i <= stoi(parts[1]); i++)      \
                selector_range;                                         \
        } else                                                          \
            throw runtime_error("illegal option, see bsdl --help");     \
    }

#define MODIFY_BOUNDS app::instance().get_centered_bounds(-1, 8)

#define MODIFY_PREFERRED(name, setter, exception)               \
    [&settings]() {                                             \
        modify_setting(#name, "\nEnter preferred " #name ":");  \
        try {                                                   \
            setter(settings.update_preferred_##name(true));     \
        } catch (exception e) {                                 \
            cerr << e.what() << endl;                           \
        }                                                       \
    }

#define IN_MODE(mode, code) {                   \
        settings["app"] = #mode;                \
        code;                                   \
    }

vector<option> option::options;

static string modify_setting(const string& setting, const string& prompt, bool retry = true) {
    settings& settings = settings::instance();
    do {
        {
            window::framed input_window(MODIFY_BOUNDS);
            try {
                settings.set(setting, input_dialog::run(input_window, prompt, "Save", color::get_accent_color(),
                                                        settings[setting], nullptr, true));
            } catch (runtime_error e) {
                cerr << e.what() << endl;
            }
        }
        boost::trim(settings[setting]);
        if (retry && !settings.is_set(setting))
            cerr << "Please enter something." << endl;
    } while (retry && !settings.is_set(setting));
    return settings[setting];
};

void option::setup_options() {
    settings& settings = settings::instance();

    auto modify_output_files = [&settings](){
        modify_setting("output_files_directory", string(COLS >= 82 ? "\n" : "") + "Enter output directory for episodes:");
        cout << "Episodes will be saved to " << stream::colored(settings["output_files_directory"]) << "." << endl;
    };

    auto modify_rename_files = [&settings]() {
        modify_setting("rename_files_directory", string(COLS >= 89 ? "\n" : "") + "Enter directory with episodes to rename:", false);
        modify_setting("rename_files_pattern", "\nEnter renaming pattern:", false);
        string pattern = settings["rename_files_pattern"] == "" ? "default" : settings["rename_files_pattern"];
        if (!settings.is_set("rename_files_directory"))
            cout << "No episodes will be renamed." << endl;
        else
            cout << "Episodes in " << stream::colored(settings["rename_files_directory"]) <<
                " will be renamed using the pattern " << stream::colored(pattern) << "." << endl;
    };

    auto modify_aggregators = MODIFY_PREFERRED(aggregators, aggregators::aggregator::set_preferred_aggregators, aggregators::exception);
    auto modify_providers = MODIFY_PREFERRED(providers, providers::provider::set_preferred_providers, providers::provider::exception);
    auto modify_subtitles = MODIFY_PREFERRED(subtitles, aggregators::subtitle::set_preferred_subtitles, aggregators::exception);

    options = {
        OPTION_ARG2(download, season, episode, {},
                    DOWNLOAD_OPTION(episode,
                                    download_selection.add(new aggregators::download_selector::episode(stoi(season), stoi(episode))),
                                    download_selection.add(new aggregators::download_selector::episode(stoi(season), i)))),
        OPTION_ARG1(download, season, {},
                    DOWNLOAD_OPTION(season, {
                            if (season == "latest")
                                download_selection.add(new aggregators::download_selector::latest_episode);
                            else if (season == "new")
                                download_selection.add(new aggregators::download_selector::new_episodes);
                            else
                                download_selection.add(new aggregators::download_selector::season(stoi(season)));
                        }, download_selection.add(new aggregators::download_selector::season(i)))),
        OPTION_ARG0(download, {}, settings._download_selection.add(new aggregators::download_selector::series)),
        
        MODIFIABLE_OPTION_ARG1(aggregator, aggregator, {}, settings.preferred_aggregators.push_back(&aggregators::aggregator::instance(aggregator)),
                               modify_aggregators, "Preferred aggregators"),
        MODIFIABLE_OPTION_ARG1(provider, provider, {}, settings.preferred_providers.push_back(&providers::provider::instance(provider, true)),
                               modify_providers, "Preferred providers"),
        MODIFIABLE_OPTION_ARG1(subtitle, subtitle, {}, settings.preferred_subtitles.push_back(&aggregators::subtitle::instance(subtitle)),
                               modify_subtitles, "Preferred subtitles"),
        
        MODIFIABLE_OPTION_ARG1(output-files, directory, {}, settings.set("output_files_directory", directory), modify_output_files, "Output directory"),
        MODIFIABLE_OPTION_ARG2(rename-files, directory, pattern, {}, {
                settings.set("rename_files_directory", directory);
                settings["rename_files_pattern"] = pattern;
            }, modify_rename_files, "Rename files"),
        OPTION_ARG1(rename-files, directory, {}, settings.set("rename_files_directory", directory)),
        OPTION_ARG0(rename-files, {}, settings.set("rename_files_directory", ".")),

        OPTION_ARG1(log-file, file, {}, settings["log_file"] = file),
        OPTION_ARG0(log-file, {}, IN_MODE(json, settings["action"] = "show-log")),
        OPTION_ARG1(config-file, file, settings["config_file"] = file, {}),
        
        OPTION_ARG0(help, {}, IN_MODE(json, settings["action"] = "help")),
        OPTION_ARG0(version, {}, settings["action"] = "version"),
        OPTION_ARG0(json, {}, IN_MODE(json,)),
        OPTION_ARG0(batch, {}, IN_MODE(batch,)),
        OPTION_ARG1(monitor, file, {}, IN_MODE(monitor, settings["monitor_file"] = file)),
        OPTION_ARG0(monitor, {}, IN_MODE(monitor,)),
        OPTION_ARG1(test, tests, {}, IN_MODE(json, settings["action"] = tests)),
        OPTION_ARG0(test, {}, IN_MODE(json, settings["action"] = "test")),
        OPTION_ARG0(uri, {}, IN_MODE(json, settings["action"] = "uri"))
    };
}

ostream& operator<<(ostream& stream, const option& option) {
    return stream << option.get_description();
}
