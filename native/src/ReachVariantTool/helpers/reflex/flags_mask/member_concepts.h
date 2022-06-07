#pragma once
#include <type_traits>
#include "../member.h"
#include "../member_range.h"

namespace cobb::reflex::impl::flags_mask {
   template<typename T> concept is_underlying_type = std::is_integral_v<T> && std::is_arithmetic_v<T>;

   template<typename T> concept is_valid_member = is_member<T> || is_member_range<T>;
}