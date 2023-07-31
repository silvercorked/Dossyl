#pragma once

#include "../Components/ComponentsCommon.h"

#include "TransformComponent.h"

namespace dossyl::gameEntity {

	DEFINE_TYPED_ID(EntityId);

	class Entity {
		EntityId _id;
	public:
		constexpr explicit Entity(EntityId id) : _id{ id } {}
		constexpr Entity() : _id{ id::invalidId } {}
		constexpr EntityId getId() const { return _id; }
		constexpr bool isValid() const { return id::isValid(_id); }

		auto transform() const -> transform::Component;
	};
}
