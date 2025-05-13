
#include "Script.h"
#include "Entity.h"

namespace dossyl::script {
	namespace {
		util::vector<detail::script_ptr>		entityScripts; // kept tightly packed (via double indexing) without holes for faster iteration
		util::vector<id::IdType>				idMapping;

		util::vector<id::GenerationType>		generations;
		util::deque<ScriptId>					freeIds;

		using script_registry = std::unordered_map<size_t, detail::script_creator>;
		auto registery() -> script_registry& {
			/* NOTE: static var placed in function because initialization order of static data
					 this assures the script_registery is defined beforea class is registered.
			*/
			static script_registry reg;
			return reg;
		}

		auto exists(ScriptId id) -> bool {
			assert(id::isValid(id));
			const id::IdType index{ id::index(id) };
			assert(index < generations.size() && idMapping[index] < entityScripts.size());
			assert(generations[index] == id::generation(id));
			return generations[index] == id::generation(id) &&
				entityScripts[idMapping[index]] &&
				entityScripts[idMapping[index]]->isValid();
		}
	} // anonymous namespace

	namespace detail {
		auto registerScript(size_t tag, script_creator func) -> u8 {
			bool result{ registery().insert(script_registry::value_type{tag, func}).second};
			assert(result);
			return static_cast<u8>(result);
		}
	} // detail namespace

	auto create(InitInfo info, gameEntity::Entity entity) -> Component {
		assert(entity.isValid());
		assert(info.scriptCreator);

		ScriptId id{};
		// see entity.cpp for similar process
		if (freeIds.size() > id::minDeletedElements) {
			id = freeIds.front();
			assert(!exists(id));
			freeIds.pop_back();
			id = ScriptId{ id::newGeneration(id) };
			++generations[id::index(id)];
		}
		else {
			id = ScriptId{ static_cast<id::IdType>(idMapping.size()) };
			idMapping.emplace_back();
			generations.push_back(0);
		}

		assert(id::isValid(id));
		const id::IdType index{ static_cast<id::IdType>(entityScripts.size()) };
		entityScripts.emplace_back(info.scriptCreator(entity)); // index is now last elem
		assert(entityScripts.back()->getId() == entity.getId());
		idMapping[id::index(id)] = index;

		return Component{id};
	}

	auto remove(Component c) -> void {
		assert(c.isValid() && exists(c.getId()));
		const ScriptId id{ c.getId() };
		const id::IdType index{ idMapping[id::index(id)] };
		const ScriptId lastId{ entityScripts.back()->script().getId() };
		util::eraseUnordered(entityScripts, index); // swap last elem with elem at index then remove last elem
		idMapping[id::index(lastId)] = index;		// then fix mapping
		idMapping[id::index(id)] = id::invalidId;
	}
}