#include "invaders_renderer.h"
#include "common.h"
#include "invaders_timer.h"
#include "invaders_opengl.h"

#include <sstream>

#define WINDOW_CENTER WINDOW_WIDTH * 0.5f

namespace Renderer {
  using namespace Math;
  using namespace Game;
  using namespace std::string_literals;

  static constexpr v4 kWhiteColour  = v4{ 1.0f, 1.0f, 1.0f, 1.0f };
  static constexpr v4 kYellowColour = v4{ 1.0f, 1.0f, 0.0f, 1.0f };

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
    // draw points on bottom right
    const auto pts = std::to_string(args.playerPoints);
    const auto width = m_textRenderer.getTextWidth(pts);
    m_textRenderer.renderText(pts, kWhiteColour, WINDOW_CENTER - (width * 0.5f), 25.0f, 1.0f);
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
    std::stringstream volumess;
    volumess << '<' << m_resourceManager.getVolume() << '>';
    const auto volumestr = "Sound:" + volumess.str();
    // this won't change frequently so it's a good idea to make them static
    static const auto continueWidth = m_textRenderer.getTextWidth("Continue");
    static const auto soundWidth    = m_textRenderer.getTextWidth(volumestr);
    static const auto quitWidth     = m_textRenderer.getTextWidth("Quit");
    if(m_menuManager.currentItem() == Game::MenuItem::CONTINUE) {
      m_textRenderer.renderText("Continue", kYellowColour, WINDOW_CENTER - (continueWidth * 0.5f), 600.0f, 1.0f);
      m_textRenderer.renderText(volumestr,  kWhiteColour,  WINDOW_CENTER - (soundWidth * 0.5f), 500.0f, 1.0f);
      m_textRenderer.renderText("Quit",     kWhiteColour,  WINDOW_CENTER - (quitWidth * 0.5f), 400.0f, 1.0f);
    } else if(m_menuManager.currentItem() == Game::MenuItem::SOUND) {
      m_textRenderer.renderText("Continue", kWhiteColour,  WINDOW_CENTER - (continueWidth * 0.5f), 600.0f, 1.0f);
      m_textRenderer.renderText(volumestr,  kYellowColour, WINDOW_CENTER - (soundWidth * 0.5f), 500.0f, 1.0f);
      m_textRenderer.renderText("Quit",     kWhiteColour,  WINDOW_CENTER - (quitWidth * 0.5f), 400.0f, 1.0f);
    } else if(m_menuManager.currentItem() == Game::MenuItem::QUIT) {
      m_textRenderer.renderText("Continue", kWhiteColour,  WINDOW_CENTER - (continueWidth * 0.5f), 600.0f, 1.0f);
      m_textRenderer.renderText(volumestr,  kWhiteColour,  WINDOW_CENTER - (soundWidth * 0.5f), 500.0f, 1.0f);
      m_textRenderer.renderText("Quit",     kYellowColour, WINDOW_CENTER - (quitWidth * 0.5f), 400.0f, 1.0f);
    }
  }

  void RendererManager::renderEnd(const std::array<ScoreEntry, 5>& scores, const bool topfive)
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto msg = "Congratulations!"s;
    if(!topfive) {
      msg = "Oops. You didn't make it :(";
    }
    auto width = m_textRenderer.getTextWidth(msg);
    m_textRenderer.renderText(msg, kWhiteColour, WINDOW_CENTER - (width * 0.5f), 800.0f, 1.0f);
    auto height = 700.0f;
    for(unsigned long i{ 0 }; i < scores.size(); ++i) {
      // yes, it's not good to build strings on the fly like this, but cba
      // for now bc this repeats only five times
      if(scores[i].m_datetimebuff[0] != '\0') {
	msg = std::string(scores[i].m_datetimebuff.data()) + " - " + std::to_string(scores[i].m_score);
	width = m_textRenderer.getTextWidth(msg);
	m_textRenderer.renderText(msg,
				  (scores[i].m_currentScore) ? kYellowColour : kWhiteColour,
				  WINDOW_CENTER - (width * 0.5f),
				  height,
				  1.0f);
	height -= 50.0f;
      }
    }
    height -= 50.0f;
    msg = "Press ENTER to play again or Q/ESC to quit";
    width = m_textRenderer.getTextWidth(msg);
    m_textRenderer.renderText(msg, kWhiteColour, WINDOW_CENTER - (width * 0.5f), height, 1.0f);
  }

  void RendererManager::renderStart()
  {
    glActiveTexture(GL_TEXTURE0);
    m_resourceManager.useShaderProgram(m_basicShader->m_id);
    glBindVertexArray(m_basicShader->m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_invadersTexture->m_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    auto msg = "Hi, welcome! Press SPACE/ENTER to play!"s;
    auto width = m_textRenderer.getTextWidth(msg);
    m_textRenderer.renderText(msg, kWhiteColour, WINDOW_CENTER - (width * 0.5f), 600.0f, 1.0f);
    msg = "You can also press Q/ESC to quit";
    width = m_textRenderer.getTextWidth(msg);
    m_textRenderer.renderText(msg, kWhiteColour, WINDOW_CENTER - (width * 0.5f), 500.0f, 1.0f);
  }

  void RendererManager::renderLevelLabel(const unsigned int currentLevel, const float alpha)
  {
    std::stringstream ss;
    ss << "Level " << (currentLevel + 1);
    const auto msg = ss.str();
    const auto width = m_textRenderer.getTextWidth(msg);
    m_textRenderer.renderText(msg, v4{ 1.0f, 1.0f, 1.0f, alpha }, WINDOW_CENTER - (width * 0.5f), 300.0f, 1.0f);
  }

  void RendererManager::renderAudioDeviceSelection(const std::vector<AudioDevice>& devices, const unsigned int currentOption)
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    static const auto msg = "Select your audio device";
    static const auto width = m_textRenderer.getTextWidth(msg);
    m_textRenderer.renderText(msg, kWhiteColour, WINDOW_CENTER - (width * 0.5f), 800.0f, 1.0f);
    auto height = 600.0f;
    for(unsigned long i{ 0 }; i < devices.size(); ++i) {
      const auto deviceName = devices[i].m_name;
      const auto deviceNameWidth = m_textRenderer.getTextWidth(deviceName);
      auto colour = (i == currentOption) ? kYellowColour : kWhiteColour;
      m_textRenderer.renderText(deviceName, colour, WINDOW_CENTER - (deviceNameWidth * 0.5f), height, 1.0f);
      height -= 100.0f;
    }
  }

};
