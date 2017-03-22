#ifndef __PTF_H__
#define __PTF_H__

#ifdef assert
#undef assert
#endif

#define assert(x) {if(!(x)) *(int *)0 = 0; }

#define GAME_CALLBACK_PARAMS Game_Memory *memory, Game_Imports *imports
#define GAME_HANDLE_KEY_PARAMS GAME_CALLBACK_PARAMS, int key, bool down
#define GAME_CALLBACK(name) extern "C" __declspec(dllexport) void name (GAME_CALLBACK_PARAMS)
#define GAME_HANDLE_KEY_CALLBACK(name) extern "C" __declspec(dllexport) void name (GAME_HANDLE_KEY_PARAMS)

struct Game_Memory {
    void *data = nullptr;
    int size = 0;
    int used = 0;
    bool is_initialized = false;
};

struct Memory_Arena {
    void *address = nullptr;
    int size = 0;
    int used = 0;
};

typedef void *(*memory_alloc_func(Game_Memory *memory, int size));
typedef void (*arena_create_func(Game_Memory *memory, Memory_Arena *arena, int size));
typedef void *(*arena_alloc_func(Memory_Arena *arena, int size));

struct Game_Imports {
    memory_alloc_func *memory_alloc = nullptr;
    arena_create_func *arena_create = nullptr;
    arena_alloc_func *arena_alloc = nullptr;
    void (*test)(float x, float y) = nullptr;
    const unsigned char *keyboard_state = nullptr;
    int real_time = 0;
    HDC dc = nullptr;
    HGLRC glrc = nullptr;
};

#endif
