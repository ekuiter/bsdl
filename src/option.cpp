#include "option.hpp"
#include "settings.hpp"
#include "app.hpp"
#include "curses/terminal.hpp"
#include "curses/widgets.hpp"

using namespace curses;

#define HANDLERS(on_validate, on_process, params)                       \
    [&settings] params { on_validate; }, [&settings] params { on_process; }
#define DETAILS(details) []() { return details; }
#define MODIFIABLE_OPTION_ARG0(key, on_validate, on_process, on_modify, description, details) \
    { #key, {}, HANDLERS(on_validate, on_process, (string, string, string)), on_modify, description, DETAILS(details) }
#define MODIFIABLE_OPTION_ARG1(key, arg1, on_validate, on_process, on_modify, description, details) \
    { #key, { #arg1 }, HANDLERS(on_validate, on_process, (string arg1, string, string)), on_modify, description, DETAILS(details) }
#define MODIFIABLE_OPTION_ARG2(key, arg1, arg2, on_validate, on_process, on_modify, description, details) \
    { #key, { #arg1, #arg2 }, HANDLERS(on_validate, on_process, (string arg1, string arg2, string)), on_modify, description, DETAILS(details) }
#define MODIFIABLE_OPTION_ARG3(key, arg1, arg2, arg3, on_validate, on_process, on_modify, description, details) \
    { #key, { #arg1, #arg2, #arg3 }, HANDLERS(on_validate, on_process, (string arg1, string arg2, string arg3)), on_modify, description, DETAILS(details) }
#define OPTION_ARG0(key, on_validate, on_process, details) MODIFIABLE_OPTION_ARG0(key, on_validate, on_process, nullptr, "", details)
#define OPTION_ARG1(key, arg1, on_validate, on_process, details) MODIFIABLE_OPTION_ARG1(key, arg1, on_validate, on_process, nullptr, "", details)
#define OPTION_ARG2(key, arg1, arg2, on_validate, on_process, details) MODIFIABLE_OPTION_ARG2(key, arg1, arg2, on_validate, on_process, nullptr, "", details)
#define OPTION_ARG3(key, arg1, arg2, arg3, on_validate, on_process, details) MODIFIABLE_OPTION_ARG3(key, arg1, arg2, arg3, on_validate, on_process, nullptr, "", details)

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

#define SET_RENAME_FILES(directory, pattern) {                  \
        settings.set("rename_files_directory", directory);      \
        settings["rename_files_pattern"] = pattern;             \
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

    auto modify_override_title = [&settings](){
        modify_setting("override_title", "\nEnter overriding title (if any):", false);
        if (!settings.is_set("override_title"))
            cout << "Will not override series title." << endl;
        else
            cout << "Will override series title with " << stream::colored(settings["override_title"]) << "." << endl;
    };

    auto modify_aggregators = MODIFY_PREFERRED(aggregators, aggregators::aggregator::set_preferred_aggregators, aggregators::exception);
    auto modify_providers = MODIFY_PREFERRED(providers, providers::provider::set_preferred_providers, providers::provider::exception);
    auto modify_subtitles = MODIFY_PREFERRED(subtitles, aggregators::subtitle::set_preferred_subtitles, aggregators::exception);

    options = { // unused abbreviations: f, g, i, k, x, y, z
        OPTION_ARG2(download, season, episode, {},
                    DOWNLOAD_OPTION(episode,
                                    download_selection.add(new aggregators::download_selector::episode(stoi(season), stoi(episode))),
                                    download_selection.add(new aggregators::download_selector::episode(stoi(season), i))),
                    "Selects an episode (e.g. -d 1 1) or a range of episodes (e.g. -d 1 1-5) for download"),
        OPTION_ARG1(download, season, {},
                    DOWNLOAD_OPTION(season, {
                            if (season == "latest")
                                download_selection.add(new aggregators::download_selector::latest_episode);
                            else if (season == "new")
                                download_selection.add(new aggregators::download_selector::new_episodes);
                            else
                                download_selection.add(new aggregators::download_selector::season(stoi(season)));
                        }, download_selection.add(new aggregators::download_selector::season(i))),
                    "Selects a season (e.g. -d 1) or a range of seasons (e.g. -d 1-5) for download"),
        OPTION_ARG0(download, {}, settings._download_selection.add(new aggregators::download_selector::series), "Selects an entire series for download"),
        
        MODIFIABLE_OPTION_ARG1(aggregator, aggregator, {}, settings.preferred_aggregators.push_back(&aggregators::aggregator::instance(aggregator)),
                               modify_aggregators, "Preferred aggregators",
                               string("Adds an aggregator (e.g. -a ") + settings::get("aggregator_bs") + ") to the list of preferred aggregators"),
        MODIFIABLE_OPTION_ARG1(provider, provider, {}, settings.preferred_providers.push_back(&providers::provider::instance(provider, true)),
                               modify_providers, "Preferred providers",
                               string("Adds a provider (e.g. -p ") + settings::get("provider_1") + ") to the list of preferred providers"),
        MODIFIABLE_OPTION_ARG1(subtitle, subtitle, {}, settings.preferred_subtitles.push_back(&aggregators::subtitle::instance(subtitle)),
                               modify_subtitles, "Preferred subtitles",
                               string("Adds a subtitle (e.g. -s ") + aggregators::subtitle::allowed_subtitles()[0] + ") to the list of preferred subtitles"),
        
        MODIFIABLE_OPTION_ARG1(output-files, directory, {}, settings.set("output_files_directory", directory), modify_output_files, "Output directory",
                               "Specifies the directory where to save downloaded episodes"),
        MODIFIABLE_OPTION_ARG2(rename-files, directory, pattern, {}, SET_RENAME_FILES(directory, pattern), modify_rename_files, "Rename files",
                               "Renames episodes in a directory matching a pattern (e.g. -r . default)"),
        OPTION_ARG1(rename-files, directory, {}, SET_RENAME_FILES(directory, ""), "Renames episodes in a directory matching the default pattern (e.g. -r .)"),
        OPTION_ARG0(rename-files, {}, SET_RENAME_FILES(".", ""), "Renames episodes in the working directory matching the default pattern"),
        MODIFIABLE_OPTION_ARG1(with-title, title, {}, settings["override_title"] = title, modify_override_title, "Override title", "Overrides the default series title"),

        OPTION_ARG1(log-file, file, {}, settings["log_file"] = file, string("Specifies a log file (default is ") + settings::instance().default_log_file() + ")"),
        OPTION_ARG0(log-file, {}, IN_MODE(json, settings["action"] = "show-log"), "Shows the log file"),
        OPTION_ARG1(config-file, file, settings["config_file"] = file, {}, string("Specifies a config file (default is ") + settings::instance().default_config_file() + ")"),
        OPTION_ARG0(edit-file, {}, IN_MODE(json, settings["action"] = "edit-file"), "Edits the config (e.g. -e) or monitor (e.g. -e -m) file"),
        
        OPTION_ARG0(help, {}, IN_MODE(json, settings["action"] = "help"), "Shows usage information"),
        OPTION_ARG0(version, {}, settings["action"] = "version", "Shows version information"),
        OPTION_ARG0(quiet, {}, settings["log_file"] = "", "Disables the log file"),
        OPTION_ARG0(json, {}, IN_MODE(json,), "Shows JSON data for selected episodes"),
        OPTION_ARG0(batch, {}, IN_MODE(batch,), "Downloads episodes using given JSON data"),
        OPTION_ARG1(monitor, file, {}, IN_MODE(monitor, settings["monitor_file"] = file), "Downloads monitored series using a monitor file"),
        OPTION_ARG0(monitor, {}, IN_MODE(monitor,), string("Downloads monitored series using the default monitor file at ") + settings::instance().default_monitor_file()),
        OPTION_ARG1(notifier, file, {}, IN_MODE(notifier, settings["monitor_file"] = file), "Notifies about monitored series using a monitor file"),
        OPTION_ARG0(notifier, {}, IN_MODE(notifier,), string("Notifies about monitored series using the default monitor file at ") + settings::instance().default_monitor_file()),
        OPTION_ARG1(test, tests, {}, IN_MODE(json, settings["action"] = tests),
                    "Runs a test or test suite (e.g. -t '*'), only quick tests (e.g. -t quick) or lists tests (e.g. -t list)"),
        OPTION_ARG0(test, {}, IN_MODE(json, settings["action"] = "test"), "Runs all tests"),
        OPTION_ARG0(uri, {}, IN_MODE(json, settings["action"] = "uri"), "Shows all URIs for a search term")
    };
}

ostream& operator<<(ostream& stream, const option& option) {
    return stream << option.get_description();
}
