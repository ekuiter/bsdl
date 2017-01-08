#include "platform.hpp"
#include "../app.hpp"

#ifdef __MINGW32__
#include <Windows.h>
#else
#include <mach-o/dyld.h>
#endif

namespace util {
	string platform::get_name() {
#ifdef __MINGW32__
		return "Windows";
#else
		return "Mac OS";
#endif
	}

	boost::filesystem::path platform::executable_path_fallback(const string& argv0) {
		if (argv0 == "")
			return "";
		boost::system::error_code ec;
		boost::filesystem::path p(
			boost::filesystem::canonical(
				argv0, boost::filesystem::current_path(), ec));
		return p.make_preferred();
	}

#ifdef __MINGW32__
	boost::filesystem::path platform::executable_path(const string& argv0) {
		char buf[1024] = { 0 };
		DWORD ret = GetModuleFileNameA(NULL, buf, sizeof(buf));
		if (ret == 0 || ret == sizeof(buf))
			return executable_path_fallback(argv0);
		return boost::filesystem::path(buf);
	}

#else
	boost::filesystem::path platform::executable_path(const string& argv0) {
		char buf[1024] = { 0 };
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
#ifdef __MINGW32__
		return utf8_to_latin1(str);
#else
		return str;
#endif
	}

	string platform::strip_chars(string str) {
#ifdef __MINGW32__
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
#else
		return str;
#endif
	}
    
	string platform::exec(string cmd) {
#ifdef __MINGW32__
		@TODO
#else
        char buffer[128];
        string result = "";
        cmd += " 2>/dev/null";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe)
            throw runtime_error(string("popen failed for command '") + cmd + "'");
        app::instance().set_title("", true, cmd.substr(0, cmd.find(' ')));
        try {
            while (!feof(pipe)) {
                if (fgets(buffer, 128, pipe) != NULL)
                    result += buffer;
            }
        } catch (...) {
            app::instance().set_title("", true, "");
            pclose(pipe);
            throw;
        }
        app::instance().set_title("", true, "");
        int exit_code = pclose(pipe) / 256;
        if (exit_code)
            throw exec_failed(exit_code);
        return result;
#endif
	}
}
