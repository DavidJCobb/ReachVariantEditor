#pragma once
#include <bit>
#include <cstdint>
#include "helpers/byteswap.h"
#include "helpers/memory.h"
#include "helpers/type_traits/strip_enum.h"
#include "halo/util/fixed_string.h"
#include "halo/util/has_read_method.h"
#include "halo/util/load_process.h"
#include "halo/util/stream_position.h"
#include "halo/load_process_base.h"

namespace halo {
   namespace impl {
      class bytereader_base {
         public:
            using basic_position_type = util::stream_position;
            using position_type       = util::full_stream_position;

         protected:
            const uint8_t* _buffer = nullptr;
            size_t _size = 0;
            position_type _position;
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
            inline uint32_t get_overshoot() const noexcept { return this->_position.overshoot.bytes; }

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
            void set_position(size_t bytepos);

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

   template<
      typename Subclass,
      typename LoadProcess
   > class bytereader : public impl::bytereader_base {
      public:
         using my_type   = Subclass;
         using base_type = bytereader<Subclass, LoadProcess>; // helper for call-super; useful if your subclass is, say, some_namespace::bitreader

         using load_process_type = std::remove_reference_t<LoadProcess>;

         static constexpr bool shares_load_process = std::is_reference_v<LoadProcess>;

      protected:
         template<typename T> static constexpr size_t bitcount_of_type = std::bit_width(std::numeric_limits<std::make_unsigned_t<cobb::strip_enum_t<T>>>::max());

      protected:
         LoadProcess _load_process;

      public:
         bytereader() requires (!shares_load_process) {}
         bytereader(load_process_type& lp) requires (shares_load_process) : _load_process(lp) {}

         load_process_type& load_process() const requires (shares_load_process) {
            return this->_load_process;
         }
         load_process_type& load_process() requires (!shares_load_process) {
            return this->_load_process;
         }
         const load_process_type& load_process() const requires (!shares_load_process) {
            return this->_load_process;
         }

         // Multi-read call
         template<typename... Types> requires (sizeof...(Types) > 1)
         void read(Types&... args) {
            (((Subclass*)this)->read(args), ...);
         }
         template<std::endian Endianness, typename... Types> requires (sizeof...(Types) > 1)
         void read(Types&... args) {
            auto e = this->endianness();
            this->set_endianness(Endianness);
            (((Subclass*)this)->read(args), ...);
            this->set_endianness(e);
         }
         
         #pragma region read
         template<typename T> requires util::has_read_method<my_type, T>
         void read(T& v) {
            v.read(*((Subclass*)this));
         }

         template<typename CharT, size_t size>
         void read(util::fixed_string<CharT, size>& out) {
            this->_read_impl(out.data(), size * sizeof(CharT));
            if constexpr (sizeof(CharT) > 1) {
               for (auto& c : out)
                  this->_apply_endianness(c);
            }
         }

         template<typename T> requires (!std::is_pointer_v<T> && !util::has_read_method<my_type, T> && !util::is_fixed_string<T>)
         void read(T& out) {
            this->_read_impl(&out, sizeof(T));
            this->_apply_endianness(out);
         }

         template<typename T> requires (!std::is_pointer_v<T>)
         void read(T* list, size_t count) {
            for (size_t i = 0; i < count; ++i)
               ((Subclass*)this)->read(*list[i]);
         }

         template<typename T, size_t count> requires (!std::is_pointer_v<T>)
         void read(std::array<T, count>& out) {
            for (auto& item : out)
               ((Subclass*)this)->read(item);
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

         #pragma region Load process messages
         void emit_notice(load_process_base::notice&& m) { this->_load_process.emit_notice(std::forward(m)); }
         void emit_warning(load_process_base::warning&& m) { this->_load_process.emit_warning(std::forward(m)); }
         void emit_error(load_process_base::error&& m) { this->_load_process.emit_error(std::forward(m)); }
         void throw_fatal(load_process_base::fatal&& m) { this->_load_process.throw_fatal(std::forward(m)); }

         void emit_notice(const load_process_base::notice& m) { this->_load_process.emit_notice(std::forward(m)); }
         void emit_warning(const load_process_base::warning& m) { this->_load_process.emit_warning(std::forward(m)); }
         void emit_error(const load_process_base::error& m) { this->_load_process.emit_error(std::forward(m)); }
         void throw_fatal(const load_process_base::fatal& m) { this->_load_process.throw_fatal(std::forward(m)); }

         template<typename T> requires load_process_message_wraps_content<T> void emit_notice(const typename T::message_content& info) {
            this->_load_process.set_stream_offsets(this->_position.bytes, this->_position.bits);
            this->_load_process.template emit_notice<T>(info);
         }
         template<typename T> requires load_process_message_wraps_content<T> void emit_warning(const typename T::message_content& info) {
            this->_load_process.set_stream_offsets(this->_position.bytes, this->_position.bits);
            this->_load_process.template emit_warning<T>(info);
         }
         template<typename T> requires load_process_message_wraps_content<T> void emit_error(const typename T::message_content& info) {
            this->_load_process.set_stream_offsets(this->_position.bytes, this->_position.bits);
            this->_load_process.template emit_error<T>(info);
         }
         template<typename T> requires load_process_message_wraps_content<T> void throw_fatal(const typename T::message_content& info) {
            this->_load_process.set_stream_offsets(this->_position.bytes, this->_position.bits);
            this->_load_process.template throw_fatal<T>(info);
         }
         
         template<typename T> requires load_process_message_wraps_content<T> void emit_notice_at(const typename T::message_content& info, const basic_position_type& pos) {
            this->_load_process.set_stream_offsets(pos.bytes, pos.bits);
            this->_load_process.template emit_notice<T>(info);
         }
         template<typename T> requires load_process_message_wraps_content<T> void emit_warning_at(const typename T::message_content& info, const basic_position_type& pos) {
            this->_load_process.set_stream_offsets(pos.bytes, pos.bits);
            this->_load_process.template emit_warning<T>(info);
         }
         template<typename T> requires load_process_message_wraps_content<T> void emit_error_at(const typename T::message_content& info, const basic_position_type& pos) {
            this->_load_process.set_stream_offsets(pos.bytes, pos.bits);
            this->_load_process.template emit_error<T>(info);
         }
         template<typename T> requires load_process_message_wraps_content<T> void throw_fatal_at(const typename T::message_content& info, const basic_position_type& pos) {
            this->_load_process.set_stream_offsets(pos.bytes, pos.bits);
            this->_load_process.template throw_fatal<T>(info);
         }
         #pragma endregion
   };
}
