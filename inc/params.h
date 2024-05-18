#pragma once

// you want to make them constexpr

#define WORLD_UNIT_TO_PIXEL          10.0f
#define GAME_WIDTH_UNITS             WINDOW_WIDTH  / WORLD_UNIT_TO_PIXEL
#define GAME_HEIGHT_UNITS            WINDOW_HEIGHT / WORLD_UNIT_TO_PIXEL
#define MISSILE_PLAYER_VEL_Y         30.0f
#define MISSILE_PLAYER_SPAWN_OFFSET   3.0f
#define MISSILE_ALIEN_VEL_Y         -20.0f
#define ALIEN_VEL_X                  10.0f
#define ALIEN_Y_MOV_AMPLITUDE         5.0f
#define ALIEN_Y_MOV_FREQUENCY         0.3f
#define MAX_ALIENS_ALIVE             16
#define MAX_PLAYER_LIVES              3
#define SIMUL_ALIENS_ALIVE           20
#define SIMUL_MISSILES_ALIVE         64
#define PLAYER_DIRSCALE              500.0f
#define PLAYER_DRAG                    4.0f

//
// 1 game unit - 10 pixels
// x           - 1280 pixels
//
// 1280 pixels are 128 game units
//

