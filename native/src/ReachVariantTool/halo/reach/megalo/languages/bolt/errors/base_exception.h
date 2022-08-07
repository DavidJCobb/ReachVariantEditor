#pragma once
#include <stdexcept>
#include <type_traits>
#include "./base.h"

namespace halo::reach::megalo::bolt::errors {
   class bolt_language_exception : public std::exception {
      public:
         virtual ~bolt_language_exception() {}
   };

   template<typename Data> requires std::is_base_of_v<base, Data>
   class scan_exception : public bolt_language_exception {
      public:
         scan_exception(const Data& d) : data(d) {}
         scan_exception(Data&& d) : data(std::move(d)) {}
      public:
         Data data = {};
   };

   template<typename Data> requires std::is_base_of_v<base, Data>
   class parse_exception : public bolt_language_exception {
      public:
         parse_exception(const Data& d) : data(d) {}
         parse_exception(Data&& d) : data(std::move(d)) {}
      public:
         Data data = {};
   };
}