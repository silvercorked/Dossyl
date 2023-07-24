#pragma once

#include "ComponentsCommon.h"

namespace dossyl::transform {
	DEFINE_TYPED_ID(TransformId);

	struct initInfo {
		f32 position[3]{};
		f32 rotation[4]{};
		f32 scale[3]{ 1.f, 1.f, 1.f };
	};

	TransformId createTransform(const initInfo& info, gameEntity::EntityId entityId);
	void removeTransform(TransformId id);
}
