#pragma once

#include "ComponentsCommon.h"

namespace dossyl {

#define INIT_INFO(component) namespace component { struct InitInfo; }
	INIT_INFO(transform); // forward declaration from transform namespace (transform.h)
#undef INIT_INFO
	namespace gameEntity {
		struct EntityInfo {
			transform::InitInfo* transform{ nullptr };
		};

		auto createGameEntity(const EntityInfo& info) -> Entity;
		auto removeGameEntity(Entity id) -> void;
		auto isAlive(Entity id) -> bool;
	}
}
