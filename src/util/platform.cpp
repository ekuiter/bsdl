#include "platform.hpp"
#include "../app.hpp"
#include "../curses/terminal.hpp"

#ifdef __MINGW32__
#include <Windows.h>
#elif defined (__APPLE__)
#include <mach-o/dyld.h>
#else
#endif

namespace util {
	string platform::get_name() {
#ifdef __MINGW32__
		return "Windows";
#elif defined (__APPLE__)
		return "Mac OS";
#else
                return "Linux";
#endif
	}

#ifdef __MINGW32__
	boost::filesystem::path platform::executable_path() {
		char buf[1024] = { 0 };
		DWORD ret = GetModuleFileNameA(NULL, buf, sizeof(buf));
		if (ret == 0 || ret == sizeof(buf))
                    @TODO
		return boost::filesystem::path(buf);
	}

#elif defined (__APPLE__)
	boost::filesystem::path platform::executable_path() {
		char buf[1024] = { 0 };
		uint32_t size = sizeof(buf);
		int ret = _NSGetExecutablePath(buf, &size);
		if (ret)
                    throw runtime_error("could not get executable path");
		boost::system::error_code ec;
		boost::filesystem::path p(
			boost::filesystem::canonical(buf, boost::filesystem::current_path(), ec));
		return p.make_preferred();
	}
#else
	boost::filesystem::path platform::executable_path() {
            @TODO
	}
#endif

	void platform::sleep(const chrono::seconds& cooldown) {
#ifdef __MINGW32__
		Sleep(chrono::duration_cast<chrono::milliseconds>(cooldown).count());
#else
		this_thread::sleep_for(cooldown);
#endif
	}

