#include "managed_pointer_list.h"
#include <cstdlib>
#include <stdexcept>

namespace cobb {
   _managed_pointer_list_impl::reference _managed_pointer_list_impl::at(size_type i) noexcept {
      if (i >= this->_size)
         throw std::out_of_range("");
      return (*this)[i];
   }
   _managed_pointer_list_impl::const_reference _managed_pointer_list_impl::at(size_type i) const noexcept {
      if (i >= this->_size)
         throw std::out_of_range("");
      return (*this)[i];
   }
   //
   void _managed_pointer_list_impl::clear() noexcept {
      this->_size = 0;
      if (this->_capacity > 50) { // arbitrary threshold for freeing the array
         free(this->_list);
         this->_list = nullptr;
         this->_capacity = 0;
      }
   }
   void _managed_pointer_list_impl::reserve(size_type c) {
      if (this->_capacity >= c)
         return;
      auto old  = this->_list;
      auto next = (value_type*)malloc(c * sizeof(value_type));
      for (size_type i = 0; i < c; i++) {
         if (i < this->_size) {
            next[i] = this->_list[i];
         } else {
            next[i] = value_type();
         }
      }
      this->_capacity = c;
      this->_list     = next;
      if (old)
         free(old);
   }
   void _managed_pointer_list_impl::resize(size_type newSize) {
      if (this->_size == newSize)
         return;
      if (this->_capacity < newSize) {
         this->reserve(newSize);
         for (size_type i = this->_size; i < newSize; i++)
            this->_list[i] = value_type();
         this->_size = newSize;
      } else {
         this->_size = newSize;
         if (this->_capacity / 2 > newSize)
            this->shrink_to_fit();
      }
   }
   void _managed_pointer_list_impl::shrink_to_fit() noexcept {
      auto old  = this->_list;
      auto next = (value_type*)malloc(this->_size * sizeof(value_type));
      for (size_type i = 0; i < this->_size; i++)
         if (i < this->_size)
            next[i] = this->_list[i];
      this->_capacity = this->_size;
      this->_list     = next;
      if (old)
         free(old);
   }
}