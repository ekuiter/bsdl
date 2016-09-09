#include "settings.hpp"
#include "app.hpp"
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::filesystem;

static path executable_path_fallback(const string& argv0) {
    if (argv0 == "")
        return "";
    boost::system::error_code ec;
    boost::filesystem::path p(
            boost::filesystem::canonical(
                    argv0, boost::filesystem::current_path(), ec));
    return p.make_preferred();
}

#ifdef __MINGW32__
#include <Windows.h>

static path executable_path(const string& argv0) {
    char buf[1024] = {0};
    DWORD ret = GetModuleFileNameA(NULL, buf, sizeof(buf));
    if (ret == 0 || ret == sizeof(buf))
        return executable_path_fallback(argv0);
    return path(buf);
}

#else
#include <mach-o/dyld.h>

static path executable_path(const string& argv0) {
    char buf[1024] = {0};
    uint32_t size = sizeof(buf);
    int ret = _NSGetExecutablePath(buf, &size);
    if (ret)
        return executable_path_fallback(argv0);
    boost::system::error_code ec;
    boost::filesystem::path p(
        boost::filesystem::canonical(buf, boost::filesystem::current_path(), ec));
    return p.make_preferred();
}

#endif

ostream& operator<<(ostream& stream, settings& _settings) {
    stream << "download_selection = " << _settings.get_download_selection() << endl;
    stream << "preferred_providers = ";
    int i = 0;
    for (auto provider : _settings.get_preferred_providers()) {
        stream << *provider;
        if (++i < _settings.get_preferred_providers().size())
            stream << ", ";
    }
    stream << endl;
    for (auto pair : _settings)
        stream << pair.first << " = " << pair.second << endl;
    return stream;
}

void settings::read(const vector<string>& args) {
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
        (*this)["config_file"] = (executable_path(args[0]).remove_filename() /= "bsdl.cfg").string();

    string config_file_name = (*this)["config_file"];
    if (!exists(config_file_name))
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

    for (i = first_arg; i < args.size(); i++) {
        if (is_arg("--download", 2) || is_arg("-d", 2))
            _download_selection.add(new bs::download_selector::episode(stoi(next_arg()), stoi(next_arg())));
        else if (is_arg("--download", 1) || is_arg("-d", 1))
            _download_selection.add(new bs::download_selector::season(stoi(next_arg())));
        else if (is_arg("--download") || is_arg("-d"))
            _download_selection.add(new bs::download_selector::series);
        else if (is_arg("--provider", 1) || is_arg("-p", 1))
            preferred_providers.push_back(&provider::instance(next_arg(), true));
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

    if (preferred_providers.size() == 0) {
        vector<string> providers;
        boost::split(providers, get("providers"), boost::is_any_of(","));
        for (auto& provider : providers) {
            boost::trim(provider);
            preferred_providers.push_back(&provider::instance(provider, true));
        }
    }

    if (!is_set("output_files_directory"))
        (*this)["output_files_directory"] = ".";
}