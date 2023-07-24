#pragma once

#include "CommonHeaders.h"

// http://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html
namespace dossyl::id {
	using IdType = u32;

	constexpr u32 generationBits{ 8 };
	constexpr u32 indexBits{ sizeof(IdType) * 8 - generationBits };

	constexpr IdType generationMask{ (IdType{1} << generationBits) - 1 };
	constexpr IdType indexMask{ (IdType{1} << indexBits) - 1 }; // 0x0fff'ffff w/ generationBits at 8

	constexpr IdType idMask{ IdType{-1} }; // all bits set, also invalid value for an id

	using GenerationType = std::conditional_t<generationBits <= 16, std::conditional_t<generationBits <= 8, u8, u16>, u32>;

	static_assert(sizeof(GenerationType) * 8 >= generationBits); // cant need more bits than storage type can hold, ie. >= 32 bits
	static_assert(sizeof(IdType) - sizeof(GenerationType) > 0);

	inline bool isValid(IdType id) {
		return id != idMask;
	}
	inline IdType index(IdType id) {
		return id & indexMask;
	}
	inline GenerationType generation(IdType id) {
		return (id >> indexBits) & generationMask;
	}
	inline IdType newGeneration(IdType id) {
		const IdType generation{ id::generation(id) + 1 };
		assert(generation < (GenerationType{0} - 1));
		return index(id) | (generation << indexBits);
	}

#if _DEBUG
	namespace internal {
		class IdBase { // debug wrapper
			IdType _id;
		public:
			constexpr explicit IdBase(IdType id) : _id{ id } {}
			constexpr operator IdType() const { return _id; } // u32 i = entityId; // this func used here to give the IdType rather than the wrapper
		};
	}
#define DEFINE_TYPED_ID(name)											\
	struct name final : dossyl::id::internal::IdBase {							\
		constexpr explicit name(dossyl::id::IdType id) : IdBase{ id } {}		\
		constexpr name() : IdBase { dossyl::id::idMask } {}						\
	};
#else
#define DEFINE_TYPED_ID(name) using name = id::IdType;
#endif
}
