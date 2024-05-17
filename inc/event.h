#pragma once

#include "invaders_math.h"

#include <functional>
#include <vector>
#include <unordered_map>
#include <variant>

namespace Game {
  class Alien;
  class Player;
}

namespace Ev {

  // @YOLO: if you need to pass new data with an event, you need to
  // add it here. Don't know if this is the best approach...
  // you also need to be careful not to pass heavy objects
  using EventData = std::variant<std::nullptr_t, Game::Alien*, Game::Player*>;

  enum class EventType {
    PlayerDestroyed,
    AlienDestroyed,
    MenuContinue,
    MenuIncreaseVolume,
    MenuDecreaseVolume,
    MenuQuit
  };

  class Event final {
  public:
    explicit Event(EventType type)
      : m_data{ nullptr },
	m_type{ type }
    {}
    Event(EventType type, EventData data)
      : m_data{ data },
	m_type{ type }
    {}
    inline auto getData() const noexcept { return m_data; }
    inline auto getType() const noexcept { return m_type; }
  private:
    EventData m_data;
    EventType m_type;
  };

  class EventManager final {
  public:
    inline void subscribe(const EventType type, std::function<void(const Event&)> listener)
    {
      m_listeners[type].push_back(listener);
    }
    inline void post(const Event event)
    {
      auto& listeners = m_listeners[event.getType()];
      for(auto& f : listeners) {
        f(event);
      }
    }
  private:
    std::unordered_map<EventType, std::vector<std::function<void(const Event&)>>> m_listeners;
  };

};
