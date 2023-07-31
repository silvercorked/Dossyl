#pragma once

#include "CommonHeaders.h"

// http://bitsquid.blogspot.com/2014/08/building-data-oriented-entity-system.html

/*
	why these indecies and not pointers?
	pointers can become invalid if reallocated, indecies do not.
	when items disappear, indecies become invalid, but not if the array has holes
	(ie, replaced destroyed members with null and skips).
	(also, 4 byte indecies vs 8 byte pointers)
*/
namespace dossyl::id {
	using IdType = u32;

	namespace internal {
		constexpr u32 generationBits{ 8 };
		constexpr u32 indexBits{ sizeof(IdType) * 8 - generationBits };

		constexpr IdType generationMask{ (IdType{1} << generationBits) - 1 };
		constexpr IdType indexMask{ (IdType{1} << indexBits) - 1 }; // 0x0fff'ffff w/ generationBits at 8
	} // internal namespace

	constexpr IdType invalidId{ (IdType)-1 }; // all bits set, also invalid value for an id
	// invalid id is max id possible, so -1 cast to idType since idTypes wrap unsigned ints
	constexpr u32 minDeletedElements{ 1024 };

	using GenerationType = std::conditional_t<internal::generationBits <= 16,
			std::conditional_t<internal::generationBits <= 8,
				u8,
				u16>,
			u32>;

	static_assert(sizeof(GenerationType) * 8 >= internal::generationBits); // cant need more bits than storage type can hold, ie. >= 32 bits
	static_assert(sizeof(IdType) - sizeof(GenerationType) > 0);

	constexpr auto isValid(IdType id) -> bool {
		return id != invalidId;
	}
	constexpr auto index(IdType id) -> IdType {
		IdType index{ id & internal::indexMask };
		assert(index != internal::indexMask);
		return index & internal::indexMask;
	}
	constexpr auto generation(IdType id) -> GenerationType {
		return (id >> internal::indexBits) & internal::generationMask;
	}
	constexpr auto newGeneration(IdType id) -> IdType {
		const IdType generation{ id::generation(id) + (IdType) 1 };
		assert(generation < (((u64) 1 << internal::generationBits) - 1));
		return index(id) | (generation << internal::indexBits);
	}

#if _DEBUG
	namespace internal {
		class IdBase { // debug wrapper
			IdType _id;
		public:
			constexpr explicit IdBase(IdType id) : _id{ id } {}
			constexpr operator IdType() const { return _id; } // u32 i = entityId; // this func used here to give the IdType rather than the wrapper
		};
	} // internal namespace
#define DEFINE_TYPED_ID(name)													\
	struct name final : dossyl::id::internal::IdBase {							\
		constexpr explicit name(dossyl::id::IdType id) : IdBase{ id } {}		\
		constexpr name() : IdBase { 0 } {}										\
	};
#else
#define DEFINE_TYPED_ID(name) using name = id::IdType;
#endif
}
