#ifndef __PTF_H__
#define __PTF_H__

struct Game_Memory {
    void *data = nullptr;
    int size = 0;
    int used = 0;
    bool is_initialized = false;
};

struct Load_Texture_Result {
    int width = 0;
    int height = 0;
    void *pixels = nullptr;
};

typedef Load_Texture_Result (*load_texture_from_file_callback)(const char *filename);

struct Game_Imports {
    int real_time = 0;
    HDC dc = nullptr;
    HGLRC glrc = nullptr;

    load_texture_from_file_callback load_texture_from_file;
};

#endif
