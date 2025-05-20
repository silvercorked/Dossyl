#pragma once

#include "ComponentsCommon.h"

namespace Dossyl::Transform {
	
	struct InitInfo {
		f32 position[3]{};
		f32 rotation[4]{};
		f32 scale[3]{ 1.f, 1.f, 1.f };
	};

	auto create(InitInfo info, GameEntity::Entity entity) -> Component;
	auto remove(Component c) -> void;
}
