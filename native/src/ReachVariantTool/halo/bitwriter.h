#pragma once
#include <bit>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#include "helpers/type_traits/strip_enum.h"
#include "halo/util/has_write_method.h"
#include "halo/util/stream_position.h"

namespace halo {
   namespace impl::bitwriter {
      template<typename T> concept is_bitwriteable = std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_same_v<T, bool>;

      template<typename T> concept is_indexed_list = requires(T& x) {
         typename T::value_type;
         typename T::entry_type;
         requires std::is_same_v<typename T::value_type*, typename T::entry_type>;
         { T::max_count } -> std::same_as<const size_t&>;
         { x[0] } -> std::same_as<typename T::value_type&>;
      };

      template<typename Writer, typename T> concept is_bitwriteable_indexed_list = requires(const T& x) {
         requires is_indexed_list<T>;
         typename T::value_type;
         requires (is_bitwriteable<typename T::value_type> || util::has_write_method<Writer, typename T::value_type>);
      };
   }

   template<
      typename Subclass//, // CRTP
   > class bitwriter {
      public:
         using base_type = bitwriter<Subclass>;
         using my_type   = Subclass;

      protected:
         uint8_t* _buffer = nullptr;
         size_t   _size   = 0; // in bytes
         util::stream_position _position;

      protected:
         template<typename T> static constexpr size_t bitcount_of_type = std::bit_width(std::numeric_limits<std::make_unsigned_t<cobb::strip_enum_t<T>>>::max());

      protected:
         uint8_t& _access_byte(size_t bytepos) const noexcept;
         void _ensure_room_for(unsigned int bitcount);

         template<typename T> void _write(T value, int bits, int& recurse_remaining);

      public:
         ~bitwriter();

         inline uint8_t* data() noexcept { return this->_buffer; }
         inline const uint8_t* data() const noexcept { return this->_buffer; }
         inline uint32_t size() const noexcept { return this->_size; }

         inline uint32_t get_bitpos() const noexcept { return this->_position.in_bits(); }
         inline uint32_t get_bitshift() const noexcept { return this->_position.bits; }
         inline uint32_t get_bytepos() const noexcept { return this->_position.bytes; }
         inline uint32_t get_bytespan() const noexcept { return this->_position.bytespan(); }

         void enlarge_by(size_t bytes);
         void reserve(size_t bytes);
         void resize(size_t bytes);
         

         // Multi-write call
         template<typename... Types> requires (sizeof...(Types) > 1)
         void write(const Types&... args) {
            (((my_type*)this)->write(args), ...);
         }
         
         #pragma region write
         template<typename T> requires util::has_write_method<my_type, T>
         void write(const T& value) {
            value.write(*(my_type*)this);
         }

         template<typename T> requires (!util::has_write_method<my_type, T> && impl::bitwriter::is_bitwriteable<T>)
         void write(const T value) {
            if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
               this->write_bits<bool>(1, value);
            } else if constexpr (std::is_same_v<std::decay_t<T>, float>) {
               this->write_bits<uint32_t>(32, std::bit_cast<uint32_t, T>(value));
            } else {
               this->write_bits<std::decay_t<T>>(bitcount_of_type<cobb::strip_enum_t<std::decay_t<T>>>, value);
            }
         }

         template<typename T> requires (impl::bitwriter::is_bitwriteable_indexed_list<my_type, T>)
         void write(const T& list) {
            this->write_bits(std::bit_width(T::max_count), list.size());
            for (const auto& item : list)
               ((my_type*)this)->write(item);
         }

         template<typename T, size_t count> requires (util::has_write_method<my_type, T> || impl::bitwriter::is_bitwriteable<T>)
         void write(const std::array<T, count>& list) {
            for (const auto& item : list)
               ((my_type*)this)->write(item);
         }
         #pragma endregion

         template<typename T> requires impl::bitwriter::is_bitwriteable<T> void write_bits(size_t bitcount, T value);
         
         void write_compressed_float(float value, const int bitcount, float min, float max, bool is_signed, bool guarantee_exact_bounds);

         template<typename CharT> void write_string(const CharT*, size_t max_length);

         template<typename T> void write_bitstream(const bitwriter<T>&);
   };

   template<typename T> concept bitwriter_subclass = requires(T& x) {
      typename T::my_type;
      requires std::is_same_v<T, typename T::my_type>;

         requires std::is_base_of_v<
         bitwriter<
            typename T::my_type
         >,
         typename T::my_type
      >;
   };

   class basic_bitwriter : public bitwriter<basic_bitwriter> {};
}

#include "bitwriter.inl"