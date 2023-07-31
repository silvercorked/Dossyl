#pragma once

#include "../Components/ComponentsCommon.h"

namespace dossyl::transform {

	DEFINE_TYPED_ID(TransformId);

	class Component final {
		TransformId _id;
	public:
		constexpr explicit Component(TransformId id) : _id{ id } {}
		constexpr Component() : _id{ id::invalidId } {}
		constexpr TransformId getId() const { return _id; }
		constexpr bool isValid() const { return id::isValid(_id); }

		auto position() const -> math::v3;
		auto rotation() const -> math::v4;
		auto scale() const -> math::v3;
	};
}
