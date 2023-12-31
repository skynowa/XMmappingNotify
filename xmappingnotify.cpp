/**
 * \file  xmappingnotify.c
 * \brief
 *
 * \see   https://stackoverflow.com/questions/35569562/how-to-catch-keyboard-layout-change-event-and-get-current-new-keyboard-layout-on
 */


#include <iostream>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/cursorfont.h>
//-------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	Display *display = ::XOpenDisplay(NULL);
	if (display == nullptr) {
		std::cerr << "cannot open display" << std::endl;
		return EXIT_FAILURE;
	}

	::XKeysymToKeycode(display, XK_F1);

	int xkbEventType {};
	::XkbQueryExtension(display, 0, &xkbEventType, 0, 0, 0);
	::XkbSelectEventDetails(display, XkbUseCoreKbd, XkbStateNotify, XkbAllStateComponentsMask,
		XkbGroupStateMask);

	::XSync(display, False);

	int i {};

	while (1) {
		XEvent event {};
		::XNextEvent(display, &event);

		if (event.type == xkbEventType) {
			auto *xkbEvent = (XkbEvent *)&event;
			if (xkbEvent->any.xkb_type == XkbStateNotify) {
				const int lang = xkbEvent->state.group;
				if (lang == 1) {
					std::cout << lang << std::endl;
				} else {
					std::cout << lang << std::endl;
				}

			   /**
				* Change cursor
				*
				* https://stackoverflow.com/questions/73833171/how-to-get-the-active-window-using-x11-xlib-c-api
				* https://github.com/UltimateHackingKeyboard/current-window-linux/blob/master/get-current-window.c
				*/

				// Get default root window of display
				Window rootWin = DefaultRootWindow(display);

				// Get the atom of the property
				Atom       property = ::XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
				const Bool isDelete {False};
				// https://docs.rs/x11/0.0.36/x11/xlib/constant.XA_WINDOW.html
				const Atom XA_WINDOW {33};

				// Return values
				Atom           type_return {};
				int            format_return {};
				unsigned long  nitems_return {};
				unsigned long  bytes_left {};
				unsigned char *data {};

				int iRv = ::XGetWindowProperty(
					display,
					rootWin,
					property,
					0,              // no offset
					1,              // one Window
					isDelete,
					XA_WINDOW,
					&type_return,   // should be XA_WINDOW
					&format_return, // should be 32
					&nitems_return, // should be 1 (zero if there is no such window)
					&bytes_left,    // should be 0 (i'm not sure but should be atomic read)
					&data           // should be non-null
				);

				if (iRv != Success) {
					std::cout << "iRv: " << iRv << std::endl;
				}

				if (nitems_return != 1) {
					std::cout << "nitems_return: " << nitems_return << std::endl;
				}

				Window activeWin = ((Window *) data)[0];

				++ i;

				const auto x11_cursor_font_id = (i % 2 == 0) ? XC_arrow : XC_dotbox;

				Cursor cursor = ::XCreateFontCursor(display, x11_cursor_font_id);

				::XDefineCursor(display, activeWin, cursor);

				::XFreeCursor(display, cursor);
				::XFree(data);
			}
		}
	}

	XCloseDisplay(display);

	return 0;
}
//-------------------------------------------------------------------------------------------------


#if 0

// https://github.com/Cougar/pwm/blob/master/cursor.c
void set_cursor(Window win, int cursor)
{
	XDefineCursor(wglobal.dpy, win, cursors[cursor]);
}

#endif
