#pragma once

#include "ComponentsCommon.h"

namespace dossyl::transform {
	
	struct InitInfo {
		f32 position[3]{};
		f32 rotation[4]{};
		f32 scale[3]{ 1.f, 1.f, 1.f };
	};

	auto createTransform(const InitInfo& info, gameEntity::Entity entity) -> Component;
	auto removeTransform(Component c) -> void;
}
