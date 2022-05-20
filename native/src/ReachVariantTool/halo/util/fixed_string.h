#pragma once
#include <array>
#include <limits>
#include <stdexcept>
#include <string>

namespace halo::util {
   template<
      typename Char,
      size_t Size,
      class Traits = std::char_traits<Char>
   > class fixed_string {
      public:
         using value_type  = Char;
         using size_type   = size_t;
         using traits_type = Traits;
         static constexpr size_type max_length = Size;

         static constexpr size_type npos = (std::numeric_limits<size_type>::max)();

         using pointer   = value_type*;
         using reference = value_type&;
         using const_pointer   = const value_type*;
         using const_reference = const value_type&;

      protected:
         std::array<value_type, Size + 1> _data = {}; // + 1 to ensure null terminator for c_str()

      public:
         using iterator       = typename decltype(_data)::iterator;
         using const_iterator = typename decltype(_data)::const_iterator;
         using reverse_iterator       = typename decltype(_data)::reverse_iterator;
         using const_reverse_iterator = typename decltype(_data)::const_reverse_iterator;

      public:
         constexpr fixed_string() {}
         constexpr fixed_string(const value_type* v) {
            size_type i = 0;
            for (; v[i]; ++i) {
               if (i >= max_length)
                  throw std::out_of_range("Value does not fit.");
               this->_data[i] = v[i];
            }
            for (; i < max_length; ++i)
               this->_data[i] = value_type(0);
            this->_data.back() = value_type(0); // ensure null terminator for c_str
         }
         template<size_type OtherSize> requires (OtherSize <= Size) constexpr fixed_string(const fixed_string<value_type, OtherSize>& o) noexcept {
            size_type i = 0;
            for (; i < OtherSize; ++i)
               this->_data[i] = o._data[i];
            for (; i < max_length; ++i)
               this->_data[i] = value_type(0);
            this->_data.back() = value_type(0); // ensure null terminator for c_str
         }

         inline constexpr size_type capacity() const { return max_length; }
         inline constexpr const_pointer c_str() const { return this->_data.data(); }
         inline constexpr pointer data() { return this->_data.data(); }
         inline constexpr const_pointer data() const { return this->_data.data(); }
         constexpr size_type size() const {
            for (size_type i = 0; i < max_length; ++i)
               if (this->_data[i] == '\0')
                  return i;
            return max_length;
         }
         inline constexpr size_type max_size() const { return max_length; }

         inline reference operator[](size_type i) { return this->_data[i]; }
         inline const_reference operator[](size_type i) const { return this->_data[i]; }

         void clear() {
            for (auto& c : this->_data)
               c = value_type(0);
         }

         inline reference front() { return this->_data.front(); }
         inline const_reference front() const { return this->_data.front(); }
         inline reference back() { return this->_data[max_length - 1]; }
         inline const_reference back() const { return this->_data[max_length - 1]; }

         inline const_iterator cbegin() const { return this->_data.begin(); }
         inline const_iterator cend() const { return cbegin() + size(); }
         inline const_iterator crbegin() const { return this->_data.crbegin() + (capacity() - size()); }
         inline const_iterator crend() const { return crend(); }

         inline iterator begin() { return this->_data.begin(); }
         inline const_iterator begin() const { cbegin(); }
         inline iterator end() { return begin() + size(); }
         inline const_iterator end() const { cend(); }

         inline iterator rbegin() { return this->_data.rbegin() + (capacity() - size()); }
         inline const_iterator rbegin() const { crbegin(); }
         inline iterator rend() { return rend(); }
         inline const_iterator rend() const { crend(); }
   };
}