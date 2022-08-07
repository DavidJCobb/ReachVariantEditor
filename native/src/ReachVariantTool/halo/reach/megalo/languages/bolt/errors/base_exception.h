#pragma once
#include <stdexcept>
#include <type_traits>
#include "./base.h"

namespace halo::reach::megalo::bolt::errors {
   template<typename Data> requires std::is_base_of_v<base, Data>
   class scan_exception : public std::exception {
      public:
         scan_exception(const Data& d) : data(d) {}
         scan_exception(Data&& d) : data(std::move(d)) {}
      public:
         Data data = {};
   };

   template<typename Data> requires std::is_base_of_v<base, Data>
   class parse_exception : public std::exception {
      public:
         parse_exception(const Data& d) : data(d) {}
         parse_exception(Data&& d) : data(std::move(d)) {}
      public:
         Data data = {};
   };
}