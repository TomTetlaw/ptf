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
    ENTITY_GOAL,
};

struct Entity {
    int parity = 0;
    int index = 0;
    Entity_Type type = ENTITY_BASE;
    const char *label = nullptr;

    Texture texture;
    Vector4 colour;
    int z = 0;

    Vector2 size;
    Vector2 half_size;

    // tile map position
    Vector2i tile_map_position;

    // next,prev in world tile
    Entity *next = nullptr;
    Entity *prev = nullptr;

    bool is_blocking = false;
    bool is_pushable = false;
    bool blocks_push = false;
};

void set_entity_texture(Entity *entity, const char *filename) {
    assert(entity);
    load_texture(filename, &entity->texture);
    entity->size = Vector2((float)entity->texture.width, (float)entity->texture.height);
    entity->size = Vector2(entity->size.x / 2, entity->size.y / 2);
}

#define MAX_TILES 1024
#define TILE_MAP_EXTRA 2*1024*1024

struct World_Tile {
    Entity *entities = nullptr;
};

struct Tile_Map_State {
    World_Tile *tiles = nullptr;
    int width = 0;
    int height = 0;
};

//todo: sort entities by Z
void add_entity_to_tile(World_Tile *tile, Entity *entity) {
    assert(tile);
    if(tile->entities) {
        entity->prev = tile->entities;
        entity->next = nullptr;
        tile->entities->next = entity;
    } else {
        tile->entities = entity;
        entity->prev = nullptr;
        entity->next = nullptr;
    }
}

void remove_entity_from_tile(World_Tile *tile) {
    assert(tile);

    if(tile->entities->prev) {
        tile->entities->prev->next = tile->entities->next;
    }
    if(tile->entities->next) {
        tile->entities->next->prev = tile->entities->prev;
    }
}

bool tile_is_blocking(World_Tile *tile) {
    Entity *entity = tile->entities;
    while(entity) {
        if(entity->is_blocking) {
            return true;
        }
        entity = entity->next;
    }

    return false;
}

bool tile_is_goal(World_Tile *tile) {
     Entity *entity = tile->entities;
    while(entity) {
        if(entity->type == ENTITY_GOAL) {
            return true;
        }
        entity = entity->next;
    }

    return false;   
}

bool tile_blocks_push(World_Tile *tile) {
     Entity *entity = tile->entities;
    while(entity) {
        if(entity->blocks_push) {
            return true;
        }
        entity = entity->next;
    }

    return false;   
}

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

    Memory_Arena tile_map_arena;
    Tile_Map_State tile_map_state;
};

World_Tile *get_world_tile(Game_State *state, Vector2i position) {
    assert(position.x >= 0 && position.x < state->tile_map_state.width);
    assert(position.y >= 0 && position.y < state->tile_map_state.height);
    return &state->tile_map_state.tiles[position.x + position.y * state->tile_map_state.width];
}

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
    entity->colour = Vector4(1, 1, 1, 1);
    state->entities.num++;

    return entity;
}

void setup_entity_player(Entity *entity) {
    entity->label = "player";
    entity->type = ENTITY_PLAYER;
    entity->z = 3;
    set_entity_texture(entity, "data/textures/player.png");
}

Tile_Map map_test;

GAME_CALLBACK(game_init) {
    ::imports = imports;
    ::memory = memory;
    memory->used = sizeof(Game_State);

    Game_State *state = (Game_State *)memory->data;

    arena_create(memory, &state->entities_arena, (sizeof(Entity) * MAX_ENTITIES) * 2);
    state->entities_allocator.arena = &state->entities_arena;
    state->entities.allocator = state->entities_allocator;
    state->entities.ensure_size(MAX_ENTITIES);

    arena_create(memory, &state->tile_map_arena, (sizeof(World_Tile) * MAX_TILES) + TILE_MAP_EXTRA);

    wglMakeCurrent(imports->dc, imports->glrc);
    glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state->player = create_entity(state);
    setup_entity_player(state->player);

    map_test.width = 8;
    map_test.height = 8;
    map_test.string =
    "xxxxxxxx" \
    "x   x  x" \
    "x x x.gx" \
    "x    .gx" \
    "x x x.gx" \
    "x   x  x" \
    "xxxxxp x" \
    "    xxxx";

    state->tile_map_state.width = map_test.width;
    state->tile_map_state.height = map_test.height;
    state->tile_map_state.tiles = (World_Tile *)arena_alloc(&state->tile_map_arena, 
        sizeof(World_Tile) * (state->tile_map_state.width * state->tile_map_state.height));

    for(int y = 0; y < state->tile_map_state.height; y++) {
        for(int x = 0; x < state->tile_map_state.width; x++) {
            int index = x + (y * state->tile_map_state.width);
            char c = map_test.string[index];
            printf("%c\n",c);
            Vector2i tile_map_position = Vector2i(x, y);
            
            assert(c >= 32);

            if(c == ' ') {
                continue;
            } else if (c == 'p') {
                state->player->tile_map_position = tile_map_position;
                continue;
            }

            Entity *entity = create_entity(state);
            World_Tile *tile = &state->tile_map_state.tiles[index];
            tile->entities = entity;

            entity->tile_map_position = tile_map_position;
            switch(c) {
            case 'x':
                entity->label = "wall";
                entity->type = ENTITY_WALL;
                entity->is_blocking = true;
                entity->z = 0;
                set_entity_texture(entity, "data/textures/wall.png");
                break;
            case '.':
                entity->label = "box";
                entity->type = ENTITY_BOX;
                entity->is_pushable = true;
                entity->blocks_push = true;
                entity->z = 2;
                set_entity_texture(entity, "data/textures/box.png");
                break;
            case 'g':
                entity->label = "goal";
                entity->type = ENTITY_GOAL;
                entity->z = 1;
                set_entity_texture(entity, "data/textures/goal.png");
                break;
            }
        }
    }
}

