#pragma once
#include <type_traits>
//#include "../../nested_enum.h"

namespace cobb::reflex2 {
   class member;
   class member_range;
}

namespace cobb::reflex2::impl::enumeration {
   template<typename T> concept is_real_member = std::is_same_v<T, member> || std::is_same_v<T, member_range> /*|| std::is_same_v<T, nested_enum>*/;
}
