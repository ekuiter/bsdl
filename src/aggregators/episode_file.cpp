#include "episode_file.hpp"
#include "../app.hpp"
#include "../util/addressed.hpp"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace aggregators {
    episode::file::file(series& _series, const string& _old_file_name, string pattern_str):
            old_file_name(_old_file_name), _episode(nullptr) {
        if (pattern_str == "" || pattern_str == "default")
            pattern_str = " (\\d+)\\.(\\d+)";
        if (pattern_str == "SxxExx")
            pattern_str = " S(\\d+)E(\\d+)";
        regex pattern(pattern_str);
        smatch results;

        if (!regex_search(old_file_name, results, pattern))
            throw exception(string("could not recognize \"") + old_file_name + "\"");

        int season_number = stoi(results[1]), number = stoi(results[2]);
        _episode = (*_series[season_number])[number];
    }
    
    bool episode::file::similar_file_exists(const string& file_name, const string& series_title) {
        directory_iterator end_it;
        for (directory_iterator it(settings::instance().output_files_directory(series_title)); it != end_it; it++)
            if (is_regular_file(it->status()) && it->path().filename().string().find(file_name) != string::npos)
                return true;
        return false;
    }

    static vector<string> rename_files(series& _series, string directory_name, string pattern_str, bool do_rename) {
        vector<string> changes;
        directory_iterator end_it;
        for (directory_iterator it(directory_name); it != end_it; it++)
            if (is_regular_file(it->status())) {
                try {
                    string old_file_name = it->path().filename().string();
                    unique_ptr<episode::file> file = _series.get_file(old_file_name, pattern_str);
                    string new_file_name = file->get_file_name() + it->path().extension().string();
                    changes.push_back(old_file_name + " => " + new_file_name);

                    if (do_rename) {
                        cout << "Renaming " << stream::colored(old_file_name) <<
                            " to " << stream::colored(new_file_name) << "." << endl;
                        path new_path = it->path();
                        rename(it->path(), new_path.remove_filename() /= new_file_name);
                    }
                } catch (aggregators::exception& e) {}
            }
        return changes;
    }

    void episode::file::rename_files(series& _series, string directory_name, string pattern_str) {
        if (!exists(directory_name))
            throw exception("directory \"" + directory_name + "\" does not exist");

        vector<string> changes = aggregators::rename_files(_series, directory_name, pattern_str, false);
        if (app::instance().is_testing())
            return;
        
        if (changes.size() == 0)
            cerr << "No renameable files could be found in \"" << directory_name << "\"." << endl;
        else {
            window::framed menu_window(app::instance().get_centered_bounds());
            auto changes_addressed = changes | util::addressed<string>();
            menu_dialog::run(menu_window, "These files will be renamed:",
                    changes_addressed, *changes_addressed.begin(), "Rename all");
            aggregators::rename_files(_series, directory_name, pattern_str, true);
        }
    }
    
    int episode::file::get_number_of_seasons() {
        if (!app::instance().get_current_series())
            throw exception("there is no current series");
        return app::instance().get_current_series()->season_number();
    }
}
