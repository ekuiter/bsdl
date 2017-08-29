#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <numeric>

using namespace std;

class option {
    static vector<option> options;
    string key;
    vector<string> arg_names;
    int args, max_args = 2;
    typedef function<void (string, string)> handler_func;
    handler_func on_validate, on_process;

    string detailed_switch() {
        return string("--") + key;
    }

    string abbreviated_switch() {
        return string("-") + key[0];
    }

    template<typename T>
    bool matches(T is_arg) {
        return is_arg(detailed_switch(), args) || is_arg(abbreviated_switch(), args);
    }

    template<typename T>
    bool call_handler(handler_func handler, T next_arg) {
        if (args == 0)
            handler("", "");
        else if (args == 1) {
            string arg1 = next_arg();
            handler(arg1, "");
        } else if (args == 2) {
            string arg1 = next_arg();
            string arg2 = next_arg();
            handler(arg1, arg2);
        } else
            throw runtime_error("invalid number of arguments");
        return true;
    }
    
public:
    static void setup_options();

    static vector<option>& get_options() {
        return options;
    }
    
    option(const string& _key, const vector<string>& _arg_names, handler_func _on_validate, handler_func _on_process):
        key(_key), arg_names(_arg_names), args(_arg_names.size()), on_validate(_on_validate), on_process(_on_process) {
        if (args > max_args)
            throw runtime_error("maximum number of arguments exceeded");
    }

    template<typename T, typename U>
    bool validate(T is_arg, U next_arg) {
        return matches(is_arg) ? call_handler(on_validate, next_arg) : false;
    }

    template<typename T, typename U>
    bool process(T is_arg, U next_arg) {
        return matches(is_arg) ? call_handler(on_process, next_arg) : false;
    }

    string usage() {
        string usage = detailed_switch();
        for (auto arg_name : arg_names)
            usage += " " + arg_name;
        return usage;
    }
};
