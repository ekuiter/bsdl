#include "option.hpp"
#include "settings.hpp"

#define HANDLERS(on_validate, on_process, params)             [&settings] params { on_validate; }, [&settings] params { on_process; }
#define OPTION_ARG0(key, on_validate, on_process)             { #key, {}, HANDLERS(on_validate, on_process, (string, string)) }
#define OPTION_ARG1(key, arg1, on_validate, on_process)       { #key, { #arg1 }, HANDLERS(on_validate, on_process, (string arg1, string)) }
#define OPTION_ARG2(key, arg1, arg2, on_validate, on_process) { #key, { #arg1, #arg2 }, HANDLERS(on_validate, on_process, (string arg1, string arg2)) }

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

vector<option> option::options;

void option::setup_options() {
    settings& settings = settings::instance();
    
    options = {
        OPTION_ARG2(download, season, episode, {},
                    DOWNLOAD_OPTION(episode,
                                    download_selection.add(new aggregators::download_selector::episode(stoi(season), stoi(episode))),
                                    download_selection.add(new aggregators::download_selector::episode(stoi(season), i)))),
        OPTION_ARG1(download, season, {},
                    DOWNLOAD_OPTION(season, {
                            if (season == "latest")
                                download_selection.add(new aggregators::download_selector::latest_episode);
                            else
                                download_selection.add(new aggregators::download_selector::season(stoi(season)));
                        }, download_selection.add(new aggregators::download_selector::season(i)))),
        OPTION_ARG0(download, {}, settings._download_selection.add(new aggregators::download_selector::series)),
        
        OPTION_ARG1(aggregator, aggregator, {}, settings.preferred_aggregators.push_back(&aggregators::aggregator::instance(aggregator))),
        OPTION_ARG1(provider, provider, {}, settings.preferred_providers.push_back(&providers::provider::instance(provider, true))),
        OPTION_ARG1(subtitle, subtitle, {}, settings.preferred_subtitles.push_back(&aggregators::subtitle::instance(subtitle))),
        
        OPTION_ARG1(output-files, directory, {}, settings["output_files_directory"] = directory),
        OPTION_ARG2(rename-files, directory, pattern, {}, {
                settings["rename_files_directory"] = directory;
                settings["rename_files_pattern"] = pattern;
            }),
        OPTION_ARG1(rename-files, directory, {}, settings["rename_files_directory"] = directory),
        OPTION_ARG0(rename-files, {}, settings["rename_files_directory"] = "."),
        
        OPTION_ARG1(log-file, file, {}, settings["log_file"] = file),
        OPTION_ARG1(config-file, file, settings["config_file"] = file, {}),
        
        OPTION_ARG0(help, {}, settings["action"] = "help"),
        OPTION_ARG0(version, {}, settings["action"] = "version"),
        OPTION_ARG1(test, tests, {}, settings["action"] = tests),
        OPTION_ARG0(test, {}, settings["action"] = "test")
    };
}
