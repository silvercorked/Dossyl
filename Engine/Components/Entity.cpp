
#include "Entity.h"
#include "Transform.h"
#include "Script.h"

namespace Dossyl::GameEntity {
	namespace { // anonymous namespace

		Util::vector<Transform::Component>		transforms;
		Util::vector<Script::Component>			scripts;

		Util::vector<Id::GenerationType>		generations;
		Util::deque<EntityId>					freeIds;
	}

	/*
		Game entity ids exist in 3 states:
			in use: has an entry in the generations list
			deleted: has been used before, has been removed, and is ready for reuse, and has an entry in freeIds
			non-existant: does not yet exist and has never been used (not stored in a list)
	*/
	auto create(EntityInfo info) -> Entity {
		assert(info.transform); // all game entities must have a transform component
		if (!info.transform) return Entity();
		EntityId id;
		if (freeIds.size() > Id::minDeletedElements) { // ok to pull id from freed ids deque
			id = freeIds.front();
			assert(!isAlive(id));
			freeIds.pop_front();
			id = EntityId{ Id::newGeneration(id) };
			++generations[Id::index(id)];
			// no need to affect transform, as there should already exist an invalid component in its place
		}
		else { // just add to generations since havent deleted enough elems to reuse yet
			id = EntityId{ (Id::IdType) generations.size() };
			generations.push_back(0); // add zero-th generation to end of generations

			transforms.emplace_back(); // add default component to end of transforms (and grab extra memory for later)
			// could use transforms.resize(generations.size) but will result in more memory grabs, which is expensive
			scripts.emplace_back();
		}
		const Entity nEntity{ id };
		const Id::IdType index{ Id::index(id) };

		// create transform component
		assert(!transforms[index].isValid());
		transforms[index] = Transform::create(*info.transform, nEntity);
		if (!transforms[index].isValid()) return Entity{};

		// Create script component
		if (info.script && info.script->scriptCreator) {
			assert(!scripts[index].isValid());
			scripts[index] = Script::create(*info.script, nEntity);
			assert(scripts[index].isValid());
		}

		return nEntity;
	}
	auto remove(EntityId id) -> void {
		const Id::IdType index{ Id::index(id) };
		assert(isAlive(id));

		if (scripts[index].isValid()) {
			Script::remove(scripts[index]);
			scripts[index] = {};
		}

		Transform::remove(transforms[index]);
		transforms[index] = Transform::Component{};
		freeIds.push_back(id);
	}
	auto isAlive(EntityId id) -> bool {
		assert(Id::isValid(id));
		const Id::IdType index {Id::index(id)};
		assert(index < generations.size()); // generations holds an elem for each alive elem, so if bigger, cant be alive
		assert(generations[index] == Id::generation(id)); // check that generation portion is the same in generations and the given entity
		return (
			generations[index] == Id::generation(id)
			&& transforms[index].isValid()
		);
	}

	auto Entity::transform() const -> Transform::Component {
		assert(isAlive(this->_id));
		const Id::IdType index { Id::index(this->_id) };
		return transforms[index];
	}

	auto Entity::script() const -> Script::Component {
		assert(isAlive(this->_id));
		const Id::IdType index{ Id::index(this->_id) };
		return scripts[index];
	}
}
