#pragma once
#include <cstdint>
#include <type_traits>
#include "bitstream.h"
#include "bitwriter.h"
#include "type_traits.h"

namespace cobb {
   struct bitnumber_no_presence_bit { static constexpr bool value = false; };

   /*
      bitnumber<4, sint8_t, false, 0, std::true_type, -1>  jumpHeight; // a bit indicates whether a 4-bit value is present; if that bit is true,  then load the value
      bitnumber<4, sint8_t, false, 0, std::false_type, -1> jumpHeight; // a bit indicates whether a 4-bit value is present; if that bit is false, then load the value
   */

   /*//
   //
   // WIP: class that does everything at run-time rather than compile-time; combined with some 
   // other stuff it'd allow me to define lists of fields and enumerate over them to read or 
   // save them.
   //
   class bitnum {
      protected:
         int      underlying_size; // in bytes
         bool     underlying_signed = false;
         bool     swap_on_read = false;
         int      offset = 0;
         bool     uses_presence_bit = false;
         bool     presence_bit      = false;
         int64_t  if_absent         = 0;
         //
         int bitcount;
         union {
            uint64_t u64;
            int64_t  i64;
            uint32_t u32;
            int32_t  i32;
            uint16_t u16;
            int16_t  i16;
            uint8_t  u8;
            int8_t   i8;
         } value;
         //
         template<typename T> void _setUnderlying() {
            this->underlying_size   = sizeof(T);
            this->underlying_signed = std::is_signed_v<T>;
         }
         //
         enum class _presence_bit_none_sentinel {};
         //
      public:
         static constexpr _presence_bit_none_sentinel no_presence_bit = _presence_bit_none_sentinel();
         //
         template<typename T> bitnum(
            //
            int     bc,
            bool    presence_bit,
            int64_t if_absent    = 0,
            bool    swap_on_read = false,
            int     offset       = 0
            //
         ) : bitcount(bc) {
            this->_setUnderlying<T>();
            this->uses_presence_bit = true;
            this->presence_bit = presence_bit;
            this->if_absent    = if_absent;
            this->swap_on_read = swap_on_read;
            this->offset       = offset;
         };
         template<typename T> bitnum(
            //
            int bc,
            _presence_bit_none_sentinel presence_bit = no_presence_bit,
            int64_t if_absent    = 0,
            bool    swap_on_read = false,
            int     offset       = 0
            //
         ) : bitcount(bc) {
            this->_setUnderlying<T>();
            this->swap_on_read = swap_on_read;
            this->offset       = offset;
         };
   };
   //*/