	string platform::utf8_to_latin1(const string& in_str) {
		const char* in = in_str.c_str();
		string out;
		if (in == NULL)
			return out;
		unsigned int codepoint;
		while (*in) {
			unsigned char ch = static_cast<unsigned char>(*in);
			codepoint = ch <= 0x7f ? ch :
				ch <= 0xbf ? (codepoint << 6) | (ch & 0x3f) :
				ch <= 0xdf ? ch & 0x1f :
				ch <= 0xef ? ch & 0x0f : ch & 0x07;
			in++;
			if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff) && codepoint <= 255)
				out.append(1, static_cast<char>(codepoint));
		}
		return out;
	}

	string platform::encode(const string& str) {
#ifdef __APPLE__
                return str;
#else
		return utf8_to_latin1(str);
#endif
	}

	string platform::strip_chars(string str) {
#ifdef __APPLE__
		return str;
#else
		vector<pair<string, string>> special_characters = {
			{"�", "A"}, {"�", "A"}, {"�", "A"}, {"�", "A"}, {"�", "AE"}, {"�", "A"}, {"�", "AE"}, {"�", "C"},
			{"�", "E"}, {"�", "E"}, {"�", "E"}, {"�", "E"}, {"�", "I"}, {"�", "I"}, {"�", "I"}, {"�", "I"},
			{"�", "D"}, {"�", "N"}, {"�", "O"}, {"�", "O"}, {"�", "O"}, {"�", "O"}, {"�", "OE"}, {"�", "x"},
			{"�", "O"}, {"�", "U"}, {"�", "U"}, {"�", "U"}, {"�", "UE"}, {"�", "Y"}, {"�", "?"}, {"�", "ss"},
			{"�", "a"}, {"�", "a"}, {"�", "a"}, {"�", "a"}, {"�", "ae"}, {"�", "a"}, {"�", "ae"}, {"�", "c"},
			{"�", "e"}, {"�", "e"}, {"�", "e"}, {"�", "e"}, {"�", "i"}, {"�", "I"}, {"�", "i"}, {"�", "i"},
			{"�", "d"}, {"�", "n"}, {"�", "o"}, {"�", "o"}, {"�", "o"}, {"�", "o"}, {"�", "oe"}, {"�", "?"},
			{"�", "o"}, {"�", "u"}, {"�", "u"}, {"�", "u"}, {"�", "ue"}, {"�", "y"}, {"�", "?"}, {"�", "y"},
		};
		int pos;
		for (auto pair : special_characters)
			while ((pos = str.find(pair.first)) != string::npos)
				str.replace(pos, pair.first.length(), pair.second);
		for (int i = 0; i < str.length(); i++)
			if (static_cast<unsigned char>(str[i]) > 0x7f)
				str.replace(i, 1, "?");
		return str;
#endif
	}
    
	string platform::exec(string cmd, bool has_output) {
#ifdef __MINGW32__
		string ignore_stderr = " 2>nul";
		auto open_pipe = _popen;
		auto close_pipe = _pclose;
		int div = 1;
#else
		string ignore_stderr = " 2>/dev/null";
		auto open_pipe = popen;
		auto close_pipe = pclose;
		int div = 256;
#endif
        char buffer[128];
        string result = "";
		cmd += ignore_stderr;
        FILE* pipe = open_pipe(cmd.c_str(), "r");
        if (!pipe)
            throw runtime_error(string("popen failed for command '") + cmd + "'");
        app::instance().set_title("", true, cmd.substr(0, cmd.find(' ')));
        curses::point p;
        cout << curses::stream::get(p);
        if (has_output && p.x)
            cout << endl;
        try {
            while (!feof(pipe)) {
                if (fgets(buffer, 128, pipe) != NULL)
                    result += buffer;
            }
        } catch (...) {
            app::instance().set_title("", true, "");
			close_pipe(pipe);
            throw;
        }
        app::instance().set_title("", true, "");
        int exit_code = close_pipe(pipe) / div;
        if (exit_code)
            throw exec_failed(exit_code);
        return result;
	}

    void platform::browse(const string& url) {
#ifdef __MINGW32__
	@TODO
#elif defined (__APPLE__)
	exec(string("open ") + url);
#else
	@TODO
#endif
    }

    static string escape(string str, bool double_quotes = false) {
        boost::replace_all(str, "'", "'\\''");
        if (double_quotes)
            boost::replace_all(str, "\"", "\\\"");
        return str;
    }

    void platform::copy(string str) {
#ifdef __MINGW32__
	@TODO
#elif defined (__APPLE__)
        exec(string("printf %s '") + escape(str) + "' | pbcopy");
#else
	@TODO
#endif
    }

    void platform::edit(const string& file) {
#ifdef __MINGW32__
	@TODO
#elif defined (__APPLE__)
        exec(string("open '") + escape(file) + "'");
#else
	@TODO
#endif  
    }

    void platform::notify(const string& title, const string& subtitle, const string& msg) {
#ifdef __MINGW32__
	@TODO
#elif defined (__APPLE__)
        exec(string("osascript -e 'display notification \"") + escape(msg, true) +
             "\" with title \"" + escape(title, true) + "\" subtitle \"" + escape(subtitle, true) + "\"'");
#else
	@TODO
#endif
    }

    bool platform::print_version(const string& formula) {
        try {
            string version = exec(formula + " --version");
            boost::trim(version);
            cout << formula << " is installed (" << version << ")." << endl;
            return true;
        } catch (exec_failed) {
            return false;
        }
    }

    void platform::brew_install(const string& formula) {
#ifdef __MINGW32__
            throw runtime_error("homebrew is only available on macOS");
#elif defined(__APPLE__)
            try {
                cout << "Installing " << formula << " ..." << endl;
                exec(string("brew install ") + formula);
            }
            catch (exec_failed) {
                try {
                    cout << "Installing Homebrew ..." << endl;
                    exec("/usr/bin/ruby -e \"$(curl -fsSL "
                                         "https://raw.githubusercontent.com/Homebrew/install/master/install)\"");
                    cout << "Homebrew has been installed." << endl;
                    try {
                        cout << "Installing " << formula << " ..." << endl;
                        exec(string("brew install ") + formula);
                    }
                    catch (exec_failed) {
                        throw runtime_error(formula + " could not be installed.\nTry again or install it manually.");
                    }
                }
                catch (exec_failed) {
                    throw runtime_error("Homebrew could not be installed.\nTry again or install it manually.");
                }
            }
            string version = exec(formula + " --version");
            boost::trim(version);
            cout << formula << " is installed (" << version << ")." << endl;
#else
            throw runtime_error("homebrew is only available on macOS");
#endif
    }

    bool platform::can_get_captcha_solution() {
#ifdef __MINGW32__
	return false;
#elif defined (__APPLE__)
        return true;
#else
        return false;
#endif
    }
    
    string platform::get_captcha_solution(const http::request& request, const providers::provider& provider) {
#ifdef __MINGW32__
	@TODO
#elif defined (__APPLE__)
        string solution;
        try {
            solution = exec(string(R"TEXT(osascript -e '
tell application "System Events"
	if not (exists application process "Google Chrome") then
		return "<chrome>"
	end if
end tell

tell application "Google Chrome"
	repeat with theTab in every window'\''s tab
		if theTab'\''s URL contains ")TEXT") + provider.get_name() + R"TEXT(" then
			set theProviderUrl to theTab'\''s URL
			go back theTab
			if theTab'\''s URL is ")TEXT" + request.get_url() + R"TEXT(" then
				close theTab
				return theProviderUrl
			else
				go forward theTab
			end if
		end if
	end repeat
end tell

return ""
')TEXT");
        } catch (exec_failed) {
            cerr << "Error while solving the captcha." << endl;
            return "";
        }
        boost::trim(solution);
        if (solution == "<chrome>") {
            cerr << "Solving the captcha requires Google Chrome!" << endl;
            solution = "";
        }
        return solution;
#else
	@TODO
#endif
    }
}
