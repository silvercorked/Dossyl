#pragma once

#include "ComponentsCommon.h"

namespace Dossyl::Script {

	struct InitInfo {
		Detail::script_creator scriptCreator;
	};

	auto create(InitInfo info, GameEntity::Entity entity) -> Component;
	auto remove(Component c) -> void;
}
