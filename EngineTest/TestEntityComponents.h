#pragma once

#include "Test.h"
#include "..\Engine\Components\Entity.h"
#include "..\Engine\Components\Transform.h"

#include <iostream>
#include <ctime>

using namespace Dossyl;

class EngineTest : public Test {
	Util::vector<GameEntity::Entity> _entities;
	u32 _added{ 0 };
	u32 _removed{ 0 };
	u32 _numEntities{ 0 };
public:
	bool initialize() override {
		srand((u32)time(nullptr));
		std::cout << "Initializing" << std::endl;
		return true;
	}
	void run() override {
		do {
			for (u32 i = 0; i < 10000; i++) {
				createRandom();
				removeRandom();
				_numEntities = (u32)_entities.size();
			}
			printResults();
		} while (getchar() != 'q');
	}
	void shutdown() override {
	
	}
private:
	void createRandom() {
		u32 count = rand() % 20;
		if (_entities.empty()) count = 1000;
		Transform::InitInfo transformInfo{};
		GameEntity::EntityInfo entityInfo {
			&transformInfo
		};
		while (count > 0) {
			_added++;
			GameEntity::Entity entity {GameEntity::create(entityInfo)};
			assert(entity.isValid() && Id::isValid(entity.getId()));
			_entities.push_back(entity);
			assert(GameEntity::isAlive(entity.getId()));
			count--;
		}
	}
	void removeRandom() {
		u32 count = rand() % 20;
		if (_entities.size() < 1000) return;
		while (count > 0) {
			const u32 index{ (u32) rand() % (u32) _entities.size() };
			const GameEntity::Entity entity {_entities[index]};
			assert(entity.isValid() && Id::isValid(entity.getId()));
			if (entity.isValid()) {
				GameEntity::remove(entity.getId());
				_entities.erase(_entities.begin() + index);
				assert(!GameEntity::isAlive(entity.getId()));
				_removed++;
			}
			count--;
		}
	}
	void printResults() {
		std::cout << "Entities created: " << _added << std::endl;
		std::cout << "Entities deleted: " << _removed << std::endl;
	}
};
