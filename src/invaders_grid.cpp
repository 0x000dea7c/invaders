#include "invaders_grid.h"

namespace Game {
  using namespace Math;

  GridManager::GridManager(const int sceneWidth, const int sceneHeight)
  {
    m_rows = std::ceil(sceneWidth  / CELL_WIDTH);
    m_cols = std::ceil(sceneHeight / CELL_HEIGHT);
    m_grid.resize(100);
    for(unsigned long i{ 0 }; i < m_grid.size(); ++i) {
      m_grid[i].resize(8);
    }
  }

  GridManager::~GridManager()
  {

  }

  void GridManager::beginFrame()
  {
    // reset grid
    for(unsigned long i{ 0 }; i < m_grid.size(); ++i) {
      m_grid[i].clear();
    }
  }

  void GridManager::update(const v3& pos, const void* ent, const EntityType type)
  {
    const auto id = getId(pos);
    m_grid[id].emplace_back(EntityGridData{ ent, type });
  }

  // you don't expect more than two entities per cell, so returning a vector shouldn't hurt much...
  std::vector<EntityGridData> GridManager::getNearby(const Math::v3& pos, const EntityType wanted)
  {
    const auto cellId = getId(pos);
    std::vector<EntityGridData> ents;
    for(const auto& e : m_grid[cellId]) {
      if(e.type == wanted) {
        ents.emplace_back(e);
      }
    }
    return ents;
  }
};
