
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

#ifdef USE_WITH_EDITOR
		auto scriptNames() -> Util::vector<std::string>& {
			/* NOTE: static var placed in function because initialization order of static data
					 this assures the script_registery is defined beforea class is registered.
			*/
			static Util::vector<std::string> names;
			return names;
		}
#endif // end USE_WITH_EDITOR

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

		auto getScriptCreator(size_t tag) -> script_creator {
			auto script = Dossyl::Script::registery().find(tag);
			assert(script != Dossyl::Script::registery().end() && script->first == tag);
			return script->second;
		}

#ifdef USE_WITH_EDITOR
		auto addScriptName(const char* name) -> u8 {
			scriptNames().emplace_back(name);
			return true;
		}
#endif // end USE_WITH_EDITOR
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

#ifdef USE_WITH_EDITOR
#include <atlsafe.h>

extern "C" __declspec(dllexport)
auto getScriptNames() -> LPSAFEARRAY {
	// CComSafeArray takes a count type of unsigned long
	const unsigned long size{ static_cast<unsigned long>(Dossyl::Script::scriptNames().size()) };
	if (size == 0) return nullptr;
	CComSafeArray<BSTR> names{ size };
	for (auto i{ 0 }; i < size; ++i) { // copy data to BSTR format which .NET can use
		names.SetAt(i, A2BSTR_EX(Dossyl::Script::scriptNames()[i].c_str()), false);
	}
	return names.Detach();
}
#endif