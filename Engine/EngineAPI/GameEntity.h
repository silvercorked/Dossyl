#pragma once

#include "../Components/ComponentsCommon.h"

#include "TransformComponent.h"
#include "ScriptComponent.h"
#include <string>

namespace Dossyl {
	namespace GameEntity {

		DEFINE_TYPED_ID(EntityId);

		class Entity {
			EntityId _id;
		public:
			constexpr explicit Entity(EntityId id) : _id{ id } {}
			constexpr Entity() : _id{ Id::invalidId } {}
			constexpr EntityId getId() const { return _id; }
			constexpr bool isValid() const { return Id::isValid(_id); }

			auto transform() const -> Transform::Component;
			auto script() const -> Script::Component;
		};
	} // namespace GameEntity

	namespace Script {
		class EntityScript : public GameEntity::Entity {
		public:
			virtual ~EntityScript() = default;
			virtual auto beginPlay() -> void {}
			virtual auto update(float) -> void {}
		protected:
			constexpr explicit EntityScript(GameEntity::Entity entity) :
				GameEntity::Entity{ entity.getId() }
			{}
		};
		namespace Detail {
			using script_ptr = std::unique_ptr<EntityScript>;
			using script_creator = script_ptr(*)(GameEntity::Entity entity);
			using string_hash = std::hash<std::string>;

			auto registerScript(size_t, script_creator) -> u8;

			/*
				getScriptCreator, addScriptName, and scriptNames are all used by the level editor to get
				script names into the level editor so users can more easily interact w/ them.
				This engine project builds into a dll which is included into both EngineDll.dll
				and GameCode.dll.
				Without the editor, no script names are needed and the dll situation is different,
				but with the level editor, EngineDll.dll contains engine code and data on game objects.
				But GameCode.dll contains GameCode and functions for instantiating scripts.
				So these functions provide data which will be exchanged in the level editor to eachother.
				Without the level editor, script creator data is in the same dll so need multiple definitions.
				1 for editor and 1 for build
			*/
#ifdef USE_WITH_EDITOR
			extern "C" __declspec(dllexport)
#endif
			auto getScriptCreator(size_t) -> script_creator;

			template <typename ScriptClass>
			auto createScript(GameEntity::Entity entity) -> script_ptr {
				assert(entity.isValid());
				return std::make_unique<ScriptClass>(entity);
			}

#ifdef USE_WITH_EDITOR
			auto addScriptName(const char*) -> u8;

#define REGISTER_SCRIPT(TYPE)																			\
		namespace {																						\
			const u8 _register##TYPE{																	\
				Dossyl::Script::Detail::registerScript(													\
					Dossyl::Script::Detail::string_hash{}(#TYPE),										\
					&Dossyl::Script::Detail::createScript<TYPE>											\
				)																						\
			};																							\
			const u8 _name##TYPE{																		\
				Dossyl::Script::Detail::addScriptName(#TYPE)											\
			};																							\
		}
// end define REGISTER_SCRIPT
#else
#define REGISTER_SCRIPT(TYPE)																			\
		namespace {																						\
			const u8 _register##TYPE{																	\
				Dossyl::Script::Detail::registerScript(													\
					Dossyl::Script::Detail::string_hash{}(#TYPE),										\
					&Dossyl::Script::Detail::createScript<TYPE>											\
				)																						\
			};																							\
		}
// end define REGISTER_SCRIPT
#endif // end USE_WITH_EDITOR
		} // namespace Detail
	} // namespace Script
}
