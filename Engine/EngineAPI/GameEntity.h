#pragma once

#include "../Components/ComponentsCommon.h"

#include "TransformComponent.h"
#include "ScriptComponent.h"
#include <string>

namespace dossyl {
	namespace gameEntity {

		DEFINE_TYPED_ID(EntityId);

		class Entity {
			EntityId _id;
		public:
			constexpr explicit Entity(EntityId id) : _id{ id } {}
			constexpr Entity() : _id{ id::invalidId } {}
			constexpr EntityId getId() const { return _id; }
			constexpr bool isValid() const { return id::isValid(_id); }

			auto transform() const -> transform::Component;
			auto script() const -> script::Component;
		};
	} // namespace gameEntity

	namespace script {
		class EntityScript : public gameEntity::Entity {
		public:
			virtual ~EntityScript() = default;
			virtual auto beginPlay() -> void {}
			virtual auto update(float) -> void {}
		protected:
			constexpr explicit EntityScript(gameEntity::Entity entity) :
				gameEntity::Entity{ entity.getId() }
			{}
		};
		namespace detail {
			using script_ptr = std::unique_ptr<EntityScript>;
			using script_creator = script_ptr(*)(gameEntity::Entity entity);
			using string_hash = std::hash<std::string>;

			auto registerScript(size_t, script_creator) -> u8;

			template <typename ScriptClass>
			auto createScript(gameEntity::Entity entity) -> script_ptr {
				assert(entity.isValid());
				return std::make_unique<ScriptClass>(entity);
			}

#define REGISTER_SCRIPT(TYPE)																			\
		class TYPE;																						\
		namespace {																						\
			const u8 _register##TYPE{																	\
				dossyl::script::detail::registerScript(													\
					dossyl::script::detail::string_hash{}(#TYPE),										\
					&dossyl::script::detail::createScript<TYPE>											\
				)																						\
			};																							\
		}
// end define REGISTER_SCRIPT
		} // namespace detail
	} // namespace script
}