   template<
      int        bitcount,   // number of bits that the value is encoded as
      typename   underlying, // type to use to hold the value in-memory / type the value is encoded as when not in a bitstream
      bool       swap         = false, // whether to bitswap the value after loading it as bits
      int        offset       = 0, // offset added to the file before saving; must be subtracted when loaded
      typename   presence_bit = bitnumber_no_presence_bit, // if std::true_type or std::false_type, then the game writes a bit indicating whether the value is present; if the bit equals the specified type, the value is present
      underlying if_absent    = underlying() // if the bitnumber has a presence bit, this value is assigned when the bit indicates absence
   > class bitnumber {
      //
      // A class which represents a number that is encoded into a bit-aligned stream, e.g. 
      // a 3-bit number or a 5-bit number. The class is designed to handle the following 
      // quirks:
      //
      // swap
      //    For numbers that need to have their bit order swapped after being read.
      //
      // offset
      //    For numbers that are incremented by some value before being encoded, and must 
      //    be decremented by the same value after being decoded, e.g. numbers that use 
      //    negative values as sentinels in-memory while being incremented to positive 
      //    to avoid sign bit issues in a file format.
      //
      // presence
      //    For numbers that have a "presence bit." If the presence bit is equal to a 
      //    certain bool, then the number's value follows it; otherwise, no value is 
      //    encoded into the file and the number should be given a default during read.
      //
      public:
         using underlying_type = underlying;
         using underlying_int  = cobb::strip_enum_t<underlying_type>; // int type if (underlying_type) is an enum
         static constexpr int  bitcount        = bitcount;
         static constexpr bool bitswap_on_read = swap;
         static constexpr int  value_offset    = offset;
         //
         underlying_type value = underlying_type();
         //
         bitnumber() {};
         bitnumber(int v) : value(underlying_type(v)) {};
         template<typename = std::enable_if_t<!std::is_same_v<int, underlying_type>>> bitnumber(underlying_type v) : value(v) {};
         //
      protected:
         static constexpr uint32_t read_flags = (bitswap_on_read ? bitstream_read_flags::swap : 0);
         //
         bool _read_presence(cobb::bitstream& stream) { // returns bool: value should be read?
            if (!this->uses_presence())
               return true;
            bool bit = stream.read_bits(1);
            if (bit == presence_bit::value)
               return true;
            this->value = underlying_type(if_absent);
            return false;
         }
         bool _write_presence(cobb::bitwriter& stream) const noexcept { // returns bool: value should be written?
            if (!this->uses_presence())
               return true;
            bool presence = presence_bit::value;
            if (this->value == if_absent) {
               stream.write(!presence, 1);
               return false;
            }
            stream.write(presence, 1);
            return true;
         }
      public:
         bool uses_presence() const noexcept {
            return !std::is_same_v<presence_bit, bitnumber_no_presence_bit>;
         }
         //
         void read(cobb::bitstream& stream) noexcept {
            if (!this->_read_presence(stream))
               return;
            this->value = underlying_type(stream.read_bits<underlying_int>(bitcount, read_flags) - value_offset);
            if (std::is_signed_v<underlying_type>)
               //
               // We have to apply the sign bit ourselves, or (offset) will break some signed 
               // values. Main example is mpvr::activity, which is incremented by 1 before 
               // saving (in case its value is -1) and then serialized as a 3-bit number.
               //
               this->value = underlying_type(cobb::apply_sign_bit((underlying_int)this->value, bitcount));
         }
         void read(cobb::bytestream& stream) noexcept {
            stream.read(this->value);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            if (!this->_write_presence(stream))
               return;
            stream.write((underlying_int)this->value + value_offset, bitcount, std::is_signed_v<underlying_type> && !this->uses_presence() && !value_offset);
         }
         void write_bits(cobb::bitwriter& stream) const noexcept {
            this->write(stream);
         }
         void write_bytes(cobb::bitwriter& stream) const noexcept {
            stream.write((underlying_int)this->value);
         }
         //
         // Operators:
         //
         bitnumber& operator=(const underlying_type& other) noexcept {
            this->value = other;
            return *this;
         }
         bitnumber& operator+=(const underlying_type& other) noexcept {
            this->value += other;
            return *this;
         }
         bitnumber& operator-=(const underlying_type& other) noexcept {
            this->value -= other;
            return *this;
         }
         bitnumber& operator*=(const underlying_type& other) noexcept {
            this->value *= other;
            return *this;
         }
         bitnumber& operator/=(const underlying_type& other) noexcept {
            this->value /= other;
            return *this;
         }
         bitnumber& operator%=(const underlying_type& other) noexcept {
            this->value %= other;
            return *this;
         }
         //
         operator underlying_int() const noexcept { return (underlying_int)this->value; }; // this cast is used implicitly for conversion operators, so we don't need to define overloads for those
   };
   class bitbool {
      public:
         using underlying_type = bool;
         static constexpr int  bitcount        = 1;
         static constexpr bool bitswap_on_read = false;
         static constexpr int  value_offset    = 0;
         //
         using presence_bit = bitnumber_no_presence_bit;
         static constexpr underlying_type if_absent = false;
         //
         underlying_type value = false;
         //
         bitbool() {};
         bitbool(bool v) : value(v) {};
         //
         void read(cobb::bitstream& stream) noexcept {
            this->value = stream.read_bits<int>(1);
         }
         void read(cobb::bytestream& stream) noexcept {
            stream.read(this->value);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            stream.write(this->value);
         }
         void write_bits(cobb::bitwriter& stream) const noexcept {
            stream.write(this->value);
         }
         void write_bytes(cobb::bitwriter& stream) const noexcept {
            stream.write((uint8_t)this->value);
         }
         //
         operator underlying_type() const noexcept { return this->value; };
   };

   // bytenumber
   // Helper class. If you have a value that actually gets stored as a whole e.g. a 
   // 16-bit or 32-bit number, this class will wrap that number type in the same 
   // interface as bitnumber.
   //
   template<typename T> using bytenumber = bitnumber<cobb::bits_in<T>, T>;
};