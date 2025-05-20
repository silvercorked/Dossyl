#pragma once

#include "../Components/ComponentsCommon.h"

namespace Dossyl::Script {

	DEFINE_TYPED_ID(ScriptId);

	class Component final {
		ScriptId _id;
	public:
		constexpr explicit Component(ScriptId id) : _id{ id } {}
		constexpr Component() : _id{ Id::invalidId } {}
		constexpr ScriptId getId() const { return _id; }
		constexpr bool isValid() const { return Id::isValid(_id); }
	};
}
