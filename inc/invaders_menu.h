#pragma once

#include "invaders_input.h"
#include "event.h"

namespace Game {

  enum class MenuItem {
    CONTINUE,
    SOUND,
    QUIT,

    UNHANDLED
  };

  inline MenuItem& operator++(MenuItem& m)
  {
    switch(m) {
    case MenuItem::CONTINUE:
      return m = MenuItem::SOUND;
    case MenuItem::SOUND:
      return m = MenuItem::QUIT;
    case MenuItem::QUIT:
      return m = MenuItem::CONTINUE;
    default:
      return m = MenuItem::UNHANDLED;
    }
  }

  inline MenuItem& operator--(MenuItem& m)
  {
    switch(m) {
    case MenuItem::CONTINUE:
      return m = MenuItem::QUIT;
    case MenuItem::SOUND:
      return m = MenuItem::CONTINUE;
    case MenuItem::QUIT:
      return m = MenuItem::SOUND;
    default:
      return m = MenuItem::UNHANDLED;
    }
  }

  class MenuManager final {
  public:
    MenuManager(Ev::EventManager& eventManager,
                const Input::InputManager& inputManager);
    ~MenuManager();
    void update();
  private:
    void handleInput();
    Ev::EventManager& m_eventManager;
    const Input::InputManager& m_inputManager;
    MenuItem m_currentItem;
  };

};
