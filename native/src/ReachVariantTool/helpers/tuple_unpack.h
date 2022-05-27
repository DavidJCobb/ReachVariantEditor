#pragma once
#include <tuple>

namespace cobb {
   template<template<typename...> class UnpackInto, typename Tuple> struct tuple_unpack;
   template<template<typename...> class UnpackInto, typename... Types> struct tuple_unpack<UnpackInto, std::tuple<Types...>> {
      using type = UnpackInto<Types...>;
   };

   template<template<typename...> class UnpackInto, typename Tuple> using tuple_unpack_t = typename tuple_unpack<UnpackInto, Tuple>::type;
}