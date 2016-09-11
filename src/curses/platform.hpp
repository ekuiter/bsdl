#pragma once

#include <curses.h>

namespace curses {
    class platform {
	public:
		static int curs_set(int visibility) {
#ifdef __MINGW32__
			return visibility ? 0 : ::curs_set(0);
#else
			return ::curs_set(visibility);
#endif
		}

		static int getmouse(MEVENT* event) {
#ifdef __MINGW32__
			nc_getmouse(event);
#else
			getmouse(event);
#endif
		}

		static int set_escdelay(int size) {
#ifndef __MINGW32__
			TODO //return ::set_escdelay(size); //ESCDELAY = size;
#else
			return 0;
#endif
		}

		static int get_newline() {
#ifdef __MINGW32__
			return '\r';
#else
			return '\n';
#endif
		}

		static bool is_backspace(int ch) {
#ifdef __MINGW32__
			return ch == 0x8;
#else
			return ch == 0x7f;
#endif
		}
	};
}
