#include "invaders_input.h"

namespace Input {

  InputManager::InputManager()
  {

  }

  InputManager::~InputManager()
  {

  }

  void InputManager::init()
  {

  }

  void InputManager::close()
  {

  }

  bool InputManager::isKeyHeld(const Key k) const
  {
    return prevkeys[k] && currkeys[k];
  }

  bool InputManager::isKeyPressed(const Key k) const
  {
    return !prevkeys[k] && currkeys[k];
  }

  void InputManager::beginFrame()
  {
    for(int i = 0; i < Key::KEY_COUNT; ++i) {
      prevkeys[i] = currkeys[i];
    }
  }

  void InputManager::updateKey(const Key k, const bool pressed)
  {
    currkeys[k] = pressed;
  }
};
