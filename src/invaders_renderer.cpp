#include "invaders_renderer.h"
#include "common.h"
#include "invaders_timer.h"
#include "invaders_opengl.h"
#include <sstream>

namespace Renderer {
  using namespace Math;
  using namespace Game;

  static constexpr v4 kWhiteColour  = v4{ 1.0f, 1.0f, 1.0f, 1.0f };
  static constexpr v4 kYellowColour = v4{ 1.0f, 1.0f, 0.0f, 1.0f };
  static constexpr v4 kSilverColour = v4{ 0.75f, 0.75f, 0.75f, 1.0f };

  RendererManager::RendererManager(Res::ResourceManager& resourceManager,
                                   TextRenderer& textRenderer,
                                   const MenuManager& menuManager)
    : m_resourceManager{ resourceManager },
      m_textRenderer{ textRenderer },
      m_menuManager{ menuManager }
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
    m_menuShader          = m_resourceManager.getShader(IDs::SID_SHADER_MENU);
    m_basicShader         = m_resourceManager.getShader(IDs::SID_SHADER_BASIC);
    m_invadersTexture     = m_resourceManager.getTex(IDs::SID_TEX_INVADERS);
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
    // player batch draw
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

  void RendererManager::renderMenu()
  {
    // you're not clearing because you want to show the game behind when you pause
    // this draws a black rectangle at the center of the screen
    glActiveTexture(GL_TEXTURE0);
    m_resourceManager.useShaderProgram(m_menuShader->m_id);
    glBindVertexArray(m_menuShader->m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // now draw text on top of it
    // @HACK: since you don't have any way to center text on the screen, you need to do this
    // make them static to avoid computing this every time
    static const auto centerX       = WINDOW_WIDTH * 0.5f;
    static const auto continueWidth = m_textRenderer.getTextWidth("Continue");
    static const auto soundWidth    = m_textRenderer.getTextWidth("Sound");
    static const auto quitWidth     = m_textRenderer.getTextWidth("Quit");
    if(m_menuManager.currentItem() == Game::MenuItem::CONTINUE) {
      m_textRenderer.renderText("Continue", kYellowColour, centerX - (continueWidth * 0.5f), 600.0f, 1.0f);
      m_textRenderer.renderText("Sound",    kWhiteColour,  centerX - (soundWidth * 0.5f), 500.0f, 1.0f);
      m_textRenderer.renderText("Quit",     kWhiteColour,  centerX - (quitWidth * 0.5f), 400.0f, 1.0f);
    } else if(m_menuManager.currentItem() == Game::MenuItem::SOUND) {
      m_textRenderer.renderText("Continue", kWhiteColour,  centerX - (continueWidth * 0.5f), 600.0f, 1.0f);
      m_textRenderer.renderText("Sound",    kYellowColour, centerX - (soundWidth * 0.5f), 500.0f, 1.0f);
      m_textRenderer.renderText("Quit",     kWhiteColour,  centerX - (quitWidth * 0.5f), 400.0f, 1.0f);
    } else if(m_menuManager.currentItem() == Game::MenuItem::QUIT) {
      m_textRenderer.renderText("Continue", kWhiteColour,  centerX - (continueWidth * 0.5f), 600.0f, 1.0f);
      m_textRenderer.renderText("Sound",    kWhiteColour,  centerX - (soundWidth * 0.5f), 500.0f, 1.0f);
      m_textRenderer.renderText("Quit",     kYellowColour, centerX - (quitWidth * 0.5f), 400.0f, 1.0f);
    }
  }

  void RendererManager::renderWinScreen()
  {
    glActiveTexture(GL_TEXTURE0);
    m_resourceManager.useShaderProgram(m_basicShader->m_id);
    glBindVertexArray(m_basicShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_invadersTexture->m_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    static const auto centerX      = WINDOW_WIDTH * 0.5f;
    static const auto winMsgWidth  = m_textRenderer.getTextWidth("Congratulations! You win!");
    static const auto win2MsgWidth = m_textRenderer.getTextWidth("Press SPACE/ENTER to play again or Q/ESC to quit");
    m_textRenderer.renderText("Congratulations! You win!", kWhiteColour, centerX - (winMsgWidth * 0.5f), 600.0f, 1.0f);
    m_textRenderer.renderText("Press SPACE/ENTER to play again or Q/ESC to quit", kWhiteColour, centerX - (win2MsgWidth * 0.5f), 500.0f, 1.0f);
  }

  void RendererManager::renderLoseScreen()
  {
    glActiveTexture(GL_TEXTURE0);
    m_resourceManager.useShaderProgram(m_basicShader->m_id);
    glBindVertexArray(m_basicShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_invadersTexture->m_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    static const auto centerX       = WINDOW_WIDTH * 0.5f;
    static const auto loseMsgWidth  = m_textRenderer.getTextWidth("You lost :(");
    static const auto lose2MsgWidth = m_textRenderer.getTextWidth("Press SPACE/ENTER to play again or Q/ESC to quit");
    m_textRenderer.renderText("You lost :(", kWhiteColour, centerX - (loseMsgWidth * 0.5f), 600.0f, 1.0f);
    m_textRenderer.renderText("Press SPACE/ENTER to play again or Q/ESC to quit", kWhiteColour, centerX - (lose2MsgWidth * 0.5f), 500.0f, 1.0f);
  }

  void RendererManager::renderStartScreen()
  {
    glActiveTexture(GL_TEXTURE0);
    m_resourceManager.useShaderProgram(m_basicShader->m_id);
    glBindVertexArray(m_basicShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_invadersTexture->m_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    static const auto centerX   = WINDOW_WIDTH * 0.5f;
    static const auto msgWidth  = m_textRenderer.getTextWidth("Hi, welcome! Press SPACE/ENTER to play!");
    static const auto msg2Width = m_textRenderer.getTextWidth("You can also press Q/ESC to quit");
    m_textRenderer.renderText("Hi, welcome! Press SPACE/ENTER to play!", kSilverColour, centerX - (msgWidth * 0.5f), 600.0f, 1.0f);
    m_textRenderer.renderText("You can also press Q/ESC to quit", kSilverColour, centerX - (msg2Width * 0.5f), 500.0f, 1.0f);
  }

  void RendererManager::renderLevelLabel(const unsigned int currentLevel, const float alpha)
  {
    std::stringstream ss;
    ss << "Level " << (currentLevel + 1);
    static const auto centerX  = WINDOW_WIDTH * 0.5f;
    const auto str = ss.str();
    static const auto msgWidth = m_textRenderer.getTextWidth(str);
    m_textRenderer.renderText(str, v4{ 1.0f, 1.0f, 1.0f, alpha }, centerX - (msgWidth * 0.5f), 300.0f, 1.0f);
  }

};
