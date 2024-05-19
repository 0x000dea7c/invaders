#pragma once

#include "params.h"

#include <string>

inline float worldToPixels(const float worldunits)
{
  return worldunits * WORLD_UNIT_TO_PIXEL;
}

inline float pixelsToWorld(const float pixels)
{
  return pixels / WORLD_UNIT_TO_PIXEL;
}

// used for converting strings to ids, wikipedia
inline int fnv1a(const std::string& text)
{
  constexpr int prime{ 0x01000193 }; // FNV-1a 32-bit prime
  unsigned int hash{ 0x811c9dc5 }; // FNV-1a 32-bit offset basis
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
  extern int SID_TEX_INVADERS;
  // shader ids
  extern int SID_SHADER_MAIN_BACKGROUND;
  extern int SID_SHADER_ALIEN;
  extern int SID_SHADER_PLAYER_LIVES;
  extern int SID_SHADER_PLAYER;
  extern int SID_SHADER_EXPLOSION;
  extern int SID_SHADER_MISSILE_PLAYER;
  extern int SID_SHADER_MISSILE_ALIEN;
  extern int SID_SHADER_TEXT;
  extern int SID_SHADER_MENU;
  extern int SID_SHADER_BASIC;
  // audio ids
  extern int SID_AUDIO_BG_MUSIC;
  extern int SID_AUDIO_EXPLOSION;
  extern int SID_AUDIO_WIN_LEVEL;
  extern int SID_AUDIO_WIN_GAME;
  extern int SID_AUDIO_LOSE_GAME;
  extern int SID_AUDIO_PLAYER_DIE;
  extern int SID_AUDIO_ROSWELL_IMPACT;
  extern int SID_AUDIO_UFO_SPAWNED;
  extern int SID_AUDIO_ROSWELL_SPAWNED;
};
