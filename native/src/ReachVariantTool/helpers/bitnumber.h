#pragma once
#include <cstdint>
#include <type_traits>
#include "stream.h"
#include "bitwriter.h"
#include "bytewriter.h"
#include "type_traits.h"

namespace cobb {
   struct bitnumber_no_presence_bit { static constexpr bool value = false; };

   /*
      bitnumber<4, sint8_t, false, 0, std::true_type, -1>  jumpHeight; // a bit indicates whether a 4-bit value is present; if that bit is true,  then load the value
      bitnumber<4, sint8_t, false, 0, std::false_type, -1> jumpHeight; // a bit indicates whether a 4-bit value is present; if that bit is false, then load the value
   */

   template<
      int        bitcount,   // number of bits that the value is encoded as
      typename   underlying, // type to use to hold the value in-memory / type the value is encoded as when not in a bitstream
      bool       offset       = false, // if true, the value is incremented by 1 when saved and must be decremented when loaded; used to avoid sign issues with small-bitcount values
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
      // The design goal for this class was to make it easier to work with files that 
      // store bit-aligned data: if you use bitnumbers to hold the data, then you only 
      // need to specify the bitcount and related details in one place (the field 
      // definition) rather than two (all read code and all write code).
      //
      public:
         using underlying_type = underlying;
         using underlying_int  = cobb::strip_enum_t<underlying_type>; // int type if (underlying_type) is an enum
         using underlying_uint = std::make_unsigned_t<underlying_int>;
         static constexpr bool is_integer_type = std::is_same_v<underlying_type, underlying_int>;
         static constexpr int  bitcount        = bitcount;
         static constexpr bool uses_offset     = offset;
         static constexpr bool has_presence    = !std::is_same_v<presence_bit, bitnumber_no_presence_bit>;
         static constexpr int  max_bitcount    = bitcount + (has_presence ? 1 : 0);
         
         underlying_type value = underlying_type{};
         
         bitnumber() {};
         bitnumber(int v) : value(underlying_type(v)) {};
         bitnumber(underlying_type v) requires !std::is_same_v<int, underlying_type> : value(v) {};
         
      protected:
         bool _read_presence(cobb::ibitreader& stream) { // returns bool: value should be read?
            if constexpr (!has_presence)
               return true;
            bool bit = stream.read_bits(1);
            if (bit == presence_bit::value)
               return true;
            this->value = underlying_type(if_absent);
            return false;
         }
         bool _write_presence(cobb::bitwriter& stream) const noexcept { // returns bool: value should be written?
            if constexpr (!has_presence)
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
         constexpr bool uses_presence() const noexcept {
            return !std::is_same_v<presence_bit, bitnumber_no_presence_bit>;
         }
         constexpr bool write_as_signed() const noexcept {
            return std::is_signed_v<underlying_type> && !this->uses_presence() && !uses_offset;
         }
         //
         bool is_absent() const noexcept {
            if constexpr (!has_presence)
               return false;
            return this->value == if_absent;
         }
         inline bool is_present() const noexcept { return !this->is_absent(); }
         //
         void read(cobb::ibitreader& stream) noexcept {
            if (!this->_read_presence(stream))
               return;
            this->value = underlying_type((underlying_int)stream.read_bits<underlying_uint>(bitcount) - (uses_offset ? 1 : 0));
            if (this->write_as_signed())
               //
               // We have to apply the sign bit ourselves, or (offset) will break some signed 
               // values. Main example is mpvr::activity, which is incremented by 1 before 
               // saving (in case its value is -1) and then serialized as a 3-bit number.
               //
               this->value = underlying_type(cobb::apply_sign_bit((underlying_int)this->value, bitcount));
         }
         void write(cobb::bitwriter& stream) const noexcept {
            if (!this->_write_presence(stream))
               return;
            stream.write((underlying_int)this->value + (uses_offset ? 1 : 0), bitcount, this->write_as_signed());
         }
         //
         void read(cobb::ibytereader& stream) noexcept {
            stream.read(this->value);
         }
         void write(cobb::bytewriter& stream) const noexcept {
            stream.write(this->value);
         }
         //
         underlying_int to_int() const noexcept { return (underlying_int)this->value; } // printf helper to avoid C4477

         #pragma region Operators
         template<typename V> requires std::is_convertible_v<V, underlying_type>
         bitnumber& operator=(const V& other) noexcept {
            this->value = other;
            return *this;
         }

         template<typename V> requires std::is_convertible_v<V, underlying_type>
         bitnumber& operator+=(const V& other) noexcept {
            this->value += other;
            return *this;
         }

         template<typename V> requires std::is_convertible_v<V, underlying_type>
         bitnumber& operator-=(const V& other) noexcept {
            this->value -= other;
            return *this;
         }

         template<typename V> requires std::is_convertible_v<V, underlying_type>
         bitnumber& operator*=(const V& other) noexcept {
            this->value *= other;
            return *this;
         }

         template<typename V> requires std::is_convertible_v<V, underlying_type>
         bitnumber& operator/=(const V& other) noexcept {
            this->value /= other;
            return *this;
         }

         template<typename V> requires std::is_convertible_v<V, underlying_type>
         bitnumber& operator%=(const V& other) noexcept {
            this->value %= other;
            return *this;
         }

         template<typename V> requires std::is_convertible_v<V, underlying_type>
         bitnumber& operator|=(const V& other) noexcept {
            this->value |= other;
            return *this;
         }

         template<typename V> requires std::is_convertible_v<V, underlying_type>
         bitnumber& operator&=(const V& other) noexcept {
            this->value &= other;
            return *this;
         }

         template<typename V> requires std::is_convertible_v<V, underlying_type>
         bitnumber& operator^=(const V& other) noexcept {
            this->value ^= other;
            return *this;
         }

         // Having an implicit cast-to-int for enums breaks switch-cases. :(
         operator underlying_type() const noexcept { return this->value; }; // implicitly creates compare operators if underlying_type is integer
         
         template<typename T> explicit operator T() const noexcept { // needed for explicit enum-to-int casts
            return (T)this->value;
         }
         #pragma endregion
   };
   class bitbool {
      public:
         using underlying_type = bool;
         static constexpr int  bitcount     = 1;
         static constexpr int  max_bitcount = bitcount;
         static constexpr bool uses_offset  = false;
         //
         using presence_bit = bitnumber_no_presence_bit;
         static constexpr underlying_type if_absent = false;
         //
         underlying_type value = false;
         //
         bitbool() {};
         bitbool(bool v) : value(v) {};
         //
         void read(cobb::ibitreader& stream) noexcept {
            this->value = stream.read_bits<int>(1);
         }
         void read(cobb::ibytereader& stream) noexcept {
            stream.read(&this->value, 1);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            stream.write(this->value);
         }
         void write(cobb::bytewriter& stream) const noexcept {
            stream.write(&this->value, 1);
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