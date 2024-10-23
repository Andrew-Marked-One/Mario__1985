#include "EntityManager.h"

void EntityManager::removeDeadEntities(EntityVec& vec) {

}

EntityManager::EntityManager() {}
void EntityManager::update() {
	for (auto& e : m_entitiesToAdd) {
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}

	for (auto it = m_entities.begin(); it != m_entities.end(); ) {
		if (!((*it)->isActive())) {
			size_t idToKill = (*it)->id();
			for (auto& pair : m_entityMap) {
				if (pair.first == (*it)->tag()) {
					auto& vec = pair.second;
					for (auto vecIt = vec.begin(); vecIt != vec.end(); vecIt++) {
						if ((*vecIt)->id() == idToKill) {
							vecIt = vec.erase(vecIt);
							break;
						}
					}
					break;
				}
			}
			it = m_entities.erase(it);
		}
		else {
			it++;
		}
	}
	m_entitiesToAdd.clear();
}
std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
	auto e = std::make_shared<Entity>(m_totalEntities++, tag);
	m_entitiesToAdd.push_back(e);
	return e;
}
EntityVec& EntityManager::getEntities() {
	return m_entities;
}
EntityVec& EntityManager::getEntities(const std::string& tag) {
	return m_entityMap[tag];
}
