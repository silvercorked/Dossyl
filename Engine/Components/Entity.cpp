
#include "Entity.h"
#include "Transform.h"

namespace dossyl::gameEntity {
	namespace { // anonymous namespace

		util::vector<transform::Component>		transforms;

		util::vector<id::GenerationType>		generations;
		util::deque<EntityId>					freeIds;
	}

	/*
		Game entity ids exist in 3 states:
			in use: has an entry in the generations list
			deleted: has been used before, has been removed, and is ready for reuse, and has an entry in freeIds
			non-existant: does not yet exist and has never been used (not stored in a list)
	*/
	auto createGameEntity(const EntityInfo& info) -> Entity {
		assert(info.transform); // all game entities must have a transform component
		if (!info.transform) return Entity();
		EntityId id;
		if (freeIds.size() > id::minDeletedElements) { // ok to pull id from freed ids deque
			id = freeIds.front();
			assert(!isAlive(Entity{ id }));
			freeIds.pop_front();
			id = EntityId{ id::newGeneration(id) };
			++generations[id::index(id)];
			// no need to affect transform, as there should already exist an invalid component in its place
		}
		else { // just add to generations since havent deleted enough elems to reuse yet
			id = EntityId{ (id::IdType) generations.size() };
			generations.push_back(0); // add zero-th generation to end of generations

			transforms.emplace_back(); // add default component to end of transforms (and grab extra memory for later)
			// could use transforms.resize(generations.size) but will result in more memory grabs, which is expensive
		}
		const Entity nEntity{ id };
		const id::IdType index{ id::index(id) };

		// create transform component
		assert(!transforms[index].isValid());
		transforms[index] = transform::createTransform(*info.transform, nEntity);
		if (!transforms[index].isValid()) return Entity{};

		return nEntity;
	}
	auto removeGameEntity(Entity e) -> void {
		const EntityId id{ e.getId() };
		const id::IdType index{ id::index(id) };
		assert(isAlive(e));
		if (isAlive(e)) {
			transform::removeTransform(transforms[index]);
			transforms[index] = transform::Component{};
			freeIds.push_back(id);
		}
	}
	auto isAlive(Entity e) -> bool {
		const EntityId id{ e.getId() };
		const id::IdType index {id::index(id)};
		assert(index < generations.size()); // generations holds an elem for each alive elem, so if bigger, cant be alive
		assert(generations[index] == id::generation(id)); // check that generation portion is the same in generations and the given entity
		return (
			generations[index] == id::generation(id)
			&& transforms[index].isValid()
		);
	}

	auto Entity::transform() const -> transform::Component {
		assert(isAlive(*this));
		const id::IdType index {id::index(this->_id) };
		return transforms[index];
	}
}
