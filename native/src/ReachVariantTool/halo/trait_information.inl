#pragma once
#include <bit>
#include <limits>
#include <type_traits>
#include "./trait_information.h"

namespace halo {
   constexpr trait_information::trait_information(impl::bare_trait_information d) : impl::bare_trait_information(d) {
      uint8_t range = (max) - (min);
      if (std::is_constant_evaluated()) {
         if (range < 0)
            throw;
      }
      if (this->bitcount == 0) {
         this->bitcount = range ? std::bit_width((uint8_t)range) : 1;
      } else {
         if (std::is_constant_evaluated()) {
            if (std::bit_width(range) > bitcount)
               throw;
         }
      }
   }
}

#define CLASS_TEMPLATE_PARAMS template<typename T>
#define CLASS_NAME trait_integer_cast_info<T>
//
namespace halo::impl {
   CLASS_TEMPLATE_PARAMS constexpr size_t CLASS_NAME::end_of_range() const {
      size_t i;
      for (i = range.size() - 1; i >= 1; --i)
         if (range[i])
            break;
      return i + 1;
   }

   CLASS_TEMPLATE_PARAMS constexpr int CLASS_NAME::maximum() const {
      int i = std::numeric_limits<int>::min();
      for (size_t j = 0; j < end_of_range(); ++j)
         if (range[j] > i)
            i = range[j];
      return i;
   }

   CLASS_TEMPLATE_PARAMS constexpr int CLASS_NAME::minimum() const {
      int i = std::numeric_limits<int>::max();
      for (size_t j = 0; j < end_of_range(); ++j)
         if (range[j] < i)
            i = range[j];
      return i;
   }
}
//
#undef CLASS_TEMPLATE_PARAMS
#undef CLASS_NAME

namespace halo::impl {
   constexpr size_t type_erased_trait_integer_cast_info::end_of_range() const {
      size_t i;
      for (i = range.size() - 1; i >= 1; --i)
         if (range[i])
            break;
      return i + 1;
   }

   constexpr int type_erased_trait_integer_cast_info::maximum() const {
      int i = std::numeric_limits<int>::min();
      for (size_t j = 0; j < end_of_range(); ++j)
         if (range[j] > i)
            i = range[j];
      return i;
   }

   constexpr int type_erased_trait_integer_cast_info::minimum() const {
      int i = std::numeric_limits<int>::max();
      for (size_t j = 0; j < end_of_range(); ++j)
         if (range[j] < i)
            i = range[j];
      return i;
   }
}

#define CLASS_TEMPLATE_PARAMS template<typename T, trait_integer_cast_info<T> Info>
#define CLASS_NAME trait_integer_cast_helpers<T, Info>
//
namespace halo::impl {
   CLASS_TEMPLATE_PARAMS /*static*/ constexpr int CLASS_NAME::_to_integer(T v) {
      int i = (int)v - (int)info.first_integer;
      if (i < info.end_of_range())
         return info.range[i];
      cobb::unreachable();
   }
   CLASS_TEMPLATE_PARAMS /*static*/ constexpr T CLASS_NAME::_from_integer(int v) {
      if (v < info.minimum())
         return info.below_range;
      if (v > info.maximum())
         return info.above_range;
      for (size_t i = 0; i < info.end_of_range(); ++i) {
         if (v == info.range[i])
            return (T)(i + (size_t)info.first_integer);
      }
      //
      // Pick the value nearest to what's been passed in:
      //
      size_t smallest_diff  = std::numeric_limits<size_t>::max();
      size_t smallest_index = 0;
      for (size_t i = 0; i < info.end_of_range(); ++i) {
         auto item = info.range[i];
         //
         auto diff = v - item;
         if (diff < 0)
            diff = -diff;
         if (diff < smallest_diff) {
            smallest_diff  = diff;
            smallest_index = i;
         }
      }
      return (T)(smallest_index + (size_t)info.first_integer);
   }
}
//
#undef CLASS_TEMPLATE_PARAMS
#undef CLASS_NAME