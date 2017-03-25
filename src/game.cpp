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
    ENTITY_PLAYER,
    ENTITY_BOX,
    ENTITY_WALL,
};

struct Entity {
    int parity = 0;
    int index = 0;
    const char *label = nullptr;
    Vector2 position;
    Vector2 velocity;
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

struct Tile_Map {
    int width = 0;
    int height = 0;
    const char *string = nullptr;
};

Entity *create_entity(Game_State *state) {
    int index = 0;

    For(state->entities) {
        if(!(*it).index) {
            (*it).index = it_index + 1;
            return &(*it);
        }
    }

    if(state->entities.num + 1 >= MAX_ENTITIES) {
        return nullptr;
    }

    Entity *entity = &state->entities[state->entities.num];
    entity->index = state->entities.num + 1;
    entity->parity = state->next_parity++;
    state->entities.num++;

    return entity;
}

void setup_entity_player(Entity *entity) {
    entity->label = "player";
    entity->type = ENTITY_PLAYER;
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

    Tile_Map map_test;
    map_test.width = 10;
    map_test.height = 10;
    map_test.string =
        "xxxxxxxxxx" \
        "x . . . .x" \
        "x        x" \
        "x. . . . x" \
        "x        x" \
        "x   p    x" \
        "x        x" \
        "x        x" \
        "x        x" \
        "xxxxxxxxxx";

    int w = map_test.width;
    int h = map_test.height;
    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            char c = map_test.string[x + (y * h)];
            Vector2 position = Vector2((float)x * 64, (float)y * 64);
            
            if(c == ' ') {
                continue;
            } else if (c == 'p') {
                state->player->position = position;
                continue;
            }

            Entity *entity = create_entity(state);
            entity->position = position;
            switch(c) {
            case 'x':
                entity->label = "wall";
                entity->type = ENTITY_WALL;
                load_texture("data/textures/wall.png", &entity->texture);
                break;
            case '.':
                entity->label = "box";
                entity->type = ENTITY_BOX;
                load_texture("data/textures/box.png", &entity->texture);
                break;
            }
        }
    }
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
    
    state->player->velocity = Vector2(0, 0);

    if(state->move_up) {
        state->player->velocity.y = -speed;
    }
    if(state->move_down) {
        state->player->velocity.y = speed;
    }
    if(state->move_left) {
        state->player->velocity.x = -speed;
    }
    if(state->move_right) {
        state->player->velocity.x = speed;
    }

    /*For(state->entities)*/ {
        Entity *it = state->player;
        int it_index = state->player->index - 1;
        Vector2 new_position = it->position + (it->velocity * state->dt);
        bool hit_test = false;
        AABB_Inside_Result result;

        AABB a;
        a.min_x = new_position.x;
        a.min_y = new_position.y;
        a.max_x = new_position.x + it->texture.width;
        a.max_y = new_position.y + it->texture.height;

        for(int other_index = 0; other_index < state->entities.num; other_index++) {
            if(it_index == other_index) {
                continue;
            }

            Entity *other = &state->entities[other_index];
    
            AABB b;
            b.min_x = other->position.x;
            b.min_y = other->position.y;
            b.max_x = other->position.x + other->texture.width;
            b.max_y = other->position.y + other->texture.height;

            result = aabb_inside(a, b);
            if(result.is_inside) {
                break;
            }
        }
        
        if(result.is_inside) {
            switch(result.direction) {
            case AABB_LEFT:
                printf("left\n");
                break;
            case AABB_RIGHT:
                printf("right\n");
                break;
            case AABB_TOP:
                printf("top\n");
                break;
            case AABB_BOTTOM:
                printf("bottom\n");
                break;
            }
        } else {
            printf("not inside\n");
            it->position = it->position + (it->velocity * state->dt);
        }
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