GAME_HANDLE_KEY_CALLBACK(game_handle_key) {
    Game_State *state = (Game_State *)memory->data;
    
    if(!down) {
        return;
    }

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

    for(int y = 0; y < state->tile_map_state.height; y++) {
        for(int x = 0; x < state->tile_map_state.width; x++) {
            state->tile_map_state.tiles[x + (y * state->tile_map_state.width)].entities = nullptr;
        }
    }

    For(state->entities) {
        int x = it->tile_map_position.x;
        int y = it->tile_map_position.y;
        add_entity_to_tile(&state->tile_map_state.tiles[x + (y * state->tile_map_state.width)], it);
    }

    Vector2i new_position = state->player->tile_map_position;

    if(state->move_up) {
        state->move_up = false;
        new_position = new_position + Vector2i(0, -1);
    }
    if(state->move_down) {
        state->move_down = false;
        new_position = new_position + Vector2i(0, 1);
    }
    if(state->move_left) {
        state->move_left = false;
        new_position = new_position + Vector2i(-1, 0);
    }
    if(state->move_right) {
        state->move_right = false;
        new_position = new_position + Vector2i(1, 0);
    }
    
    if(new_position != state->player->tile_map_position) {
        World_Tile *tile = get_world_tile(state, new_position);
        if(!tile_is_blocking(tile)) {
            Entity *entity = tile->entities;
            while(entity) {
                if(entity->is_pushable) {
                    Vector2i push_dir;

                    Vector2i a = state->player->tile_map_position;
                    Vector2i b = entity->tile_map_position;
                    if(a.x < b.x) {
                        push_dir.x = 1;
                    } else if(a.x > b.x) {
                        push_dir.x = -1;
                    } else if (a.y < b.y) {
                        push_dir.y = 1;
                    } else {
                        push_dir.y = -1;
                    }

                    Vector2i resulting_position = entity->tile_map_position + push_dir;
                    World_Tile *push_to_tile = get_world_tile(state, resulting_position);

                    if(!tile_is_blocking(push_to_tile) && !tile_blocks_push(push_to_tile)) {
                        state->player->tile_map_position = state->player->tile_map_position + push_dir;
                        entity->tile_map_position = resulting_position;

                        if(tile_is_goal(push_to_tile)) {
                            entity->colour = Vector4(0, 1, 0, 1);
                        } else {
                            entity->colour = Vector4(1, 1, 1, 1);
                        }
                    }
                }
                entity = entity->next;
            }

            if(!tile_blocks_push(tile)) {
                state->player->tile_map_position = new_position;
            }
        }
    }
}

/*
void qsort (void* base, size_t num, size_t size,
            int (*compar)(const void*,const void*));
*/

int sort_entity_by_z(const void *a, const void *b) {
    return ((Entity *)a)->z - ((Entity *)b)->z;
}

GAME_CALLBACK(game_render) {
    Game_State *state = (Game_State *)memory->data;

    wglMakeCurrent(imports->dc, imports->glrc);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /*for(int y = 0; y < state->tile_map_state.height; y++) {
        for(int x = 0; x < state->tile_map_state.width; x++) {
            World_Tile *tile = &state->tile_map_state.tiles[x + (y * state->tile_map_state.width)];
            
            Entity *entity = tile->entities;
            while(entity) {
                immediate_render_texture(&entity->texture, v2_to_v2i(entity->tile_map_position, 64), entity->colour);
                entity = entity->next;
            }
        }
    }*/

    static Entity *sorted_entities = (Entity *)arena_alloc(&state->entities_arena, sizeof(Entity) * state->entities.num);
    memcpy(sorted_entities, state->entities.data, sizeof(Entity) * state->entities.num);
    qsort(sorted_entities, state->entities.num, sizeof(Entity), sort_entity_by_z);

    for(int i = 0; i < state->entities.num; i++) {
        immediate_render_texture(&sorted_entities[i].texture, v2_to_v2i(sorted_entities[i].tile_map_position, 64), sorted_entities[i].colour);
    }
}