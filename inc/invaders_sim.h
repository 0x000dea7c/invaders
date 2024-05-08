#pragma once

namespace Sim {
  // NOTE: this manager should be initialised after ResourceManager!
  // NOTE: since the game doesn't allow window resizing (bc don't know how to handle
  // it properly yet) all game units will be pixels.

  enum class State {
    PLAY,
    MENU,
    WIN_LEVEL,
    WIN_GAME,
    LOSE,
  };

  class SimulationManager {
  public:
    SimulationManager();
    ~SimulationManager();
    void init();
    void close();
    void update(const float delta);
    inline auto shouldEnd() const { return m_end; }
  private:
    void updateGrid();
    void checkGameState();
    int m_sceneWidth;
    int m_sceneHeight;
    State m_state;
    bool m_end;
  };

};
