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

			template <typename ScriptClass>
			auto createScript(GameEntity::Entity entity) -> script_ptr {
				assert(entity.isValid());
				return std::make_unique<ScriptClass>(entity);
			}

#define REGISTER_SCRIPT(TYPE)																			\
		class TYPE;																						\
		namespace {																						\
			const u8 _register##TYPE{																	\
				Dossyl::Script::Detail::registerScript(													\
					Dossyl::Script::Detail::string_hash{}(#TYPE),										\
					&Dossyl::Script::Detail::createScript<TYPE>											\
				)																						\
			};																							\
		}
// end define REGISTER_SCRIPT
		} // namespace Detail
	} // namespace Script
}
