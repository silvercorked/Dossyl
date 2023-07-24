#pragma once

#include <stdint.h>

// unsigned integers
using u8	= uint8_t;
using u16	= uint16_t;
using u32	= uint32_t;
using u64	= uint64_t;

// signed integers
using s8	= int8_t;
using s16	= int16_t;
using s32	= int32_t;
using s64	= int64_t;

// unsigned invalid values
constexpr u8	u8InvalidId		{ 0xffui8 };
constexpr u16	u16InvalidId	{ 0xffffui16 };
constexpr u32	u32InvalidId	{ 0xffff'ffffui32 };
constexpr u64	u64InvalidId	{ 0xffff'ffff'ffff'ffffui64 };

// floating point
using f32 = float;
