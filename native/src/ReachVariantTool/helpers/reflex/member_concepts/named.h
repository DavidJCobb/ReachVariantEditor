#pragma once
#include "../member.h"
#include "../member_range.h"
#include "../nested_enum.h"

namespace cobb::reflex::member_concepts {
   template<typename T> concept named = requires {
      requires (is_member<T> || is_member_range<T> || is_nested_enum<T>);
      { T::name };
   };
}