#include "invaders_input.h"

namespace Input {

  InputManager::InputManager()
  {
    for(unsigned int i{ 0 }; i < static_cast<unsigned int>(Key::KEY_COUNT); ++i) {
      m_currkeys[i] = false;
      m_prevkeys[i] = false;
    }
  }

  InputManager::~InputManager()
  {

  }

  bool InputManager::isKeyHeld(const Key k) const
  {
    return m_prevkeys[static_cast<unsigned int>(k)] && m_currkeys[static_cast<unsigned int>(k)];
  }

  bool InputManager::isKeyPressed(const Key k) const
  {
    return !m_prevkeys[static_cast<unsigned int>(k)] && m_currkeys[static_cast<unsigned int>(k)];
  }

  void InputManager::beginFrame()
  {
    for(unsigned int i{ 0 }; i < static_cast<unsigned int>(Key::KEY_COUNT); ++i) {
      m_prevkeys[i] = m_currkeys[i];
    }
  }

  void InputManager::updateKey(const Key k, const bool pressed)
  {
    m_currkeys[static_cast<unsigned int>(k)] = pressed;
  }

};
