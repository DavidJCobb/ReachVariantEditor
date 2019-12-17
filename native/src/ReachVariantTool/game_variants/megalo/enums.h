#pragma once
#include <cstdint>
#include <string>
#include <type_traits>
#include "../../helpers/bitwise.h"

namespace Megalo {
   class SmartEnum {
      public:
         constexpr SmartEnum(const char** v, uint32_t n) : values(v), count(n) {}
         //
         const char** values = nullptr;
         uint32_t     count  = 0;
         //
         constexpr inline int count_bits() const noexcept {
            if (!this->count)
               return 0;
            return cobb::bitcount(this->count);
         }
         constexpr inline int index_bits() const noexcept {
            if (!this->count)
               return 0;
            return cobb::bitcount(this->count - 1);
         }
         //
         void to_string(std::string& out, uint32_t value) const noexcept;
         //
         const char* operator[](size_t a) const noexcept {
            if (a < this->count)
               return this->values[a];
            return nullptr;
         }
         //
         int32_t lookup(const char* value) const noexcept {
            for (uint32_t i = 0; i < this->count; i++)
               if (_stricmp(value, this->values[i]) == 0)
                  return i;
            return -1;
         }
         //
         SmartEnum* slice(size_t from, size_t to) const noexcept {
            size_t size = to - from;
            const char** pointers = (const char**)malloc(sizeof(const char*) * size);
            for (size_t i = 0; i < size; i++)
               pointers[i] = this->values[i + to];
            return new SmartEnum(pointers, size);
         }
   };
   #define megalo_define_smart_enum(name, ...) namespace { namespace __smart_enum_members { const char* _##name##[] = { __VA_ARGS__ }; } }; extern const ::Megalo::SmartEnum name = ::Megalo::SmartEnum(__smart_enum_members::_##name##, std::extent<decltype(__smart_enum_members::_##name##)>::value);

   class SmartFlags {
      public:
         constexpr SmartFlags(const char** v, uint32_t n) : values(v), count(n) {}
         //
         const char** values = nullptr;
         uint32_t     count  = 0;
         //
         constexpr inline int bits() const noexcept {
            return this->count;
         }
         //
         void to_string(std::string& out, uint32_t value) const noexcept;
   };
   #define megalo_define_smart_flags(name, ...) namespace { namespace __smart_flags_members { const char* _##name##[] = { __VA_ARGS__ }; } }; extern const ::Megalo::SmartFlags name = ::Megalo::SmartFlags(__smart_flags_members::_##name##, std::extent<decltype(__smart_flags_members::_##name##)>::value);
}