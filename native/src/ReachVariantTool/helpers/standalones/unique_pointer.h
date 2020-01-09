#pragma once

namespace cobb {
   template<typename T> class unique_pointer {
      //
      // A stripped-down counterpart to std::unique_ptr. This class has no support for 
      // bounded arrays or custom deleters, but it loads without including the full 
      // content of <memory>, which can turn out to be quite large.
      //
      // It also implicitly casts to a regular pointer, allowing you to pass it directly 
      // to functions that take a regular pointer as an argument.
      //
      public:
         using element_type = T;
         using pointer = T*;
         //
      protected:
         pointer _data = nullptr;
         //
      public:
         unique_pointer() {}
         unique_pointer(pointer v) : _data(v) {}
         unique_pointer(const unique_pointer& other) = delete; // no copy-assign
         unique_pointer(unique_pointer&& other) { // move-construct
            this->_data = other._data;
            other._data = nullptr;
         }
         ~unique_pointer() {
            if (this->_data) {
               delete this->_data;
               this->_data = nullptr;
            }
         }
         //
         inline pointer get() const noexcept { return this->_data; }
         inline pointer operator->() const noexcept { return this->_data; }
         inline element_type& operator*() const noexcept { return *this->_data; }
         inline operator bool() const noexcept { return this->_data != nullptr; }
         inline operator pointer() const noexcept { return this->_data; }
         //
         pointer release() noexcept {
            auto p = this->_data;
            this->_data = nullptr;
            return p;
         }
         void reset(pointer p) noexcept {
            if (this->_data)
               delete this->_data;
            this->_data = p;
         }
         void swap(unique_pointer& other) noexcept {
            auto o = other._data;
            other._data = this->_data;
            this->_data = o;
         }
         //
         unique_pointer& operator=(pointer v) noexcept {
            if (this->_data)
               delete this->_data;
            this->_data = v;
         }
         unique_pointer& operator=(const unique_pointer& other) = delete; // no copy-assign
         unique_pointer& operator=(unique_pointer&& other) noexcept {
            if (this->_data)
               delete this->_data;
            this->_data = other._data;
            other._data = nullptr;
            return *this;
         } // move-assign
   };
   template<typename T, class... Args> unique_pointer<T> make_unique(Args&&... args) {
      return unique_pointer<T>(new T(static_cast<Args&&>(args)...)); // the static_cast is what std::forward does; I didn't feel like including <utility>
   }
}
namespace std {
   template<typename T> constexpr void swap(::cobb::unique_pointer<T>& a, ::cobb::unique_pointer<T>& b) noexcept {
      a.swap(b);
   }
}