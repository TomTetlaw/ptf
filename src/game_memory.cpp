#include "precompiled.h"
#include "game.h"

void *memory_alloc(Game_Memory *memory, int size) {
    assert(memory->used + size < memory->size);
    void *location = (char *)memory->data + memory->used;
    memory->used += size;
    return location;
}

void arena_create(Game_Memory *memory, Memory_Arena *arena, int size) {
    assert(memory->used + size < memory->size);
    arena->address = (void *)((char *)memory->data + memory->used);
    memory->used += size;
    arena->size = size;
}

void *arena_alloc(Memory_Arena *arena, int size) {
    assert(arena->used + size < arena->size);
    void *location = (char *)arena->address + arena->used;
    arena->used += size;
    return location;
}