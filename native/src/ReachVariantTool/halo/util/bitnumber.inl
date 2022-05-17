#pragma once
#include "./bitnumber.h"

#define CLASS_TEMPLATE_PARAMS template<typename Underlying, typename ParamsType, ParamsType Params>
#define CLASS_NAME bitnumber<Underlying, ParamsType, Params>
//
namespace halo::util {
   CLASS_TEMPLATE_PARAMS
   template<class Stream> requires impl::bitnumber::read_bitstream<Stream, typename CLASS_NAME::underlying_uint>
   bool CLASS_NAME::_read_presence(Stream& stream) { // returns bool: value should be read?
      if constexpr (!has_presence)
         return true;
      bool bit = stream.read_bits(1);
      if (bit == params.presence.value())
         return true;
      if constexpr (params.if_absent.has_value()) {
         this->value = params.if_absent.value();
      } else {
         this->value = params.initial;
      }
      return false;
   }
}
//
#undef CLASS_TEMPLATE_PARAMS
#undef CLASS_NAME
