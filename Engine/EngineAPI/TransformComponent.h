#pragma once

#include "../Components/ComponentsCommon.h"

namespace Dossyl::Transform {

	DEFINE_TYPED_ID(TransformId);

	class Component final {
		TransformId _id;
	public:
		constexpr explicit Component(TransformId id) : _id{ id } {}
		constexpr Component() : _id{ Id::invalidId } {}
		constexpr TransformId getId() const { return _id; }
		constexpr bool isValid() const { return Id::isValid(_id); }

		auto position() const -> Math::v3;
		auto rotation() const -> Math::v4;
		auto scale() const -> Math::v3;
	};
}
