#pragma once

#include <string>

#define WINDOW_WIDTH          1280
#define WINDOW_HEIGHT         960
#define WINDOW_NAME_GNU_LINUX "X11 Invaders!"
#define MAX_PLAYER_LIVES      3
#define SIMUL_ALIENS_ALIVE    16
#define SIMUL_MISSILES_ALIVE  64

// used for converting strings to ids, wikipedia
inline int fnv1a(const std::string& text)
{
  constexpr int prime = 0x01000193; // FNV-1a 32-bit prime
  int hash = 0x811c9dc5; // FNV-1a 32-bit offset basis
  for(const auto& c : text) {
    hash ^= (unsigned int)c;
    hash *= prime;
  }
  return hash;
}

namespace IDs {
  // texture ids
  extern int SID_TEX_PLAYER;
  extern int SID_TEX_ALIEN_ATLAS;
  extern int SID_TEX_MAIN_BACKGROUND;
  extern int SID_TEX_MISSILE_PLAYER;
  extern int SID_TEX_MISSILE_ALIEN;
  extern int SID_TEX_EXPLOSION;
  extern int SID_TEX_PLAYER_LIVES;
  // shader ids
  extern int SID_SHADER_MAIN_BACKGROUND;
  extern int SID_SHADER_ALIEN;
  extern int SID_SHADER_PLAYER_LIVES;
  extern int SID_SHADER_PLAYER;
  extern int SID_SHADER_EXPLOSION;
  extern int SID_SHADER_MISSILE_PLAYER;
  extern int SID_SHADER_MISSILE_ALIEN;
};
