#pragma once

#include <map>
#include <thread>
#include <boost/filesystem.hpp>

using namespace std;

namespace util {
    class platform {
    	static boost::filesystem::path executable_path_fallback(const string& argv0);
        static string utf8_to_latin1(const string& in_str);

    public:
        static string get_name();
    	static boost::filesystem::path executable_path(const string& argv0);
    	static void sleep(const chrono::seconds& cooldown);
    	static string encode(const string& str);
    	static string strip_chars(string str);        
    	static string exec(string cmd, bool has_output = false);
        static void browse(const string& url);
        
        struct exec_failed : exception {
            int exit_code;
            exec_failed(int _exit_code): exit_code(_exit_code) {}

            const char* what() const noexcept override {
                return (string("exec failed with exit code ") + to_string(exit_code)).c_str();
            }
        };
    };
}
