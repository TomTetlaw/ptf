#include "precompiled.h"

const char *game_dll_path = "game.dll";
const char *game_temp_dll_path = "game_temp.dll";

HANDLE find_notification;
FILETIME last_file_time;

bool is_recording = false;
bool is_playing_back = false;

FILE *recording_file = nullptr;

Array<void *> textures_to_free;

typedef void (*game_callback)(GAME_CALLBACK_PARAMS);
typedef void (*game_handle_key_callback)(GAME_HANDLE_KEY_PARAMS);

struct Game_Code {
    HMODULE game_dll_handle;
    game_callback init = nullptr;
    game_callback update = nullptr;
    game_callback render = nullptr;
    game_handle_key_callback handle_key = nullptr;
};

Game_Memory game_memory = {};
Game_Code game_code = {};
Game_Imports game_imports = {};

void win32_load_game_dll(Game_Code *code) {
    if(code->game_dll_handle) {
        FreeLibrary(code->game_dll_handle);
    }

    CopyFile(game_dll_path, game_temp_dll_path, FALSE);

    code->game_dll_handle = LoadLibrary(game_temp_dll_path);
    if(code->game_dll_handle) {
        code->init = (game_callback)GetProcAddress(code->game_dll_handle, "game_init");
        code->update = (game_callback)GetProcAddress(code->game_dll_handle, "game_update");
        code->render = (game_callback)GetProcAddress(code->game_dll_handle, "game_render");
        code->handle_key = (game_handle_key_callback)GetProcAddress(code->game_dll_handle, "game_handle_key");
    }
}

