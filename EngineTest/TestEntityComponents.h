#pragma once

#include "Test.h"
#include "..\Engine\Components\Entity.h"
#include "..\Engine\Components\Transform.h"

#include <iostream>
#include <ctime>

using namespace dossyl;

class EngineTest : public Test {
	util::vector<gameEntity::Entity> _entities;
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
		transform::InitInfo transformInfo{};
		gameEntity::EntityInfo entityInfo {
			&transformInfo
		};
		while (count > 0) {
			_added++;
			gameEntity::Entity entity {gameEntity::createGameEntity(entityInfo)};
			assert(entity.isValid() && id::isValid(entity.getId()));
			_entities.push_back(entity);
			assert(gameEntity::isAlive(entity));
			count--;
		}
	}
	void removeRandom() {
		u32 count = rand() % 20;
		if (_entities.size() < 1000) return;
		while (count > 0) {
			const u32 index{ (u32) rand() % (u32) _entities.size() };
			const gameEntity::Entity entity {_entities[index]};
			assert(entity.isValid() && id::isValid(entity.getId()));
			if (entity.isValid()) {
				gameEntity::removeGameEntity(entity);
				_entities.erase(_entities.begin() + index);
				assert(!gameEntity::isAlive(entity));
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
