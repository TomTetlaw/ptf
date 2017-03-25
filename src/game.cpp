#include "precompiled.h"
#include "game.h"

Game_Imports *imports = nullptr;
Game_Memory *memory = nullptr;

struct Game_Allocator {
    Memory_Arena *arena = nullptr;

	void *alloc(int n) { 
        return arena_alloc(arena, n);
	}

	void dealloc(void *ptr) {
	}
};

#define MAX_ENTITIES 256

enum Entity_Type {
    ENTITY_BASE,
};

struct Entity {
    int parity = 0;
    int index = 0;
    Vector2 position;
    Texture texture;
    Entity_Type type = ENTITY_BASE;
};

struct Game_State {
    Entity *player = nullptr;
    bool move_up = false;
    bool move_down = false;
    bool move_left = false;
    bool move_right = false;

    float dt = 0.0f;
    float real_time = 0.0f;

    Memory_Arena entities_arena;
    Game_Allocator entities_allocator;
    Array<Entity, Game_Allocator> entities;
    int next_parity = 0;
};

Entity *create_entity(Game_State *state) {
    int index = 0;

    For(state->entities) {
        if(!(*it).index) {
            (*it).index = it_index;
            return &(*it);
        }
    }

    if(state->entities.num + 1 >= MAX_ENTITIES) {
        return nullptr;
    }

    Entity *entity = &state->entities[state->entities.num];
    entity->index = state->entities.num;
    state->entities.num++;

    return entity;
}

void setup_entity_player(Entity *entity) {
    load_texture("data/textures/player.png", &entity->texture);
}

GAME_CALLBACK(game_init) {
    ::imports = imports;
    ::memory = memory;
    memory->used = sizeof(Game_State);

    Game_State *state = (Game_State *)memory->data;

    arena_create(memory, &state->entities_arena, sizeof(Entity) * MAX_ENTITIES);
    state->entities_allocator.arena = &state->entities_arena;
    state->entities.allocator = state->entities_allocator;
    state->entities.ensure_size(MAX_ENTITIES);

    wglMakeCurrent(imports->dc, imports->glrc);
    glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state->player = create_entity(state);
    setup_entity_player(state->player);
}

GAME_HANDLE_KEY_CALLBACK(game_handle_key) {
    Game_State *state = (Game_State *)memory->data;
    
    if(key == SDL_SCANCODE_W) {
        state->move_up = down;
    }
    if(key == SDL_SCANCODE_S) {
        state->move_down = down;
    }
    if(key == SDL_SCANCODE_A) {
        state->move_left = down;
    }
    if(key == SDL_SCANCODE_D) {
        state->move_right = down;
    }
}

GAME_CALLBACK(game_update) {
    Game_State *state = (Game_State *)memory->data;

    state->dt = (imports->real_time / 1000.0f) - state->real_time;
    state->real_time = imports->real_time / 1000.0f;

    float speed = 100.0f;
    
    if(state->move_up) {
        state->player->position.y -= speed * state->dt;
    }
    if(state->move_down) {
        state->player->position.y += speed * state->dt;
    }
    if(state->move_left) {
        state->player->position.x -= speed * state->dt;
    }
    if(state->move_right) {
        state->player->position.x += speed * state->dt;
    }
}

GAME_CALLBACK(game_render) {
    Game_State *state = (Game_State *)memory->data;

    wglMakeCurrent(imports->dc, imports->glrc);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    For(state->entities) {
        Entity *entity = it;
        immediate_render_texture(&it->texture, it->position);
    }
}