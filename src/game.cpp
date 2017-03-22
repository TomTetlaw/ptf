#include "precompiled.h"
#include "game.h"

struct Game_State {
    float player_x;
    float player_y;
    bool move_up = false;
    bool move_down = false;
    bool move_left = false;
    bool move_right = false;

    float dt = 0.0f;
    float real_time = 0.0f;
};

GAME_CALLBACK(game_init) {
    Game_State *state = (Game_State *)memory->data;
    state->player_x = 0;
    state->player_y = 0;
    memory->used = sizeof(Game_State);
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
        state->player_y -= speed * state->dt;
    }
    if(state->move_down) {
        state->player_y += speed * state->dt;
    }
    if(state->move_left) {
        state->player_x -= speed * state->dt;
    }
    if(state->move_right) {
        state->player_x += speed * state->dt;
    }
}

GAME_CALLBACK(game_render) {
    Game_State *state = (Game_State *)memory->data;
    wglMakeCurrent(imports->dc, imports->glrc);

    Box player_box;
    player_box.x = state->player_x;
    player_box.y = state->player_y;
    player_box.width = 100.0f;
    player_box.height = 100.0f;

    immediate_render_box(player_box, Vector4(1, 1, 1, 1), false);
}