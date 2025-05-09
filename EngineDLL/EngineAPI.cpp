
#ifndef EDITOR_INTERFACE
#define EDITOR_INTERFACE extern "C" __declspec(dllexport)
#endif // !EDITOR_INTERFACE

#include "CommonHeaders.h"
#include "Id.h"
#include "..\Engine\Components\Entity.h"
#include "..\Engine\Components\Transform.h"

using namespace dossyl;

namespace {
	struct TransformComponent { //
		f32 position[3];
		f32 rotation[3];
		f32 scale[3];

		auto [[nodiscard]] toInitInfo() -> transform::InitInfo {
			using namespace DirectX;
			transform::InitInfo info{};
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
	struct GameEntityDescriptor {
		TransformComponent transform;
	};

	gameEntity::Entity entityFromId(id::IdType id) {
		return gameEntity::Entity{ gameEntity::EntityId{id} };
	}
} // anonymous namespace

EDITOR_INTERFACE auto CreateGameEntity(GameEntityDescriptor* e) -> id::IdType {
	assert(e);
	GameEntityDescriptor& desc{ *e };
	transform::InitInfo transformInfo{ desc.transform.toInitInfo() };
	gameEntity::EntityInfo entityInfo { &transformInfo };
	return gameEntity::createGameEntity(entityInfo).getId();
}

EDITOR_INTERFACE auto RemoveGameEntity(id::IdType id) -> void {
	assert(id::isValid(id));
	gameEntity::removeGameEntity(entityFromId(id));
}
