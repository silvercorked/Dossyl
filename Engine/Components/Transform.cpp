
#include "Transform.h"
#include "Entity.h"

namespace dossyl::transform {
	namespace {

		util::vector<math::v3> positions;
		util::vector<math::v4> rotations;
		util::vector<math::v3> scales;

	} // anonymous namespace

	auto create(InitInfo info, gameEntity::Entity entity) -> Component {
		assert(entity.isValid());
		const id::IdType entityIndex { id::index(entity.getId()) };
		if (positions.size() > entityIndex) {
			positions[entityIndex] = math::v3(info.position);
			rotations[entityIndex] = math::v4(info.rotation);
			scales[entityIndex] = math::v3(info.scale);
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

	auto Component::position() const->math::v3 {
		assert(isValid());
		return positions[id::index(_id)];
	}
	auto Component::rotation() const->math::v4 {
		assert(isValid());
		return rotations[id::index(_id)];
	}
	auto Component::scale() const->math::v3 {
		assert(isValid());
		return scales[id::index(_id)];
	}
}
