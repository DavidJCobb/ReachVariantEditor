#pragma once
#include "./bitnumber.h"

#define CLASS_TEMPLATE_PARAMS template<size_t Bitcount, typename Underlying, bitnumber_params<Underlying> Params> requires (Bitcount > 0)
#define CLASS_NAME bitnumber<Bitcount, Underlying, Params>
//
namespace halo {
   CLASS_TEMPLATE_PARAMS bool CLASS_NAME::is_absent() const noexcept {
      if constexpr (!has_presence)
         return false;
      if constexpr (params.if_absent.has_value())
         return this->value == params.if_absent.value();
      return this->value == params.initial;
   }
   
   #pragma region Operators
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator=(const underlying_type& other) {
      this->value = other;
      return *this;
   }
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator+=(const underlying_type& other) requires (is_integer_type) {
      this->value += other;
      return *this;
   }
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator-=(const underlying_type& other) requires (is_integer_type) {
      this->value -= other;
      return *this;
   }
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator*=(const underlying_type& other) requires (is_integer_type) {
      this->value *= other;
      return *this;
   }
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator/=(const underlying_type& other) requires (is_integer_type) {
      this->value /= other;
      return *this;
   }
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator%=(const underlying_type& other) requires (is_integer_type) {
      this->value %= other;
      return *this;
   }
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator|=(const underlying_type& other) requires (is_integer_type) {
      this->value |= other;
      return *this;
   }
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator&=(const underlying_type& other) requires (is_integer_type) {
      this->value &= other;
      return *this;
   }
   CLASS_TEMPLATE_PARAMS CLASS_NAME& CLASS_NAME::operator^=(const underlying_type& other) requires (is_integer_type) {
      this->value ^= other;
      return *this;
   }
   #pragma endregion
}
//
#undef CLASS_TEMPLATE_PARAMS
#undef CLASS_NAME