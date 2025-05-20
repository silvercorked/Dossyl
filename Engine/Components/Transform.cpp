
#include "Transform.h"
#include "Entity.h"

namespace Dossyl::Transform {
	namespace {

		Util::vector<Math::v3> positions;
		Util::vector<Math::v4> rotations;
		Util::vector<Math::v3> scales;

	} // anonymous namespace

	auto create(InitInfo info, GameEntity::Entity entity) -> Component {
		assert(entity.isValid());
		const Id::IdType entityIndex { Id::index(entity.getId()) };
		if (positions.size() > entityIndex) {
			positions[entityIndex] = Math::v3(info.position);
			rotations[entityIndex] = Math::v4(info.rotation);
			scales[entityIndex] = Math::v3(info.scale);
		}
		else {
			assert(positions.size() == entityIndex);
			positions.emplace_back(info.position);
			rotations.emplace_back(info.rotation);
			scales.emplace_back(info.scale);
		}
		return Component(TransformId{ entity.getId() });
	}
	auto remove(Component c) -> void {
		assert(c.isValid());
	}

	auto Component::position() const->Math::v3 {
		assert(isValid());
		return positions[Id::index(_id)];
	}
	auto Component::rotation() const->Math::v4 {
		assert(isValid());
		return rotations[Id::index(_id)];
	}
	auto Component::scale() const->Math::v3 {
		assert(isValid());
		return scales[Id::index(_id)];
	}
}
