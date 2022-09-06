#pragma once
#include <array>
#include <type_traits>

namespace cobb::array {
   template<typename Functor, typename ValueType, size_t Size>
      requires std::is_invocable<Functor, std::add_const_t<std::add_lvalue_reference_t<ValueType>>>::value
   consteval auto map(const std::array<ValueType, Size>& src, Functor&& mutator) {
      using ResultType = std::invoke_result_t<Functor, ValueType>;

      std::array<ResultType, Size> dst = {};
      for (size_t i = 0; i < Size; ++i) {
         dst[i] = mutator(src[i]);
      }
      return dst;
   }

   // Variant with index argument
   template<typename Functor, typename ValueType, size_t Size>
      requires std::is_invocable<Functor, std::add_const_t<std::add_lvalue_reference_t<ValueType>>, size_t>::value
   consteval auto map(const std::array<ValueType, Size>& src, Functor&& mutator) {
      using ResultType = std::invoke_result_t<Functor, ValueType, size_t>;

      std::array<ResultType, Size> dst = {};
      for (size_t i = 0; i < Size; ++i) {
         dst[i] = mutator(src[i], i);
      }
      return dst;
   }
}