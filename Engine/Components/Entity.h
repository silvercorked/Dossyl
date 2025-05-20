#pragma once

#include "ComponentsCommon.h"

namespace Dossyl {

#define INIT_INFO(Component) namespace Component { struct InitInfo; }

	INIT_INFO(Transform); // forward declaration from transform namespace (transform.h)
	INIT_INFO(Script);

#undef INIT_INFO
	namespace GameEntity {
		struct EntityInfo {
			Transform::InitInfo* transform{ nullptr };
			Script::InitInfo* script{ nullptr };
		};

		auto create(EntityInfo info) -> Entity;
		auto remove(EntityId id) -> void;
		auto isAlive(EntityId id) -> bool;
	}
}
