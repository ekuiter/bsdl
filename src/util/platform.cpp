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
			{"À", "A"}, {"Á", "A"}, {"Â", "A"}, {"Ã", "A"}, {"Ä", "AE"}, {"Å", "A"}, {"Æ", "AE"}, {"Ç", "C"},
			{"È", "E"}, {"É", "E"}, {"Ê", "E"}, {"Ë", "E"}, {"Ì", "I"}, {"Í", "I"}, {"Î", "I"}, {"Ï", "I"},
			{"Ð", "D"}, {"Ñ", "N"}, {"Ò", "O"}, {"Ó", "O"}, {"Ô", "O"}, {"Õ", "O"}, {"Ö", "OE"}, {"×", "x"},
			{"Ø", "O"}, {"Ù", "U"}, {"Ú", "U"}, {"Û", "U"}, {"Ü", "UE"}, {"Ý", "Y"}, {"Þ", "?"}, {"ß", "ss"},
			{"à", "a"}, {"á", "a"}, {"â", "a"}, {"ã", "a"}, {"ä", "ae"}, {"å", "a"}, {"æ", "ae"}, {"ç", "c"},
			{"è", "e"}, {"é", "e"}, {"ê", "e"}, {"ë", "e"}, {"ì", "i"}, {"í", "I"}, {"î", "i"}, {"ï", "i"},
			{"ð", "d"}, {"ñ", "n"}, {"ò", "o"}, {"ó", "o"}, {"ô", "o"}, {"õ", "o"}, {"ö", "oe"}, {"÷", "?"},
			{"ø", "o"}, {"ù", "u"}, {"ú", "u"}, {"û", "u"}, {"ü", "ue"}, {"ý", "y"}, {"þ", "?"}, {"ÿ", "y"},
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

    void platform::copy(string str) {
#ifdef __MINGW32__
	@TODO
#elif defined (__APPLE__)
        boost::replace_all(str, "'", "'\\''");
	exec(string("printf %s '") + str + "' | pbcopy");
#else
	@TODO
#endif
    }
}