void win32_alloc_memory(Game_Memory *memory) {
 	void *base_address = (void *)(512*1024*1024);
	memory->size = 8 * 1024 * 1024;
	memory->data = VirtualAlloc(base_address, memory->size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

void win32_free_memory(Game_Memory *memory) {
    VirtualFree(memory->data, 0, MEM_RELEASE);
}

void win32_open_find_notification() {
    find_notification = FindFirstChangeNotification("w:\\build\\", FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
}

void win32_close_find_notification() {
    FindCloseChangeNotification(find_notification);
}

void win32_check_for_new_dll(Game_Code *code) {
    if (WaitForSingleObject(find_notification, 0) == WAIT_OBJECT_0) {
		HANDLE game_dll_handle = CreateFile(game_dll_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (game_dll_handle != INVALID_HANDLE_VALUE) {
			FILETIME game_dll_time;
			if (GetFileTime(game_dll_handle, NULL, NULL, &game_dll_time)) {
                CloseHandle(game_dll_handle);

				if (CompareFileTime(&last_file_time, &game_dll_time)) {
					win32_load_game_dll(code);
					last_file_time = game_dll_time;
				}
			}
		}

        FindNextChangeNotification(find_notification);
	}
}

Load_Texture_Result load_texture_from_file(const char *filename) {
    Load_Texture_Result result;

    SDL_Surface *surf = IMG_Load(filename);
	if (!surf) {
        printf("Failed to load texture %s: %s\n", filename, SDL_GetError());
		return result;
	}

	if (surf->format->format != SDL_PIXELFORMAT_RGBA8888) {
		SDL_PixelFormat format = { 0 };
		format.BitsPerPixel = 32;
		format.BytesPerPixel = 4;
		format.format = SDL_PIXELFORMAT_RGBA8888;
		format.Rshift = 0;
		format.Gshift = 8;
		format.Bshift = 16;
		format.Ashift = 24;
		format.Rmask = 0xff << format.Rshift;
		format.Gmask = 0xff << format.Gshift;
		format.Bmask = 0xff << format.Bshift;
		format.Amask = 0xff << format.Ashift;

		SDL_Surface *newSurf = SDL_ConvertSurface(surf, &format, 0);

		SDL_FreeSurface(surf);
		surf = newSurf;
	}

    int out_size = surf->w * surf->h * surf->format->BytesPerPixel;
    char *out = new char[out_size];
    memcpy_s(out, out_size, surf->pixels, out_size);

    textures_to_free.append(out);

    result.width = surf->w;
    result.height = surf->h;
    result.pixels = out;

    SDL_FreeSurface(surf);

    return result;
}

void write_initial_game_state() {
    fwrite(&game_memory.used, sizeof(int), 1, recording_file);
    fwrite(game_memory.data, 1, game_memory.used, recording_file);
}

void read_initial_game_state() {
    int used = 0;
    fread(&used, sizeof(int), 1, recording_file);
    game_memory.used = used;
    fread(game_memory.data, 1, used, recording_file);
}

void handle_event(SDL_Event &ev) {
    switch (ev.type) {
    case SDL_QUIT:
        sys.running = false;
        break;
    case SDL_KEYDOWN: {
        bool ctrl_pressed = (ev.key.keysym.mod & KMOD_CTRL) > 0;
        bool alt_pressed = (ev.key.keysym.mod & KMOD_ALT) > 0;
        bool shift_pressed = (ev.key.keysym.mod & KMOD_SHIFT) > 0;
        if (ev.key.keysym.scancode == SDL_SCANCODE_R && ctrl_pressed) {
            if (is_recording) {
                is_recording = false;
                is_playing_back = true;
                if (recording_file) {
                    fclose(recording_file);
                }
                fopen_s(&recording_file, "data/recording.rc", "rb");
                read_initial_game_state();
            }
            else {
                is_playing_back = false;
                is_recording = true;
                if (recording_file) {
                    fclose(recording_file);
                }
                fopen_s(&recording_file, "data/recording.rc", "wb");
                write_initial_game_state();
            }
        }
        else {
            (*game_code.handle_key)(&game_memory, &game_imports, ev.key.keysym.scancode, true);
        }
    } break;
    case SDL_KEYUP:
        (*game_code.handle_key)(&game_memory, &game_imports, ev.key.keysym.scancode, false);
        break;
    case SDL_SYSWMEVENT:
        /*if (ev.syswm.msg->msg.win.msg == WM_ACTIVATEAPP) {
            if (ev.syswm.msg->msg.win.wParam) {
                SDL_SetWindowOpacity(sys.window, 1.0f);
            }
            else {
                SetWindowPos(sys.hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                SDL_SetWindowOpacity(sys.window, 0.4f);
            }
        }*/ break;
    }
}

int main(int argc, char *argv[]) {
    system_init(argc, argv);

    win32_alloc_memory(&game_memory);
    assert(game_memory.data);

    win32_open_find_notification();

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, sys.window_size.x, sys.window_size.y, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glewInit();

    SDL_SysWMinfo info = {};
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(sys.window, &info);
    game_imports.dc = info.info.win.hdc;
    game_imports.glrc = wglGetCurrentContext();
    game_imports.load_texture_from_file = load_texture_from_file;
    sys.hwnd = info.info.win.window;

    win32_load_game_dll(&game_code);
    assert(game_code.init);
    assert(game_code.update);
    assert(game_code.render);
    assert(game_code.handle_key);

    (*game_code.init)(&game_memory, &game_imports);
    For(textures_to_free) {
        delete[] (*it);
    }

    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

	while (sys.running) {
		SDL_Event ev;

        while(SDL_PollEvent(&ev)) {
            if(is_recording && recording_file) {
                int event_here = 1;
                fwrite(&event_here, sizeof(int), 1, recording_file);

                int valid = 0;
                if(ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP) {
                    if(ev.key.keysym.scancode == SDL_SCANCODE_W) {
                        valid = 1;
                    } else if(ev.key.keysym.scancode == SDL_SCANCODE_A) {
                        valid = 1;
                    } else if(ev.key.keysym.scancode == SDL_SCANCODE_S) {
                        valid = 1;
                    } else if(ev.key.keysym.scancode == SDL_SCANCODE_D) {
                        valid = 1;
                    }
                }
                fwrite(&valid, sizeof(int), 1, recording_file);
                if (valid) {
                    fwrite(&ev, sizeof(SDL_Event), 1, recording_file);
                }
            }

            handle_event(ev);
        }

        if(is_recording && recording_file) {
            int event_here = 0;
            fwrite(&event_here, sizeof(int), 1, recording_file);
        }

        if(is_playing_back && recording_file) {
            int event_here = 0;
            while(true) {
                fread(&event_here, sizeof(int), 1, recording_file);
                if(!event_here) {
                    break;
                }

                int valid = 0;
                fread(&valid, sizeof(int), 1, recording_file);
                if(valid) {
                    fread(&ev, sizeof(SDL_Event), 1, recording_file);
                    handle_event(ev);
                }

                if(feof(recording_file)) {
                    fseek(recording_file, 0, SEEK_SET);
                    read_initial_game_state();
                }
            }
        }

		if (!sys.running) {
			break;
		}

        win32_check_for_new_dll(&game_code);

        game_imports.real_time = SDL_GetTicks();
        (*game_code.update)(&game_memory, &game_imports);

        (*game_code.render)(&game_memory, &game_imports);
        wglMakeCurrent(game_imports.dc, game_imports.glrc);
        
        SDL_GL_SwapWindow(sys.window);
	}

    if(recording_file) {
        fclose(recording_file);
    }

    system_shutdown();
    win32_close_find_notification();
    win32_free_memory(&game_memory);
    return 0;
}