#pragma once

#include <functional>
#include <vector>
#include <unordered_map>

namespace Ev {

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
      : m_type{ type },
        m_entity{ nullptr }
    {}
    // NOTE: you know this is bad!!!!!!! cursed code, the reason for this is because there are situations
    // where you need to delete an alien or a player when they collide with a missile. In those cases,
    // the missile emits an event of player/alien destroyed and then within that event a reference of the
    // entity to be deleted is passed... this is probably wrong.
    explicit Event(EventType type, void* entity)
      : m_type{ type },
        m_entity{ entity }
    {}
    inline auto getType() const { return m_type; }
    inline auto getEntity() const { return m_entity; }
  private:
    EventType m_type;
    void* m_entity;
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
