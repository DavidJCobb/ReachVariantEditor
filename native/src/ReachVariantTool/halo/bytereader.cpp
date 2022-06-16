#include "bytereader.h"

namespace halo {
   namespace impl {
      const uint8_t* bytereader_base::data_at_current_position() const {
         return this->_buffer + this->_position.bytes;
      }

      void bytereader_base::_read_impl(void* out, size_t size) {
         this->_peek_impl(out, size);
         this->_position.advance_by_bytes(size);
         this->_position.clamp_to_size(this->size());
      }
      void bytereader_base::_peek_impl(void* out, size_t size) {
         if (!this->is_in_bounds(size)) {
            memset(out, 0, size);
            return;
         }
         auto* src = this->data() + this->get_position();
         memcpy(out, src, size);
      }

      void bytereader_base::set_endianness(std::endian e) {
         this->_endianness = e;
      }

      void bytereader_base::set_buffer(const uint8_t* b, size_t s) {
         this->_buffer   = b;
         this->_size     = s;
         this->_position = {};
      }
      void bytereader_base::set_position(size_t bytepos) {
         this->_position.set_in_bytes(bytepos);
         this->_position.clamp_to_size(this->size());
      }

      void bytereader_base::skip(size_t bytecount) {
         this->_position.advance_by_bytes(bytecount);
         this->_position.clamp_to_size(this->size());
      }
   }
}