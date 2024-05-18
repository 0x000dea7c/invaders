#pragma once

#define WINDOW_NAME_GNU_LINUX "X11 Invaders!"

constexpr unsigned int WINDOW_WIDTH{ 1280 };
constexpr unsigned int WINDOW_HEIGHT{ 960 };
constexpr float WORLD_UNIT_TO_PIXEL{ 10.0f };
constexpr float GAME_WIDTH_UNITS{ WINDOW_WIDTH  / WORLD_UNIT_TO_PIXEL };
constexpr float GAME_HEIGHT_UNITS{ WINDOW_HEIGHT / WORLD_UNIT_TO_PIXEL };
constexpr float MISSILE_PLAYER_VEL_Y{ 30.0f };
constexpr float MISSILE_PLAYER_SPAWN_OFFSET{ 3.0f };
constexpr float MISSILE_ALIEN_VEL_Y{ -20.0f };
constexpr float ALIEN_VEL_X{ 10.0f };
constexpr float ALIEN_Y_MOV_AMPLITUDE{ 5.0f };
constexpr float ALIEN_Y_MOV_FREQUENCY{ 0.3f };
constexpr float MAX_ALIENS_ALIVE{ 16 };
constexpr float MAX_PLAYER_LIVES{ 3 };
constexpr float SIMUL_ALIENS_ALIVE{ 20 };
constexpr float SIMUL_MISSILES_ALIVE{ 64 };
constexpr float PLAYER_DIRSCALE{ 500.0f };
constexpr float PLAYER_DRAG{ 4.0f };

//
// 1 game unit - 10 pixels
// x           - 1280 pixels
//
// 1280 pixels are 128 game units
//

