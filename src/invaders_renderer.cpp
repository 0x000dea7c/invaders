#include "invaders_renderer.h"
#include "common.h"
#include "invaders_timer.h"
#include "invaders_opengl.h"

namespace Renderer {
  using namespace Math;

  RendererManager::RendererManager(Res::ResourceManager& resourceManager)
    : m_resourceManager{ resourceManager }
  {
    m_backgroundShader    = m_resourceManager.getShader(IDs::SID_SHADER_MAIN_BACKGROUND);
    m_backgroundTex       = m_resourceManager.getTex(IDs::SID_TEX_MAIN_BACKGROUND);
    m_alienShader         = m_resourceManager.getShader(IDs::SID_SHADER_ALIEN);
    m_alienTex            = m_resourceManager.getTex(IDs::SID_TEX_ALIEN_ATLAS);
    m_playerLivesShader   = m_resourceManager.getShader(IDs::SID_SHADER_PLAYER_LIVES);
    m_playerLivesTex      = m_resourceManager.getTex(IDs::SID_TEX_PLAYER_LIVES);
    m_playerShader        = m_resourceManager.getShader(IDs::SID_SHADER_PLAYER);
    m_playerTex           = m_resourceManager.getTex(IDs::SID_TEX_PLAYER);
    m_explosionShader     = m_resourceManager.getShader(IDs::SID_SHADER_EXPLOSION);
    m_explosionTex        = m_resourceManager.getTex(IDs::SID_TEX_EXPLOSION);
    m_missilePlayerShader = m_resourceManager.getShader(IDs::SID_SHADER_MISSILE_PLAYER);
    m_missilePlayerTex    = m_resourceManager.getTex(IDs::SID_TEX_MISSILE_PLAYER);
    m_missileAlienShader  = m_resourceManager.getShader(IDs::SID_SHADER_MISSILE_ALIEN);
    m_missileAlienTex     = m_resourceManager.getTex(IDs::SID_TEX_MISSILE_ALIEN);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  RendererManager::~RendererManager()
  {

  }

  void RendererManager::render(const RenderArgs& args)
  {
    // clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // activate texture (in this game there's only one texture per shader)
    glActiveTexture(GL_TEXTURE0);
    // background
    m_resourceManager.useShaderProgram(m_backgroundShader->m_id);
    m_resourceManager.setUniformFloat(m_backgroundShader->m_id, "time", Game::time() * 1.0f);
    glBindVertexArray(m_backgroundShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_backgroundTex->m_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // alien batch draw
    m_resourceManager.useShaderProgram(m_alienShader->m_id);
    glBindVertexArray(m_alienShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_alienTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.aliensToDraw);
    // player lives batch draw
    m_resourceManager.useShaderProgram(m_playerLivesShader->m_id);
    glBindVertexArray(m_playerLivesShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_playerLivesTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.playerLivesToDraw);
    // player
    m_resourceManager.useShaderProgram(m_playerShader->m_id);
    glBindVertexArray(m_playerShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_playerTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.playersToDraw);
    // explosion batch draw
    m_resourceManager.useShaderProgram(m_explosionShader->m_id);
    glBindVertexArray(m_explosionShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_explosionTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.explosionsToDraw);
    // player missiles batch draw
    m_resourceManager.useShaderProgram(m_missilePlayerShader->m_id);
    glBindVertexArray(m_missilePlayerShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_missilePlayerTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.playerMissilesToDraw);
    // alien missiles batch draw
    m_resourceManager.useShaderProgram(m_missileAlienShader->m_id);
    glBindVertexArray(m_missileAlienShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_missileAlienTex->m_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, args.alienMissilesToDraw);
  }

};
