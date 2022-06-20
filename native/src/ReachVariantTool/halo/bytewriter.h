#include <bit>
#include <cstdint>
#include <type_traits>
#include "helpers/type_traits/strip_enum.h"
#include "helpers/byteswap.h"
#include "halo/util/fixed_string.h"
#include "halo/util/has_write_method.h"
#include "halo/util/stream_position.h"

namespace halo {
   namespace impl {
      class bytewriter_base {
         public:
            using basic_position_type = util::stream_position;
            using position_type       = util::full_stream_position;

         protected:
            uint8_t* _buffer = nullptr;
            size_t   _size   = 0; // in bytes
            util::stream_position _position;

            void _ensure_room_for(size_t);

         public:
            ~bytewriter_base();

            inline uint8_t* data() const noexcept { return this->_buffer; }
            inline uint32_t size() const noexcept { return this->_size; }
            inline size_t get_position() const noexcept { return this->_position.bytes; }

            uint8_t* data_at(size_t) const noexcept;
            uint8_t* destination() const noexcept;

            void write(const void* out, size_t length);
            void write_to_offset(size_t offset, const void* out, size_t length);

            void enlarge_by(size_t bytes) {
               this->resize(this->_size + bytes);
            }
            void pad(size_t bytes) {
               this->_ensure_room_for(bytes);
               memset(this->destination(), 0, bytes);
               this->_position.advance_by_bytes(bytes);
            }
            void skip(size_t bytes) {
               this->_position.advance_by_bytes(bytes);
            }
            void pad_to_bytepos(uint32_t bytepos);
            void resize(size_t size);
      };

      namespace bytewriter {
         template<typename T> concept bytewriteable = requires {
            requires !std::is_pointer_v<T>;
         };

         template<typename T> struct _raw_type_for {
            using type = void;
         };
         template<typename T> requires (sizeof(T) == 1) struct _raw_type_for<T> { using type = uint8_t; };
         template<typename T> requires (sizeof(T) == 2) struct _raw_type_for<T> { using type = uint16_t; };
         template<typename T> requires (sizeof(T) == 4) struct _raw_type_for<T> { using type = uint32_t; };
         template<typename T> requires (sizeof(T) == 8) struct _raw_type_for<T> { using type = uint64_t; };

         template<typename T> using raw_type_for = _raw_type_for<T>::type;

         template<typename T> concept supports_endianness = requires {
            requires bytewriteable<T>;
            requires (std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T>);
            requires !std::is_same_v<raw_type_for<T>, void>;
         };

         template<typename T> concept has_underlying_type = requires (const T& x) {
            typename T::underlying_type;
            { (typename T::underlying_type)x };
         };
         template<typename T> concept underlying_type_supports_endianness = requires {
            typename T::underlying_type;
            requires has_underlying_type<T>;
            requires supports_endianness<typename T::underlying_type>;
         };

         template<typename T> auto as_raw(T v) {
            if constexpr (underlying_type_supports_endianness<T>) {
               return (raw_type_for<typename T::underlying_type>)(typename T::underlying_type)(v);
            } else {
               return (raw_type_for<T>)(v);
            }
         }
      }
   }

   template<
      typename Subclass
   > class bytewriter : public impl::bytewriter_base {
      public:
         using base_type = bytewriter<Subclass>;
         using my_type   = Subclass;

         using impl::bytewriter_base::write;

      public:
         // Multi-write call (no endianness)
         template<typename... Types> requires ((sizeof...(Types) > 1) && (impl::bytewriter::bytewriteable<Types> && ...))
         void write(Types&... args) {
            (((my_type*)this)->write(args), ...);
         }

         // Multi-write call (endianness)
         template<std::endian E, typename... Types> requires ((sizeof...(Types) > 1) && (impl::bytewriter::bytewriteable<Types> && ...))
         void write(Types&... args) {
            (((my_type*)this)->template write<E, Types>(args), ...);
         }

         template<typename CharT, size_t size>
         void write(const util::fixed_string<CharT, size>& out) {
            bytewriter_base::write(out.data(), size * sizeof(CharT));
         }

         template<typename T> requires impl::bytewriter::bytewriteable<T>
         void write(T v) {
            if constexpr (util::has_write_method<my_type, T>) {
               v.write(*(my_type*)this);
            } else {
               if constexpr (impl::bytewriter::supports_endianness<T>) {
                  this->write<std::endian::little>(v);
               } else if constexpr (impl::bytewriter::underlying_type_supports_endianness<T>) {
                  this->write<std::endian::little>(v);
               } else {
                  bytewriter_base::write(&v, sizeof(T));
               }
            }
         }
         template<typename T> requires impl::bytewriter::bytewriteable<T>
         void write_to_offset(size_t offset, T v) {
            if constexpr (util::has_write_method<my_type, T>) {
               auto pos = this->get_position();
               this->_position.bytes = offset;
               v.write(*(my_type*)this);
               this->_position.bytes = pos;
            } else {
               if constexpr (impl::bytewriter::supports_endianness<T>) {
                  this->write_to_offset<std::endian::little>(offset, v);
               } else if constexpr (impl::bytewriter::underlying_type_supports_endianness<T>) {
                  this->write_to_offset<std::endian::little>(offset, v);
               } else {
                  bytewriter_base::write_to_offset(offset, &v, sizeof(T));
               }
            }
         }

         template<std::endian E, typename T> requires ((impl::bytewriter::supports_endianness<T> && !util::has_write_method<my_type, T>) || impl::bytewriter::underlying_type_supports_endianness<T>)
         void write(T v) {
            auto value = impl::bytewriter::as_raw(v);
            //
            using raw_type = decltype(value);
            if constexpr (sizeof(raw_type) > 1) {
               if constexpr (E != std::endian::native)
                  value = cobb::byteswap(value);
            }
            bytewriter_base::write(&value, sizeof(raw_type));
         }
         template<std::endian E, typename T> requires ((impl::bytewriter::supports_endianness<T> && !util::has_write_method<my_type, T>) || impl::bytewriter::underlying_type_supports_endianness<T>)
         void write_to_offset(size_t offset, T v) {
            auto value = impl::bytewriter::as_raw(v);
            //
            using raw_type = decltype(value);
            if constexpr (sizeof(raw_type) > 1) {
               if constexpr (E != std::endian::native)
                  value = cobb::byteswap(value);
            }
            bytewriter_base::write_to_offset(offset, &value, sizeof(raw_type));
         }
   };
}