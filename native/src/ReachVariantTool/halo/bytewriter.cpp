#include "bytewriter.h"

namespace halo::impl {
   void bytewriter_base::_ensure_room_for(size_t count) {
      size_t target = this->get_position() + count;
      if (target > this->_size)
         this->resize(target);
   }

   bytewriter_base::~bytewriter_base();

   uint8_t* bytewriter_base::data_at(size_t offset) const noexcept {
      return this->_buffer + offset;
   }
   uint8_t* bytewriter_base::destination() const noexcept {
      return this->data_at(this->get_position());
   }

   void bytewriter_base::write(const void* out, size_t length) {
      this->_ensure_room_for(length);
      memcpy(this->destination(), out, length);
      this->_position.advance_by_bytes(length);
   }
   void bytewriter_base::write_to_offset(size_t offset, const void* out, size_t length) {
      if (offset + length > this->_size)
         this->resize(offset + length);
      memcpy(this->data_at(offset), out, length);
   }

   void bytewriter_base::pad_to_bytepos(uint32_t bytepos) {
      auto current = this->get_position();
      if (bytepos <= current)
         return;
      if (bytepos > this->_size)
         this->resize(bytepos);
      memset(this->data_at(current), 0, bytepos - current);
      this->_position.set_in_bytes(bytepos);
   }
   void bytewriter_base::resize(size_t size) {
      if (this->_size == size)
         return;
      this->_buffer = (uint8_t*)realloc(this->_buffer, size);
   }
}