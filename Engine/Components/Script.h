#pragma once

#include "ComponentsCommon.h"

namespace dossyl::script {

	struct InitInfo {
		detail::script_creator scriptCreator;
	};

	auto create(InitInfo info, gameEntity::Entity entity) -> Component;
	auto remove(Component c) -> void;
}
