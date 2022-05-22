#include "bytereader.h"

namespace halo {
   namespace impl {
      const uint8_t* bytereader_base::data_at_current_position() const {
         return this->_buffer + this->_position.bytes;
      }

      void bytereader_base::_read_impl(void* out, size_t size) {
         this->_peek_impl(out, size);
         this->_position.bytes += size;
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
      void bytereader_base::set_position(uint32_t bytepos) {
         this->_position.bytes = bytepos;
         if (bytepos > this->size()) {
            this->_position.bytes    = this->size();
            this->_position.overflow = (bytepos - this->size()) * 8;
         } else
            this->_position.overflow = 0;
      }

      void bytereader_base::skip(size_t bytecount) {
         this->set_position(this->get_position() + bytecount);
      }
   }
}