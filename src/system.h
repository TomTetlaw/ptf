#ifndef __SYSTEM_H__
#define __SYSTEM_H__

struct System {
	HWND hwnd;
	SDL_Window *window = nullptr;
	SDL_GLContext context = nullptr;

    bool running = true;

	Vector2 window_size;
};

extern System sys;

void system_init(int argc, char *argv[]);
void system_shutdown();
void system_error(const char *text, ...);

#endif