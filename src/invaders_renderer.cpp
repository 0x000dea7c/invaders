#include "invaders_renderer.h"
#include "common.h"
#include "invaders_resources.h"
#include "invaders_timer.h"
#include "invaders_opengl.h"

#include <cassert>

extern Res::ResourceManager gResourceManager;

namespace Renderer {
  using namespace Math;

  RendererManager::RendererManager()
  {

  }

  RendererManager::~RendererManager()
  {

  }

  void RendererManager::init()
  {
    // get all shaders and textures, this means it was loaded before this class is created
    assert(IDs::SID_SHADER_MAIN_BACKGROUND != 0);
    assert(IDs::SID_TEX_MAIN_BACKGROUND != 0);
    assert(IDs::SID_SHADER_ALIEN != 0);
    assert(IDs::SID_TEX_ALIEN_ATLAS != 0);
    assert(IDs::SID_SHADER_PLAYER_LIVES != 0);
    assert(IDs::SID_TEX_PLAYER_LIVES != 0);
    assert(IDs::SID_SHADER_PLAYER != 0);
    assert(IDs::SID_TEX_PLAYER != 0);
    assert(IDs::SID_SHADER_EXPLOSION != 0);
    assert(IDs::SID_TEX_EXPLOSION != 0);
    assert(IDs::SID_SHADER_MISSILE_PLAYER != 0);
    assert(IDs::SID_TEX_MISSILE_PLAYER != 0);
    assert(IDs::SID_SHADER_MISSILE_ALIEN != 0);
    assert(IDs::SID_TEX_MISSILE_ALIEN != 0);
    m_backgroundShader  = gResourceManager.getShader(IDs::SID_SHADER_MAIN_BACKGROUND);
    m_backgroundTex = gResourceManager.getTex(IDs::SID_TEX_MAIN_BACKGROUND);
    m_alienShader  = gResourceManager.getShader(IDs::SID_SHADER_ALIEN);
    m_alienTex = gResourceManager.getTex(IDs::SID_TEX_ALIEN_ATLAS);
    m_playerLivesShader = gResourceManager.getShader(IDs::SID_SHADER_PLAYER_LIVES);
    m_playerLivesTex = gResourceManager.getTex(IDs::SID_TEX_PLAYER_LIVES);
    m_playerShader = gResourceManager.getShader(IDs::SID_SHADER_PLAYER);
    m_playerTex = gResourceManager.getTex(IDs::SID_TEX_PLAYER);
    m_explosionShader = gResourceManager.getShader(IDs::SID_SHADER_EXPLOSION);
    m_explosionTex = gResourceManager.getTex(IDs::SID_TEX_EXPLOSION);
    m_missilePlayerShader = gResourceManager.getShader(IDs::SID_SHADER_MISSILE_PLAYER);
    m_missilePlayerTex = gResourceManager.getTex(IDs::SID_TEX_MISSILE_PLAYER);
    m_missileAlienShader = gResourceManager.getShader(IDs::SID_SHADER_MISSILE_ALIEN);
    m_missileAlienTex = gResourceManager.getTex(IDs::SID_TEX_MISSILE_ALIEN);
  }

  void RendererManager::close()
  {

  }

  void RendererManager::render(const RenderArgs& args)
  {
    // clear
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // activate texture (in this game there's only one texture per shader)
    glActiveTexture(GL_TEXTURE0);
    // background
    gResourceManager.useShaderProgram(m_backgroundShader->m_id);
    gResourceManager.setUniformFloat(m_backgroundShader->m_id, "time", Game::time() * 0.6f);
    glBindVertexArray(m_backgroundShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_backgroundTex->m_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // alien batch draw
    gResourceManager.useShaderProgram(m_alienShader->m_id);
    glBindVertexArray(m_alienShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_alienTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.aliensToDraw);
    // player lives batch draw
    gResourceManager.useShaderProgram(m_playerLivesShader->m_id);
    glBindVertexArray(m_playerLivesShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_playerLivesTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.playerLivesToDraw);
    // player
    gResourceManager.useShaderProgram(m_playerShader->m_id);
    glBindVertexArray(m_playerShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_playerTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.playersToDraw);
    // explosion batch draw
    gResourceManager.useShaderProgram(m_explosionShader->m_id);
    glBindVertexArray(m_explosionShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_explosionTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.explosionsToDraw);
    // player missiles batch draw
    gResourceManager.useShaderProgram(m_missilePlayerShader->m_id);
    glBindVertexArray(m_missilePlayerShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_missilePlayerTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.playerMissilesToDraw);
    // alien missiles batch draw
    gResourceManager.useShaderProgram(m_missileAlienShader->m_id);
    glBindVertexArray(m_missileAlienShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_missileAlienTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.alienMissilesToDraw);
  }

};
