
#include "Common.h"

#include "CommonHeaders.h"
#include "Id.h"
#include "..\Engine\Components\Entity.h"
#include "..\Engine\Components\Transform.h"
#include "..\Engine\Components\Script.h"

using namespace Dossyl;

namespace {
	struct TransformComponent {
		f32 position[3];
		f32 rotation[3];
		f32 scale[3];

		auto [[nodiscard]] toInitInfo() -> Transform::InitInfo {
			using namespace DirectX;
			Transform::InitInfo info{};
			memcpy(&info.position[0], &position[0], sizeof(f32) * _countof(position));
			memcpy(&info.scale[0], &scale[0], sizeof(f32) * _countof(scale));
			XMFLOAT3A rot{ &rotation[0] };
			XMVECTOR quat{ XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3A(&rot)) };
			XMFLOAT4A rotQuat{};
			XMStoreFloat4A(&rotQuat, quat);
			memcpy(&info.rotation[0], &rotQuat.x, sizeof(f32) * _countof(info.rotation));
			return info;
		}
	};
	struct ScriptComponent {
		Script::Detail::script_creator scriptCreator;
		auto toInitInfo() const -> Script::InitInfo {
			Script::InitInfo info{};
			info.scriptCreator = this->scriptCreator;
			return info;
		}
	};
	struct GameEntityDescriptor {
		TransformComponent transform;
		ScriptComponent script;
	};

	GameEntity::Entity entityFromId(Id::IdType id) {
		return GameEntity::Entity{ GameEntity::EntityId{id} };
	}
} // anonymous namespace

EDITOR_INTERFACE auto CreateGameEntity(GameEntityDescriptor* e) -> Id::IdType {
	assert(e);
	GameEntityDescriptor& desc{ *e };
	Transform::InitInfo transformInfo{ desc.transform.toInitInfo() };
	Script::InitInfo scriptInfo{ desc.script.toInitInfo() };
	GameEntity::EntityInfo entityInfo{
		&transformInfo,
		&scriptInfo
	};
	return GameEntity::create(entityInfo).getId();
}

EDITOR_INTERFACE auto RemoveGameEntity(Id::IdType id) -> void {
	assert(Id::isValid(id));
	GameEntity::remove(GameEntity::EntityId{ id });
}
