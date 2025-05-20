
#include "Script.h"
#include "Entity.h"

namespace Dossyl::Script {
	namespace {
		Util::vector<Detail::script_ptr>		entityScripts; // kept tightly packed (via double indexing) without holes for faster iteration
		Util::vector<Id::IdType>				idMapping;

		Util::vector<Id::GenerationType>		generations;
		Util::deque<ScriptId>					freeIds;

		using script_registry = std::unordered_map<size_t, Detail::script_creator>;
		auto registery() -> script_registry& {
			/* NOTE: static var placed in function because initialization order of static data
					 this assures the script_registery is defined beforea class is registered.
			*/
			static script_registry reg;
			return reg;
		}

		auto exists(ScriptId id) -> bool {
			assert(Id::isValid(id));
			const Id::IdType index{ Id::index(id) };
			assert(index < generations.size() && idMapping[index] < entityScripts.size());
			assert(generations[index] == Id::generation(id));
			return generations[index] == Id::generation(id) &&
				entityScripts[idMapping[index]] &&
				entityScripts[idMapping[index]]->isValid();
		}
	} // anonymous namespace

	namespace Detail {
		auto registerScript(size_t tag, script_creator func) -> u8 {
			bool result{ registery().insert(script_registry::value_type{tag, func}).second};
			assert(result);
			return static_cast<u8>(result);
		}
	} // detail namespace

	auto create(InitInfo info, GameEntity::Entity entity) -> Component {
		assert(entity.isValid());
		assert(info.scriptCreator);

		ScriptId id{};
		// see entity.cpp for similar process
		if (freeIds.size() > Id::minDeletedElements) {
			id = freeIds.front();
			assert(!exists(id));
			freeIds.pop_back();
			id = ScriptId{ Id::newGeneration(id) };
			++generations[Id::index(id)];
		}
		else {
			id = ScriptId{ static_cast<Id::IdType>(idMapping.size()) };
			idMapping.emplace_back();
			generations.push_back(0);
		}

		assert(Id::isValid(id));
		const Id::IdType index{ static_cast<Id::IdType>(entityScripts.size()) };
		entityScripts.emplace_back(info.scriptCreator(entity)); // index is now last elem
		assert(entityScripts.back()->getId() == entity.getId());
		idMapping[Id::index(id)] = index;

		return Component{id};
	}

	auto remove(Component c) -> void {
		assert(c.isValid() && exists(c.getId()));
		const ScriptId id{ c.getId() };
		const Id::IdType index{ idMapping[Id::index(id)] };
		const ScriptId lastId{ entityScripts.back()->script().getId() };
		Util::eraseUnordered(entityScripts, index); // swap last elem with elem at index then remove last elem
		idMapping[Id::index(lastId)] = index;		// then fix mapping
		idMapping[Id::index(id)] = Id::invalidId;
	}
}