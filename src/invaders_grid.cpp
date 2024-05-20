#include "invaders_grid.h"

#include <algorithm>

namespace Game {
  using namespace Math;
  using namespace Ev;

  GridManager::GridManager(const int sceneWidth,
			   const int sceneHeight,
			   EventManager& eventManager)
    : m_eventManager{ eventManager }
  {
    m_rows = std::ceil(sceneWidth  / pixelsToWorld(CELL_WIDTH));
    m_cols = std::ceil(sceneHeight / pixelsToWorld(CELL_HEIGHT));
    m_grid.resize(m_rows * m_cols);
    for(auto& v : m_grid) {
      v.resize(8);
    }
    m_eventManager.subscribe(EventType::AlienDestroyed, [this](const Event& event) {
      auto data = event.getData();
      if(std::holds_alternative<Alien*>(data)) {
	const auto* a = std::get<Alien*>(data);
	const auto cellId = getId(a->m_pos);
	auto it = std::remove_if(m_grid[cellId].begin(), m_grid[cellId].end(), [a](const EntityGridData& arg) {
	  return a == arg.data;
	});
	m_grid[cellId].erase(it);
      }
    });
    m_eventManager.subscribe(EventType::PlayerDestroyed, [this](const Event& event) {
      auto data = event.getData();
      if(std::holds_alternative<Player*>(data)) {
	const auto* p = std::get<Player*>(data);
	const auto cellId = getId(p->m_pos);
	auto it = std::remove_if(m_grid[cellId].begin(), m_grid[cellId].end(), [p](const EntityGridData& arg) {
	  return p == arg.data;
	});
	m_grid[cellId].erase(it);
      }
    });
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

  void GridManager::update(const v3& pos, void* ent, const EntityType type)
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
