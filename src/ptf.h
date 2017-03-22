#ifndef __PTF_H__
#define __PTF_H__

struct Game_Memory {
    void *data = nullptr;
    int size = 0;
    int used = 0;
    bool is_initialized = false;
};

struct Game_Imports {
    int real_time = 0;
    HDC dc = nullptr;
    HGLRC glrc = nullptr;
};

#endif
