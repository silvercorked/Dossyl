#pragma once

#include "ComponentsCommon.h"

namespace dossyl {

#define INIT_INFO(component) namespace component { struct initInfo; }
	INIT_INFO(transform); // forward declaration from transform namespace (transform.h)
#undef INIT_INFO
	namespace gameEntity {
		struct EntityInfo {
			transform::initInfo* transform{ nullptr };
		};

		EntityId createGameEntity(const EntityInfo& info);
		void removeGameEntity(EntityId id);
		bool isAlive(EntityId id);
	}
}
