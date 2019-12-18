#pragma once
#include <cstdint>
#include <type_traits>
#include "polyfills_cpp20.h"
#include "templating.h"

namespace cobb {
   class bitwriter {
      protected:
         enum class _is_signed_sentinel {};
      public:
         static constexpr _is_signed_sentinel is_signed = _is_signed_sentinel();
      protected:
         uint8_t* _buffer = nullptr;
         uint32_t _size   = 0;
         uint32_t _bitpos = 0;
         //
         void _ensure_room_for(unsigned int bitcount) noexcept;
         //
      public:
         inline uint32_t get_bitpos()   const noexcept { return this->_bitpos; };
         inline uint32_t get_bytepos()  const noexcept { return this->_bitpos / 8; };
         inline int      get_bitshift() const noexcept { return this->_bitpos % 8; };
         //
         inline void enlarge_by(uint32_t bytes) noexcept {
            this->resize(this->_size + bytes);
         }
         inline void pad_bytes(uint32_t bytes) noexcept {
            while (bytes--)
               this->write(0, 8);
         }
         void pad_to_bytepos(uint32_t bytepos) noexcept;
         void resize(uint32_t size) noexcept;
         void write(uint32_t value, int bits, int& recurse_remaining) noexcept; // should naturally write in big-endian
         inline void write(const uint32_t value, int bits) noexcept {
            this->write(value, bits, bits);
         }
         inline void write(int32_t value, int bits, _is_signed_sentinel) noexcept {
            if (value < 0)
               value |= (1 << (bits - 1));
            this->write(value, bits, bits);
         }
         //
         template<typename T, cobb_enable_case(1, !std::is_bounded_array_v<T>)> void write(const T& value) noexcept {
            if (std::is_signed_v<T>)
               this->write((int32_t)value, cobb::bits_in<T>, is_signed);
            else
               this->write((uint32_t)value, cobb::bits_in<T>);
         };
         template<typename T, cobb_enable_case(2, std::is_bounded_array_v<T>)> void write(const T& value) noexcept {
            using item_type = std::remove_extent_t<T>; // from X[i] to X
            //
            for (int i = 0; i < std::extent<T>::value; i++)
               if (std::is_signed_v<item_type>)
                  this->write(value[i], cobb::bits_in<item_type>, is_signed);
               else
                  this->write(value[i], cobb::bits_in<item_type>);
         };
         template<int = 0> void read(const float& value) noexcept {
            union {
               uint32_t i;
               float    f = value;
            } value;
            this->write(value.i, 32);
         }
         template<int = 0> void read(const bool& value) noexcept {
            this->write(value ? 1 : 0, 1);
         }
   };
}