#include "invaders_menu.h"

namespace Game {
  using namespace Input;
  using namespace Ev;

  MenuManager::MenuManager(Ev::EventManager& eventManager,
                           const Input::InputManager& inputManager)
    : m_eventManager{ eventManager },
      m_inputManager{ inputManager },
      m_currentItem{ MenuItem::CONTINUE }
  {
  }

  MenuManager::~MenuManager()
  {
  }

  void MenuManager::update()
  {
    handleInput();
  }

  void MenuManager::handleInput()
  {
    if(m_inputManager.isKeyPressed(Key::KEY_UP) || m_inputManager.isKeyPressed(Key::KEY_W)) {
      --m_currentItem;
    } else if(m_inputManager.isKeyPressed(Key::KEY_DOWN) || m_inputManager.isKeyPressed(Key::KEY_S)) {
      ++m_currentItem;
    } else if(m_inputManager.isKeyPressed(Key::KEY_ESCAPE)) {
      m_eventManager.post(Event(EventType::MenuContinue));
    } else if(m_inputManager.isKeyPressed(Key::KEY_ENTER)) {
      if(m_currentItem == MenuItem::CONTINUE) {
        m_eventManager.post(Event(EventType::MenuContinue));
      } else if(m_currentItem == MenuItem::QUIT) {
        m_eventManager.post(Event(EventType::MenuQuit));
      }
    } else if(m_currentItem == MenuItem::SOUND) {
      if(m_inputManager.isKeyPressed(Key::KEY_LEFT) || m_inputManager.isKeyPressed(Key::KEY_A)) {
	m_eventManager.post(Event(EventType::MenuDecreaseVolume));
      } else if(m_inputManager.isKeyPressed(Key::KEY_RIGHT) || m_inputManager.isKeyPressed(Key::KEY_D)) {
	m_eventManager.post(Event(EventType::MenuIncreaseVolume));
      }
    }
  }
};
