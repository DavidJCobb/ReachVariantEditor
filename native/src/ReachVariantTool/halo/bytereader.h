#pragma once
#include <bit>
#include <cstdint>
#include "helpers/byteswap.h"
#include "helpers/memory.h"
#include "helpers/type_traits/strip_enum.h"
#include "./util/fixed_string.h"
#include "./util/has_read_method.h"
#include "./util/load_process.h"

namespace halo {
   namespace impl {
      class bytereader_base {
         protected:
            const uint8_t* _buffer = nullptr;
            size_t _size = 0;
            struct {
               size_t  overflow = 0; // number of bits by which we have passed the end of the file
               size_t  bytes    = 0;
            } _position;
            std::endian _endianness = std::endian::native;

            void _read_impl(void*, size_t);
            void _peek_impl(void*, size_t);

            template<typename T> void _apply_endianness(T& out) {
               if constexpr (std::is_integral_v<T> && sizeof(T) > 1) {
                  if (this->endianness() != std::endian::native)
                     out = cobb::byteswap(out);
               }
            }

         public:
            bytereader_base() {}

            inline const uint8_t* data() const noexcept { return this->_buffer; }
            inline uint32_t size() const noexcept { return this->_size; }
            inline size_t get_position() const noexcept { return this->_position.bytes; }
            inline uint32_t get_overshoot() const noexcept { return this->_position.overflow / 8; }

            inline std::endian endianness() const noexcept { return this->_endianness; }
            void set_endianness(std::endian);

            const uint8_t* data_at_current_position() const;

            bool is_at_end() const noexcept {
               return this->_position.bytes >= this->size();
            }
            bool is_in_bounds(uint32_t bytes = 0) const noexcept {
               if (bytes)
                  return this->_position.bytes + bytes <= this->size();
               return this->_position.bytes < this->size();
            }

            void set_buffer(const uint8_t* b, size_t s);
            void set_position(uint32_t bytepos);

            cobb::generic_buffer read_buffer(size_t size) {
               auto out = cobb::generic_buffer(size);
               this->_read_impl(out.data(), size);
               return out;
            }

            template<typename T>
            void peek(T& out) const noexcept {
               this->_peek_impl(&out, sizeof(T));
               this->_apply_endianness(out);
            }

            void skip(size_t bytecount);
            
      };
   }

   template<typename LoadProcess> class bytereader : public impl::bytereader_base {
      public:
         using load_process_type = std::remove_reference_t<LoadProcess>;

         static constexpr bool has_load_process    = util::load_process<load_process_type>;
         static constexpr bool shares_load_process = has_load_process && std::is_reference_v<LoadProcess>;

      protected:
         template<typename T> static constexpr size_t bitcount_of_type = std::bit_width(std::numeric_limits<std::make_unsigned_t<cobb::strip_enum_t<T>>>::max());

         using load_process_member_types = util::extract_load_process_types<load_process_type>;
         struct _dummy {};

      protected:
         std::conditional_t<!has_load_process, _dummy, LoadProcess> _load_process;

      public:
         bytereader() requires (!shares_load_process) {}
         bytereader(load_process_type& lp) requires (shares_load_process) : _load_process(lp) {}

         load_process_type& load_process() requires (!std::is_same_v<load_process_type, void>) {
            return this->_load_process;
         }
         const load_process_type& load_process() const requires (!std::is_same_v<load_process_type, void>) {
            return this->_load_process;
         }

         // Multi-read call
         template<typename... Types> requires (sizeof...(Types) > 1)
            void read(Types&... args) {
            (this->read(args), ...);
         }
         template<std::endian Endianness, typename... Types> requires (sizeof...(Types) > 1)
            void read(Types&... args) {
            auto e = this->endianness();
            this->set_endianness(Endianness);
            (this->read(args), ...);
            this->set_endianness(e);
         }
         
         #pragma region read
         template<typename T> requires util::has_read_method<bytereader, T>
         void read(T& v) {
            v.read(*this);
         }

         template<typename CharT, size_t size>
         void read(util::fixed_string<CharT, size>& out) {
            this->_read_impl(out.data(), size * sizeof(CharT));
            if constexpr (sizeof(CharT) > 1) {
               for (auto& c : out)
                  this->_apply_endianness(c);
            }
         }

         template<typename T> requires (!std::is_pointer_v<T> && !util::has_read_method<bytereader, T> && !util::is_fixed_string<T>)
         void read(T& out) {
            this->_read_impl(&out, sizeof(T));
            this->_apply_endianness(out);
         }

         template<typename T> requires (!std::is_pointer_v<T>)
         void read(T* list, size_t count) {
            for (size_t i = 0; i < count; ++i)
               this->read(*list[i]);
         }

         template<typename T, size_t count> requires (!std::is_pointer_v<T>)
         void read(std::array<T, count>& out) {
            for (auto& item : out)
               this->read(item);
         }
         #pragma endregion

         bytereader slice_forward(size_t size) const {
            if constexpr (shares_load_process) {
               bytereader out(this->load_process());
               out.set_buffer(this->data_at_current_position(), this->size() - this->get_position());
               return out;
            } else {
               bytereader out;
               out.set_buffer(this->data_at_current_position(), this->size() - this->get_position());
               return out;
            }
         }
   };
}
