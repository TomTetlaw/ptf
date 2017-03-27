#ifndef __GAME_H__
#define __GAME_H__

extern Game_Imports *imports;
extern Game_Memory *memory;

struct Texture {
    int width = 0;
    int height = 0;
    unsigned int api_object = 0;
};

void load_texture(const char *filename, Texture *texture);

void immediate_render_texture(Texture *texture, Vector2 position, Vector4 colour);
void immediate_render_line(Vector2 a, Vector2 b, Vector4 colour);
void immediate_render_box(Box box, Vector4 colour, bool fill);

struct Memory_Arena {
    void *address = nullptr;
    int size = 0;
    int used = 0;
};

void *memory_alloc(Game_Memory *memory, int size);
void arena_create(Game_Memory *memory, Memory_Arena *arena, int size);
void *arena_alloc(Memory_Arena *arena, int size);

#endif