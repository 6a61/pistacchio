#pragma once

#include <cstdint>
#include <memory>

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8  = uint8_t;

using s64 = int64_t;
using s32 = int32_t;
using s16 = int16_t;
using s8  = int8_t;

using f32 = float;
using f64 = double;

template<class Ty>
using sptr = std::shared_ptr<Ty>;

template<class Ty>
using uptr = std::unique_ptr<Ty>;
