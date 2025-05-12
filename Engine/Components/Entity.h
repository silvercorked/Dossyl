#pragma once

#include "ComponentsCommon.h"

namespace dossyl {

#define INIT_INFO(component) namespace component { struct InitInfo; }

	INIT_INFO(transform); // forward declaration from transform namespace (transform.h)
	INIT_INFO(script);

#undef INIT_INFO
	namespace gameEntity {
		struct EntityInfo {
			transform::InitInfo* transform{ nullptr };
			script::InitInfo* script{ nullptr };
		};

		auto create(EntityInfo info) -> Entity;
		auto remove(EntityId id) -> void;
		auto isAlive(EntityId id) -> bool;
	}
}
