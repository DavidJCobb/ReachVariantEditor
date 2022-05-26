#pragma once
#include <array>
#include <bitset>
#include <stdexcept>
#include <type_traits>

namespace halo::util {
   template<typename T, size_t Maximum> class ct_vector {
      public:
         using value_type = T;
         static constexpr size_t size_limit = Maximum;

      protected:
         using _storage_type = std::array<value_type, size_limit>;

         _storage_type _list = {};
         size_t _size = 0;

      public:
         using iterator = _storage_type::iterator;
         using const_iterator = _storage_type::const_iterator;
         using reverse_iterator = _storage_type::reverse_iterator;
         using const_reverse_iterator = _storage_type::const_reverse_iterator;

         iterator begin() { return this->_list.begin(); }
         const_iterator cbegin() const { return this->_list.cbegin(); }
         const_iterator begin() const { return cbegin(); }
         iterator end() { return begin() + _size; }
         const_iterator cend() { return cbegin() + _size; }
         reverse_iterator rend() { return this->_list.rend(); }
         reverse_iterator crend() { return this->_list.crend(); }
         const_reverse_iterator rend() const { return crend(); }
         reverse_iterator rbegin() { return this->_list.rbegin() + _size - 1; }

      public:
         constexpr ct_vector() {}

         template<typename... Types> requires (sizeof...(Types) <= size_limit && (std::is_same_v<Types, value_type> && ...))
         constexpr ct_vector(Types... args) : _list{{ std::forward<Types>(args)... }}, _size(sizeof...(Types)) {
         }

         constexpr size_t capacity() const { return size_limit; }
         constexpr size_t size() const { return this->_size; }

         constexpr value_type& operator[](size_t i) {
            if (std::is_constant_evaluated()) {
               if (i >= size_limit)
                  throw;
            }
            return this->_list[i];
         }
         constexpr const value_type& operator[](size_t i) const {
            if (std::is_constant_evaluated()) {
               if (i >= size_limit)
                  throw;
            }
            return this->_list[i];
         }
   };
}
